# Hey this file was generated by Structy from
# gem_settings.structy. Maybe don't edit it!

from dataclasses import dataclass
from typing import ClassVar

import structy


@dataclass
class GemSettings(structy.Struct):
    _PACK_STRING : ClassVar[str] = "HhHiiiiiiiiiiH??ii"

    PACKED_SIZE : ClassVar[int] = 58
    """The total size of the struct once packed."""

    adc_gain_corr: int = 2048
    """The ADC's internal gain correction register."""

    adc_offset_corr: int = 0
    """The ADC's internal offset correction register."""

    led_brightness: int = 127
    """The front-plate LED brightness."""

    castor_knob_min: structy.Fix16 = -1.2
    """Configuration for the CV knob mins and maxs in volts, defaults to -1.2 to +1.2."""

    castor_knob_max: structy.Fix16 = 1.2

    pollux_knob_min: structy.Fix16 = -1.2

    pollux_knob_max: structy.Fix16 = 1.2

    chorus_max_intensity: structy.Fix16 = 0.05
    """Maximum amount that the chorus can impact Pollux's frequency."""

    lfo_frequency: structy.Fix16 = 0.2
    """The default LFO frequency in hertz."""

    cv_offset_error: structy.Fix16 = 0.0
    """Error correction for the ADC readings for the CV input."""

    cv_gain_error: structy.Fix16 = 1.0

    smooth_initial_gain: structy.Fix16 = 0.1
    """Pitch input CV smoothing parameters."""

    smooth_sensitivity: structy.Fix16 = 30.0

    pollux_follower_threshold: int = 100
    """This is the "deadzone" for Pollux's pitch CV input, basically, it
            should be around 0v and it's the point where Pollux starts following
            Castor's pitch CV. By default this is 100 code points to allow some
            variance in time and temperature."""

    castor_lfo_pwm: bool = False
    """Route LFO to PWM for oscillators"""

    pollux_lfo_pwm: bool = False

    pitch_knob_nonlinearity: structy.Fix16 = 0.6
    """The firmware adjusts the pitch CV knobs so that it's easier to tune
            the oscillators. It does this by spreading the values near the center
            of the knob out so that the range at the center is more fine. This
            makes the knobs non-linear. This setting controls how "intense"
            this non-linearity is. A value of 0.33 is equivalent to a linear
            response. Higher values make it easier and easier to tune the
            oscillator but sacrifice the range, values lower than 0.33 will
            make it harder to tune and aren't recommended."""

    base_cv_offset: structy.Fix16 = 1.0
    """The base CV offset applied to the pitch inputs."""
