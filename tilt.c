#include <stdint.h>

#include "accelerometer.h"

int16_t TiltGetPitchAs14bit()
{
    return AccelerometerGetZ();
}
int16_t TiltGetRollAs14bit()
{
    return AccelerometerGetX();
}
int16_t TiltGetPitchAsTenths()
{
    return (int16_t)((int32_t)TiltGetPitchAs14bit() * 573 / 16384);
}
int16_t TiltGetRollAsTenths()
{
    return (int16_t)((int32_t)TiltGetRollAs14bit() * 573 / 16384);
}
