#include <stdint.h>

#include "../eeprom.h"
#include "../1-wire-device.h"

#include "adc.h"
#include "eeprom-this.h"
#include "fresh-sensor.h"
#include "voltage.h"

static  int16_t _sensorUvPer16th = 0;
static  int16_t _sensorBaseTemp16ths = 0;
static  int16_t _sensorBaseMv = 0;
static uint64_t _rom = 0;

 int16_t FreshSensorGetUvPer16th    () { return _sensorUvPer16th    ; }
 int16_t FreshSensorGetBaseTemp16ths() { return _sensorBaseTemp16ths; }
 int16_t FreshSensorGetBaseMv       () { return _sensorBaseMv       ; }
uint64_t FreshSensorGetRom          () { return _rom                ; }

void     FreshSensorSetUvPer16th    ( int16_t value) { _sensorUvPer16th     = value; EepromSaveS16(EEPROM_FRESH_SENSOR_UV_PER_16TH_S16    , value); }
void     FreshSensorSetBaseTemp16ths( int16_t value) { _sensorBaseTemp16ths = value; EepromSaveS16(EEPROM_FRESH_SENSOR_BASE_TEMP_16THS_S16, value); }
void     FreshSensorSetBaseMv       ( int16_t value) { _sensorBaseMv        = value; EepromSaveS16(EEPROM_FRESH_SENSOR_BASE_MV_S16        , value); }
void     FreshSensorSetRom          (uint64_t value) { _rom                 = value; EepromSaveU64(EEPROM_FRESH_ROM_U64                   , value); }

int16_t FreshSensorGetTemperature16ths()
{
    return OneWireDeviceGetDataById(_rom);
}
int16_t FreshSensorGetSupplyMv()
{
    return VoltageGetMv();
}

int16_t FreshSensorGetMvRaw()
{
    return AdcGetMv0();
}

int16_t FreshSensorGetMvCompensatedForTemperature()
{
    int16_t sensorTemp16ths = FreshSensorGetTemperature16ths();
    if (sensorTemp16ths > 120*16 || sensorTemp16ths < -20*16) return FreshSensorGetMvRaw();
    int16_t extra = (_sensorUvPer16th * (sensorTemp16ths - _sensorBaseTemp16ths)) / 1024;
    extra += _sensorBaseMv - FRESH_SENSOR_MV_AT_ZERO_PSI;
    return FreshSensorGetMvRaw() - extra;
}
void FreshSensorInit()
{
    _rom                 = EepromReadU64(EEPROM_FRESH_ROM_U64                   );
    _sensorUvPer16th     = EepromReadS16(EEPROM_FRESH_SENSOR_UV_PER_16TH_S16    );
    _sensorBaseTemp16ths = EepromReadS16(EEPROM_FRESH_SENSOR_BASE_TEMP_16THS_S16);
    _sensorBaseMv        = EepromReadS16(EEPROM_FRESH_SENSOR_BASE_MV_S16        );
    
    if (_sensorUvPer16th     >=  10000 || _sensorUvPer16th     <=  -10000) _sensorUvPer16th     = 0;
    if (_sensorBaseTemp16ths >=  30*16 || _sensorBaseTemp16ths <=  -30*16) _sensorBaseTemp16ths = 0;
    if (_sensorBaseMv        >=   1000 || _sensorBaseMv        <=       0) _sensorBaseMv        = 500;
}
