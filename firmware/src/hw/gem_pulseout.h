/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Routines for using the SAM D21 TCC peripheral to generate pulse/
    square wave/pwm signals.
*/

#include "wntr_gpio.h"
#include "wntr_ramfunc.h"
#include <stdbool.h>
#include <stdint.h>

struct GemPulseOutConfig {
    uint32_t gclk;
    uint32_t gclk_freq;
    struct WntrGPIOPin tcc0_pin;
    uint32_t tcc0_wo;
    struct WntrGPIOPin tcc1_pin;
    uint32_t tcc1_wo;
};

typedef void (*gem_pulseout_ovf_callback)(uint8_t inst);

void gem_pulseout_init(const struct GemPulseOutConfig* po, gem_pulseout_ovf_callback ovf_callback);
void gem_pulseout_set_period(const struct GemPulseOutConfig* po, uint8_t channel, uint32_t period) RAMFUNC;

inline static uint32_t gem_pulseout_frequency_to_period(const struct GemPulseOutConfig* po, uint32_t freq_millihertz) {
    return (((po->gclk_freq * 100) / freq_millihertz) - 1);
}

inline static void
gem_pulseout_set_frequency(const struct GemPulseOutConfig* po, uint8_t channel, uint32_t freq_millihertz) {
    gem_pulseout_set_period(po, channel, gem_pulseout_frequency_to_period(po, freq_millihertz));
}
