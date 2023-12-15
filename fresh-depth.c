#include <stdint.h>

#include "../eeprom.h"

#include "fresh-sensor.h"
#include "eeprom-this.h"
#include "tilt.h"

#define MM_PER_PSI 703

static  int16_t _sensorPosnFront = 0; //Front of CL is +ve; rear of CL is -ve
static  int16_t _sensorPosnRight = 0; //Right of CL is +ve; left of CL is -ve seen as facing front

int16_t FreshDepthGetSensorPosnFront() { return _sensorPosnFront; }
int16_t FreshDepthGetSensorPosnRight() { return _sensorPosnRight; }
 
void    FreshDepthSetSensorPosnFront( int16_t value) { _sensorPosnFront = value; EepromSaveS16(EEPROM_FRESH_SENSOR_FRONT_S16  , value); }
void    FreshDepthSetSensorPosnRight( int16_t value) { _sensorPosnRight = value; EepromSaveS16(EEPROM_FRESH_SENSOR_RIGHT_S16  , value); }

int16_t FreshDepthGetMmRaw()
{
    int16_t mv = FreshSensorGetMvCompensatedForTemperature();
    int32_t mm = (int32_t)(mv - FRESH_SENSOR_MV_AT_ZERO_PSI) * MM_PER_PSI * FRESH_SENSOR_MAX_PSI / (FRESH_SENSOR_MV_AT_MAX_PSI - FRESH_SENSOR_MV_AT_ZERO_PSI);
    return (int16_t)mm;
}

int16_t FreshDepthGetMmCompensatedForTilt()
{
    int32_t roll  = TiltGetRollAs14bit();
    int32_t pitch = TiltGetPitchAs14bit();
    int32_t extra = (roll * _sensorPosnRight - pitch * _sensorPosnFront) / 16384 ; //Negative because a positive pitch reduces the depth at the front
    return FreshDepthGetMmRaw() - (int16_t)extra;
}

void FreshDepthInit()
{
    _sensorPosnFront = EepromReadS16(EEPROM_FRESH_SENSOR_FRONT_S16  );
    _sensorPosnRight = EepromReadS16(EEPROM_FRESH_SENSOR_RIGHT_S16  );
    
    if (_sensorPosnFront >= 2000 || _sensorPosnFront <= -2000) _sensorPosnFront = 0;
    if (_sensorPosnRight >= 2000 || _sensorPosnRight <= -2000) _sensorPosnRight = 0;
}