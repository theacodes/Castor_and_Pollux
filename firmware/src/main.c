/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "fix16.h"
#include "gem.h"
#include "printf.h"
#include "sam.h"
#include <stdlib.h>
#include <string.h>

/* Forward declarations */
static wntr_periodic_waveform_function lfo_waveshape_setting_to_func_(uint8_t n);
static RAMFUNC void pulse_ovf_callback_(uint8_t inst);

/* Configuration */
static uint32_t board_revision_;
static const struct GemADCConfig* adc_cfg_;
static const struct GemADCInput* adc_inputs_;
static const struct GemOscillatorInputConfig* osc_input_cfg_;
static const struct GemPulseOutConfig* pulse_cfg_;
static const struct GemI2CConfig* i2c_cfg_;
static const struct GemSPIConfig* spi_cfg_;
static const struct GemDotstarCfg* dotstar_cfg_;

/* Inputs */
static uint32_t adc_results_live_[GEM_IN_COUNT];
static uint32_t adc_results_snapshot_[GEM_IN_COUNT];
static uint32_t* adc_results_ = adc_results_live_;
static struct WntrButton button_;

/* State */
static struct GemSettings settings_;
static struct {
    wntr_periodic_waveform_function functions[2];
    fix16_t frequencies[2];
    fix16_t factors[2];
    fix16_t phases[2];
} lfo_state_;
static struct WntrMixedPeriodicWaveform lfo_;
static struct GemOscillator castor_;
static struct GemOscillator pollux_;
static enum GemMode mode_ = GEM_MODE_NORMAL;

/* Timekeeping */
static uint32_t animation_time_ = 0;
static uint32_t sample_time_ = 0;
static uint32_t idle_cycles_ = 0;

/*
    Initializes the core processor, clocks, peripherals, drivers, settings,
    and global state.
*/
static void init_() {
    /*
        Core system initialization.
    */

    /*
        Since the peripheral configuration depends on the hardware revision,
        the firmware must detect which hardware revision. This is done by
        connecting otherwise unused GPIO pins to ground. The firmware checks
        the value of those pins to determine the hardware revision.
    */
    WntrGPIOPin_set_as_input(GEM_REV5_PIN, true);

    // rev5 pin is floating for revisions < 5, so it gets pulled up.
    if (WntrGPIOPin_get(GEM_REV5_PIN) == true) {
        board_revision_ = 4;
        adc_cfg_ = &GEM_REV1_ADC_CFG;
        adc_inputs_ = GEM_REV1_ADC_INPUTS;
        osc_input_cfg_ = &GEM_REV1_OSC_INPUT_CFG;
        pulse_cfg_ = &GEM_REV1_PULSE_OUT_CFG;
        i2c_cfg_ = &GEM_REV1_I2C_CFG;
        spi_cfg_ = &GEM_REV1_SPI_CFG;
        dotstar_cfg_ = &GEM_REV1_DOTSTAR_CFG;
    }
    // rev5 pin is tied to ground in revisions > 5.
    else {
        board_revision_ = 5;
        adc_cfg_ = &GEM_REV5_ADC_CFG;
        adc_inputs_ = GEM_REV5_ADC_INPUTS;
        osc_input_cfg_ = &GEM_REV5_OSC_INPUT_CFG;
        pulse_cfg_ = &GEM_REV5_PULSE_OUT_CFG;
        i2c_cfg_ = &GEM_REV5_I2C_CFG;
        spi_cfg_ = &GEM_REV5_SPI_CFG;
        dotstar_cfg_ = &GEM_REV5_DOTSTAR_CFG;
    }

    /* Tell the world who we are and how we got here. :) */
    printf("Hello, I am Gemini.\n - hardware: rev%u\n - firmware: %s\n", board_revision_, wntr_build_info_string());

    /*
        Peripheral setup
    */

    /* Gemini uses USB MIDI for editing settings and factory configuration. */
    wntr_usb_init();

    /* Gemini uses i2c to communicate with the external DAC. */
    gem_i2c_init(i2c_cfg_);
    gem_mcp_4728_init(i2c_cfg_);

    /* Gemini uses SPI to communicate with the Dotstar LEDs. */
    gem_spi_init(spi_cfg_);

    /*
        Global state initialization.
    */

    /*
        Gemini uses a pseudo-random number generator for the LED animation.
        To keep things simple, it just uses its serial number as the seed.
        If it needs to be more fancy in the future it could be changed to read
        a floating ADC input and use that as the seed.
    */
    uint8_t serial_number[WNTR_SERIAL_NUMBER_LEN];
    wntr_serial_number(serial_number);
    wntr_random_init(*((uint32_t*)(serial_number)));

    /*
        Gemini stores the user's settings in NVM so they have to be explicitly
        loaded.
    */
    GemSettings_load(&settings_);
    GemSettings_print(&settings_);

    /*
        Gemini also stores a ramp table in NVM. This table is used to
        compensate for amplitude loss in the ramp waveform as frequency
        increases.
    */
    gem_ramp_table_load();

    /*
        Driver configuration.
    */

    /* Register SysEx commands used for factory setup. */
    gem_sysex_init(adc_inputs_, i2c_cfg_, pulse_cfg_);

    /* Enable the Dotstar driver and LED animation. */
    gem_dotstar_init(settings_.led_brightness);
    gem_led_animation_init();

    /*
        Gemini tries to make the most out of SAMD21's ADC.

        First, it uses digital offset and gain error correction which is
        measured during assembly and stored in the user settings. This gives
        Gemini significantly more accurate readings.

        Second, it measures all of the input channels in the background using
        "channel scanning". This frees up the main loop to do other things while
        waiting for new measurements for all the channels.
    */
    gem_adc_init(adc_cfg_, settings_.adc_offset_corr, settings_.adc_gain_corr);

    for (size_t i = 0; i < GEM_IN_COUNT; i++) { gem_adc_init_input(&(adc_inputs_[i])); }
    gem_adc_start_scanning(adc_inputs_, GEM_IN_COUNT, adc_results_live_);

    /*
        Gemini uses the WntrButton helper for the hard sync button since it
        needs to know when the button is tapped vs held.
    */
    button_.pin = button_pin_.pin;
    button_.port = button_pin_.port;
    WntrButton_init(&button_);

    /*
        Oscillator configuration and initialization.
    */

    /*
        Gemini has an internal low-frequency oscillator that can be used to
        modulate the pitch and pulse width of the oscillators.

        It's current limited to just combining two waveforms, but it could
        support more.
    */
    lfo_state_.functions[0] = lfo_waveshape_setting_to_func_(settings_.lfo_1_waveshape);
    lfo_state_.functions[1] = lfo_waveshape_setting_to_func_(settings_.lfo_2_waveshape);
    lfo_state_.frequencies[0] = settings_.lfo_1_frequency;
    lfo_state_.frequencies[1] = fix16_mul(settings_.lfo_1_frequency, settings_.lfo_2_frequency_ratio);
    lfo_state_.factors[0] = settings_.lfo_1_factor;
    lfo_state_.factors[1] = settings_.lfo_2_factor;
    lfo_state_.phases[0] = F16(0);
    lfo_state_.phases[1] = F16(0);
    WntrMixedPeriodicWaveform_init(
        &lfo_, 2, lfo_state_.functions, lfo_state_.frequencies, lfo_state_.factors, lfo_state_.phases, wntr_ticks());

    /*
        Gemini has two oscillators - Castor & Pollux. For the most part they're
        completely independent: they each have their own pitch and pulse width
        inputs, their own pitch knob range configuration, and their own
        dedicated outputs.

        They share some configuration: the ADC error adjustment, pitch
        knob non-linearity, and smoothie settings.
    */
    gem_oscillator_init(
        (struct WntrErrorCorrection){.offset = settings_.cv_offset_error, .gain = settings_.cv_gain_error},
        settings_.pitch_knob_nonlinearity);

    castor_ = (struct GemOscillator){
        .number = 0,
        .pitch_offset = settings_.base_cv_offset,
        .pitch_cv_min = osc_input_cfg_->pitch_cv_min,
        .pitch_cv_max = osc_input_cfg_->pitch_cv_max,
        .lfo_pitch_factor = settings_.chorus_max_intensity,
        .pitch_knob_min = settings_.castor_knob_min,
        .pitch_knob_max = settings_.castor_knob_max,
        .pulse_width_bitmask = settings_.pulse_width_bitmask,
        .can_follow = false,
        .nonzero_threshold = settings_.pollux_follower_threshold,
    };
    GemOscillator_init(&castor_);

    pollux_ = (struct GemOscillator){
        .number = 1,
        .pitch_offset = settings_.base_cv_offset,
        .pitch_cv_min = osc_input_cfg_->pitch_cv_min,
        .pitch_cv_max = osc_input_cfg_->pitch_cv_max,
        .lfo_pitch_factor = settings_.chorus_max_intensity,
        .pitch_knob_min = settings_.pollux_knob_min,
        .pitch_knob_max = settings_.pollux_knob_max,
        .pulse_width_bitmask = settings_.pulse_width_bitmask,
        // If Pollux doesn't have any pitch CV input it'll follow Castor's
        // pitch. Rev 1-4 detects lack of pitch CV input by checking if Pollux's
        // pitch CV is near zero. Rev 5 has a switched jack, but still does
        // the near zero check to follow Castor when both pitch inputs are
        // unpatched.
        .nonzero_threshold = settings_.pollux_follower_threshold,
        // Pollux allows the LFO to modulate its pitch. This allows the
        // "chorusing" effect when the two oscillators have matching pitches
        // and their outputs are mixed.
        .apply_lfo_to_pitch = true,
    };
    GemOscillator_init(&pollux_);

    /* Configure the square wave output for the oscillators */
    gem_pulseout_init(pulse_cfg_, pulse_ovf_callback_);
}

static wntr_periodic_waveform_function lfo_waveshape_setting_to_func_(uint8_t n) {
    switch (n) {
        case 0:
            return wntr_triangle;
        case 1:
            return wntr_sine;
        case 2:
            return wntr_sawtooth;
        case 3:
            return wntr_square;
        default:
            return wntr_triangle;
    }
}

/*
    This callback is called from the pulseout overflow/underflow callback.
*/
static void pulse_ovf_callback_(uint8_t inst) {
    (void)inst;
    if (mode_ == GEM_MODE_HARD_SYNC) {
        TCC1->CTRLBSET.reg = TCC_CTRLBSET_CMD_RETRIGGER;
    }
}

static inline __attribute__((always_inline)) void update_dac_() {
    /*
        Update the DAC outputs with the new charge and pulse width
        values.

        Each oscillator requires two DAC outputs.

        The first one is used to compensation for amplitude loss as frequency
        increases. Higher voltage allows the ramp core's integrating capacitor
        to charge more quickly and reach a higher voltage before the timer
        resets the ramp.

        The second is used to control the pulse-width of the pulse waveform.
        The output voltage goes into a comparator that compares against the
        ramp waveform to generate a pulse.
    */
    if (board_revision_ >= 5) {
        gem_mcp_4728_write_channels(
            i2c_cfg_,
            (struct GemMCP4278Channel){.value = pollux_.ramp_cv},
            (struct GemMCP4278Channel){.value = pollux_.pulse_width},
            (struct GemMCP4278Channel){.value = castor_.ramp_cv},
            (struct GemMCP4278Channel){.value = castor_.pulse_width});
    } else {
        gem_mcp_4728_write_channels(
            i2c_cfg_,
            (struct GemMCP4278Channel){.value = castor_.ramp_cv},
            (struct GemMCP4278Channel){.value = castor_.pulse_width},
            (struct GemMCP4278Channel){.value = pollux_.ramp_cv},
            (struct GemMCP4278Channel){.value = pollux_.pulse_width});
    }
}

/*
    This task handles taking in the input state (from the ADC and PORT) and
    updating the oscillators, recalculating their outputs, and applying the
    outputs to the pulse generators and DACs.
*/

static RAMFUNC void oscillator_task_() {
    uint32_t now = wntr_ticks();

    /*
        Update the internal LFO used for modulating pitch and pulse width.
    */
    fix16_t lfo_value = WntrMixedPeriodicWaveform_step(&lfo_, now);
    gem_led_tweak_data.lfo_value = lfo_value;
    fix16_t lfo_amplitude = UINT12_NORMALIZE(adc_results_[GEM_IN_CHORUS_POT]);

    /*
        Update both oscillator's internal state based on the ADC inputs.
    */
    struct GemOscillatorInputs inputs = {
        .pitch_cv_code = adc_results_[GEM_IN_CV_A],
        .pitch_knob_code = adc_results_[GEM_IN_CV_A_POT],
        .pulse_cv_code = adc_results_[GEM_IN_DUTY_A],
        .pulse_knob_code = adc_results_[GEM_IN_DUTY_A_POT],
        .lfo_amplitude = lfo_amplitude,
        .lfo_phase = lfo_value,
    };

    GemOscillator_update(&castor_, inputs);

    /*
        Make Pollux follow Castor's pitch if there's nothing connected to
        Pollux's pitch CV input.

        If GemOscillator_update detects no input it'll use the value set here.
    */
    inputs = (struct GemOscillatorInputs){
        .pitch_cv_code = adc_results_[GEM_IN_CV_B],
        .pitch_knob_code = adc_results_[GEM_IN_CV_B_POT],
        .pulse_cv_code = adc_results_[GEM_IN_DUTY_B],
        .pulse_knob_code = adc_results_[GEM_IN_DUTY_B_POT],
        .lfo_amplitude = lfo_amplitude,
        .lfo_phase = lfo_value,
        .reference_pitch = castor_.pitch,
    };
    pollux_.pitch = castor_.pitch;
    GemOscillator_update(&pollux_, inputs);

    // Oscillator post-update applies final values to the oscillator state.
    // TODO: I can maybe put this in update()?
    GemOscillator_post_update(pulse_cfg_, &castor_);
    GemOscillator_post_update(pulse_cfg_, &pollux_);

    /*
        Update the timers with their new values calculated from their
        oscillator's pitch.

        It's important that these get updated at essentially the same time so
        that they have a stable phase relationship. Therefore, interrupts are
        disabled while Gemini modifies the timer configuration.
    */
    __disable_irq();
    gem_pulseout_set_period(pulse_cfg_, 0, castor_.pulseout_period);
    gem_pulseout_set_period(pulse_cfg_, 1, pollux_.pulseout_period);
    __enable_irq();

    update_dac_();
}

static uint16_t last_loop_time_ = 0;

static RAMFUNC void monitor_task_() {
    if (!gem_sysex_monitor_enabled()) {
        return;
    }

    /*
        To help with testing and debugging, Gemini can send its state over
        MIDI SysEx to the monitoring script in `/factory/monitor.py`.
    */

    uint16_t loop_time = (uint16_t)(wntr_ticks() - last_loop_time_);

    struct GemMonitorUpdate monitor_update = {
        .castor_pitch_knob = gem_oscillator_calc_pitch_knob(
            castor_.pitch_knob_min,
            castor_.pitch_knob_max,
            settings_.pitch_knob_nonlinearity,
            adc_results_live_[GEM_IN_CV_A_POT]),
        .castor_pitch_cv =
            gem_oscillator_calc_pitch_cv(castor_.pitch_cv_min, castor_.pitch_cv_max, adc_results_live_[GEM_IN_CV_A]),
        .castor_pulse_width_knob = adc_results_live_[GEM_IN_DUTY_A_POT],
        .castor_pulse_width_cv = adc_results_live_[GEM_IN_DUTY_A],
        .pollux_pitch_knob = gem_oscillator_calc_pitch_knob(
            pollux_.pitch_knob_min,
            pollux_.pitch_knob_max,
            settings_.pitch_knob_nonlinearity,
            adc_results_live_[GEM_IN_CV_B_POT]),
        .pollux_pitch_cv =
            gem_oscillator_calc_pitch_cv(pollux_.pitch_cv_min, pollux_.pitch_cv_max, adc_results_live_[GEM_IN_CV_B]),
        .pollux_pulse_width_knob = adc_results_live_[GEM_IN_DUTY_B_POT],
        .pollux_pulse_width_cv = adc_results_live_[GEM_IN_DUTY_B],
        .button_state = button_.state,
        // TODO: Re-enable
        //.lfo_intensity = pitch_lfo_intensity,
        .loop_time = loop_time,
        .animation_time = (uint16_t)(animation_time_),
        .sample_time = (uint16_t)(idle_cycles_)};

    gem_sysex_send_monitor_update(&monitor_update);

    last_loop_time_ = wntr_ticks();
}

/*
    This task is responsible for switching modes as well as dealing with the
    "tweak" interface: when the button is held down the user can customize
    module settings using the input knobs.
*/

#define IF_WAGGLED(variable, channel)                                                                                  \
    uint16_t variable = adc_results_live_[channel];                                                                    \
    int32_t variable##_delta = variable - adc_results_snapshot_[channel];                                              \
    if (labs(variable##_delta) > 50) {
#define IF_WAGGLED_END }

static RAMFUNC void mode_and_tweak_task_() {
    WntrButton_update(&button_);

    // If the button was just tapped the change to the next mode.
    if (WntrButton_tapped(&button_)) {
        mode_ = (mode_ + 1) % GEM_MODE_COUNT;
        gem_led_animation_set_mode(mode_);
    }

    // Tweak mode takes over the interface, so don't let the
    // oscillator_step task see changes to the ADC inputs while we're
    // in tweak mode.
    //
    // This is done by taking a "snapshot" of the current ADC readings and
    // pointing oscillator_step's `adc_results` to the snapshot. When
    // exiting tweak mode the `adc_results` pointer is set back to the
    // live ADC results.
    if (WntrButton_hold_started(&button_)) {
        memcpy(adc_results_snapshot_, adc_results_live_, GEM_IN_COUNT * sizeof(adc_results_live_[0]));
        adc_results_ = adc_results_snapshot_;
        gem_led_animation_set_mode(GEM_MODE_FLAG_TWEAK);
    }

    // Tweak!
    if (WntrButton_held(&button_)) {
        /* Chorus intensity knob controls the LFO frequency in tweak mode. */
        IF_WAGGLED(chorus_pot_code, GEM_IN_CHORUS_POT)
            fix16_t frequency_value = UINT12_NORMALIZE(chorus_pot_code);
            lfo_state_.frequencies[0] = fix16_mul(frequency_value, GEM_TWEAK_MAX_LFO_FREQ);
        IF_WAGGLED_END
    }

    // If we just left tweak mode, undo the ADC result trickery.
    if (WntrButton_hold_ended(&button_)) {
        adc_results_ = adc_results_live_;
        gem_led_animation_set_mode(mode_);
    }
}

/* Gemini uses MIDI for factory setup and for the settings editor. */
static void midi_task_() {
    struct WntrMIDIMessage msg = {};
    if (!wntr_midi_receive(&msg)) {
        return;
    }
    if (msg.code_index == MIDI_CODE_INDEX_SYSEX_START_OR_CONTINUE) {
        wntr_midi_dispatch_sysex();
    }
}

int main(void) {
    init_();

    uint32_t last_sample_time = wntr_ticks();

    while (1) {
        /*
            Gemini doesn't use an RTOS, instead, it just runs a few tasks that
            are expected to be behave and yield time to other tasks.
        */
        wntr_usb_task();
        midi_task_();
        mode_and_tweak_task_();

        /*
            The LED animation task internally ensures that it only runs once
            every few milliseconds. See GEM_ANIMATION_INTERVAL.
        */
        uint32_t animation_start_time = wntr_ticks();
        if (gem_led_animation_step(dotstar_cfg_)) {
            animation_time_ = wntr_ticks() - animation_start_time;
        }

        /*
            The oscillator & tweak tasks only need to be called when there's
            a new set of ADC readings ready. The ADC is constantly scanning
            in the background, so that gives the USB, MIDI, and LED animation
            tasks time to run between oscillator updates.
        */
        if (gem_adc_results_ready()) {
            sample_time_ = (uint16_t)(wntr_ticks() - last_sample_time);
            last_sample_time = wntr_ticks();
            oscillator_task_();
            monitor_task_();
            idle_cycles_ = 0;
        } else {
            idle_cycles_++;
        }
    }

    return 0;
}
