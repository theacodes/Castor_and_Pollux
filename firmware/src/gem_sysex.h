/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "fix16.h"
#include "gem_adc.h"
#include "gem_i2c.h"
#include "gem_monitor_update.h"
#include "gem_pulseout.h"
#include <stdbool.h>
#include <stdint.h>

/* Implementations of SysEx commands for calibration, setup, and configuration. */

void gem_sysex_init(
    const struct GemADCInput* adc_inputs, const struct GemI2CConfig* i2c, const struct GemPulseOutConfig* pulse);

void gem_sysex_send_monitor_update(struct GemMonitorUpdate* update);