#include "gem_voice_param_table.h"
#include "fix16.h"

struct gem_voice_params gem_voice_param_table[] = {
    {.voltage = F16(0.0000), .period_reg = 122311, .castor_dac_code = 40, .pollux_dac_code = 40},
    {.voltage = F16(0.0833), .period_reg = 115446, .castor_dac_code = 40, .pollux_dac_code = 40},
    {.voltage = F16(0.1667), .period_reg = 108967, .castor_dac_code = 40, .pollux_dac_code = 40},
    {.voltage = F16(0.2500), .period_reg = 102851, .castor_dac_code = 40, .pollux_dac_code = 40},
    {.voltage = F16(0.3333), .period_reg = 97078, .castor_dac_code = 42, .pollux_dac_code = 42},
    {.voltage = F16(0.4167), .period_reg = 91630, .castor_dac_code = 44, .pollux_dac_code = 44},
    {.voltage = F16(0.5000), .period_reg = 86487, .castor_dac_code = 46, .pollux_dac_code = 46},
    {.voltage = F16(0.5833), .period_reg = 81633, .castor_dac_code = 48, .pollux_dac_code = 48},
    {.voltage = F16(0.6667), .period_reg = 77051, .castor_dac_code = 50, .pollux_dac_code = 50},
    {.voltage = F16(0.7500), .period_reg = 72726, .castor_dac_code = 53, .pollux_dac_code = 53},
    {.voltage = F16(0.8333), .period_reg = 68644, .castor_dac_code = 56, .pollux_dac_code = 56},
    {.voltage = F16(0.9167), .period_reg = 64792, .castor_dac_code = 60, .pollux_dac_code = 60},
    {.voltage = F16(1.0000), .period_reg = 61155, .castor_dac_code = 62, .pollux_dac_code = 62},
    {.voltage = F16(1.0833), .period_reg = 57723, .castor_dac_code = 65, .pollux_dac_code = 65},
    {.voltage = F16(1.1667), .period_reg = 54483, .castor_dac_code = 69, .pollux_dac_code = 69},
    {.voltage = F16(1.2500), .period_reg = 51425, .castor_dac_code = 73, .pollux_dac_code = 73},
    {.voltage = F16(1.3333), .period_reg = 48539, .castor_dac_code = 77, .pollux_dac_code = 77},
    {.voltage = F16(1.4167), .period_reg = 45814, .castor_dac_code = 82, .pollux_dac_code = 82},
    {.voltage = F16(1.5000), .period_reg = 43243, .castor_dac_code = 87, .pollux_dac_code = 87},
    {.voltage = F16(1.5833), .period_reg = 40816, .castor_dac_code = 91, .pollux_dac_code = 91},
    {.voltage = F16(1.6667), .period_reg = 38525, .castor_dac_code = 96, .pollux_dac_code = 96},
    {.voltage = F16(1.7500), .period_reg = 36363, .castor_dac_code = 103, .pollux_dac_code = 103},
    {.voltage = F16(1.8333), .period_reg = 34322, .castor_dac_code = 108, .pollux_dac_code = 108},
    {.voltage = F16(1.9167), .period_reg = 32395, .castor_dac_code = 115, .pollux_dac_code = 115},
    {.voltage = F16(2.0000), .period_reg = 30577, .castor_dac_code = 122, .pollux_dac_code = 122},
    {.voltage = F16(2.0833), .period_reg = 28861, .castor_dac_code = 129, .pollux_dac_code = 129},
    {.voltage = F16(2.1667), .period_reg = 27241, .castor_dac_code = 137, .pollux_dac_code = 137},
    {.voltage = F16(2.2500), .period_reg = 25712, .castor_dac_code = 146, .pollux_dac_code = 146},
    {.voltage = F16(2.3333), .period_reg = 24269, .castor_dac_code = 154, .pollux_dac_code = 154},
    {.voltage = F16(2.4167), .period_reg = 22907, .castor_dac_code = 164, .pollux_dac_code = 164},
    {.voltage = F16(2.5000), .period_reg = 21621, .castor_dac_code = 173, .pollux_dac_code = 173},
    {.voltage = F16(2.5833), .period_reg = 20407, .castor_dac_code = 183, .pollux_dac_code = 183},
    {.voltage = F16(2.6667), .period_reg = 19262, .castor_dac_code = 193, .pollux_dac_code = 193},
    {.voltage = F16(2.7500), .period_reg = 18181, .castor_dac_code = 203, .pollux_dac_code = 203},
    {.voltage = F16(2.8333), .period_reg = 17160, .castor_dac_code = 215, .pollux_dac_code = 215},
    {.voltage = F16(2.9167), .period_reg = 16197, .castor_dac_code = 228, .pollux_dac_code = 228},
    {.voltage = F16(3.0000), .period_reg = 15288, .castor_dac_code = 241, .pollux_dac_code = 241},
    {.voltage = F16(3.0833), .period_reg = 14430, .castor_dac_code = 255, .pollux_dac_code = 255},
    {.voltage = F16(3.1667), .period_reg = 13620, .castor_dac_code = 271, .pollux_dac_code = 271},
    {.voltage = F16(3.2500), .period_reg = 12855, .castor_dac_code = 287, .pollux_dac_code = 287},
    {.voltage = F16(3.3333), .period_reg = 12134, .castor_dac_code = 307, .pollux_dac_code = 307},
    {.voltage = F16(3.4167), .period_reg = 11453, .castor_dac_code = 322, .pollux_dac_code = 322},
    {.voltage = F16(3.5000), .period_reg = 10810, .castor_dac_code = 342, .pollux_dac_code = 342},
    {.voltage = F16(3.5833), .period_reg = 10203, .castor_dac_code = 362, .pollux_dac_code = 362},
    {.voltage = F16(3.6667), .period_reg = 9630, .castor_dac_code = 382, .pollux_dac_code = 382},
    {.voltage = F16(3.7500), .period_reg = 9090, .castor_dac_code = 404, .pollux_dac_code = 404},
    {.voltage = F16(3.8333), .period_reg = 8580, .castor_dac_code = 428, .pollux_dac_code = 428},
    {.voltage = F16(3.9167), .period_reg = 8098, .castor_dac_code = 453, .pollux_dac_code = 453},
    {.voltage = F16(4.0000), .period_reg = 7644, .castor_dac_code = 479, .pollux_dac_code = 479},
    {.voltage = F16(4.0833), .period_reg = 7214, .castor_dac_code = 512, .pollux_dac_code = 512},
    {.voltage = F16(4.1667), .period_reg = 6809, .castor_dac_code = 543, .pollux_dac_code = 543},
    {.voltage = F16(4.2500), .period_reg = 6427, .castor_dac_code = 576, .pollux_dac_code = 576},
    {.voltage = F16(4.3333), .period_reg = 6066, .castor_dac_code = 616, .pollux_dac_code = 616},
    {.voltage = F16(4.4167), .period_reg = 5726, .castor_dac_code = 648, .pollux_dac_code = 648},
    {.voltage = F16(4.5000), .period_reg = 5404, .castor_dac_code = 688, .pollux_dac_code = 688},
    {.voltage = F16(4.5833), .period_reg = 5101, .castor_dac_code = 728, .pollux_dac_code = 728},
    {.voltage = F16(4.6667), .period_reg = 4815, .castor_dac_code = 771, .pollux_dac_code = 771},
    {.voltage = F16(4.7500), .period_reg = 4544, .castor_dac_code = 821, .pollux_dac_code = 821},
    {.voltage = F16(4.8333), .period_reg = 4289, .castor_dac_code = 865, .pollux_dac_code = 865},
    {.voltage = F16(4.9167), .period_reg = 4049, .castor_dac_code = 925, .pollux_dac_code = 925},
    {.voltage = F16(5.0000), .period_reg = 3821, .castor_dac_code = 975, .pollux_dac_code = 975},
    {.voltage = F16(5.0833), .period_reg = 3607, .castor_dac_code = 1035, .pollux_dac_code = 1035},
    {.voltage = F16(5.1667), .period_reg = 3404, .castor_dac_code = 1095, .pollux_dac_code = 1095},
    {.voltage = F16(5.2500), .period_reg = 3213, .castor_dac_code = 1155, .pollux_dac_code = 1155},
    {.voltage = F16(5.3333), .period_reg = 3033, .castor_dac_code = 1225, .pollux_dac_code = 1225},
    {.voltage = F16(5.4167), .period_reg = 2862, .castor_dac_code = 1305, .pollux_dac_code = 1305},
    {.voltage = F16(5.5000), .period_reg = 2702, .castor_dac_code = 1385, .pollux_dac_code = 1385},
    {.voltage = F16(5.5833), .period_reg = 2550, .castor_dac_code = 1475, .pollux_dac_code = 1475},
    {.voltage = F16(5.6667), .period_reg = 2407, .castor_dac_code = 1556, .pollux_dac_code = 1556},
    {.voltage = F16(5.7500), .period_reg = 2272, .castor_dac_code = 1646, .pollux_dac_code = 1646},
    {.voltage = F16(5.8333), .period_reg = 2144, .castor_dac_code = 1746, .pollux_dac_code = 1746},
    {.voltage = F16(5.9167), .period_reg = 2024, .castor_dac_code = 1856, .pollux_dac_code = 1856},
    {.voltage = F16(6.0000), .period_reg = 1910, .castor_dac_code = 1966, .pollux_dac_code = 1966},
    {.voltage = F16(6.0833), .period_reg = 1803, .castor_dac_code = 2086, .pollux_dac_code = 2086},
    {.voltage = F16(6.1667), .period_reg = 1702, .castor_dac_code = 2216, .pollux_dac_code = 2216},
    {.voltage = F16(6.2500), .period_reg = 1606, .castor_dac_code = 2350, .pollux_dac_code = 2350},
    {.voltage = F16(6.3333), .period_reg = 1516, .castor_dac_code = 2495, .pollux_dac_code = 2495},
    {.voltage = F16(6.4167), .period_reg = 1431, .castor_dac_code = 2638, .pollux_dac_code = 2638},
    {.voltage = F16(6.5000), .period_reg = 1350, .castor_dac_code = 2801, .pollux_dac_code = 2801},
    {.voltage = F16(6.5833), .period_reg = 1275, .castor_dac_code = 2991, .pollux_dac_code = 2991},
    {.voltage = F16(6.6667), .period_reg = 1203, .castor_dac_code = 3159, .pollux_dac_code = 3159},
    {.voltage = F16(6.7500), .period_reg = 1135, .castor_dac_code = 3359, .pollux_dac_code = 3359},
    {.voltage = F16(6.8333), .period_reg = 1072, .castor_dac_code = 3559, .pollux_dac_code = 3559},
    {.voltage = F16(6.9167), .period_reg = 1011, .castor_dac_code = 3780, .pollux_dac_code = 3780},
    {.voltage = F16(7.0000), .period_reg = 955, .castor_dac_code = 4010, .pollux_dac_code = 4010},
};

size_t gem_voice_param_table_len = sizeof(gem_voice_param_table) / sizeof(struct gem_voice_params);