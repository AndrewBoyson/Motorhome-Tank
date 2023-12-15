#include <stdint.h>

#include "adc.h"

#define R_L 1800
#define R_U 3300

int16_t VoltageGetMv()
{
    int32_t mv = (int32_t)AdcGetMv2() * (R_L + R_U) / R_L;
    return (int16_t)mv;
}