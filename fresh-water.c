
#include <stdint.h>

#include "../eeprom.h"

#include "eeprom-this.h"
#include "fresh-sensor.h"
#include "fresh-depth.h"

static uint16_t _tankWidth = 0;
static uint16_t _tankLength = 0;

uint16_t FreshWaterGetTankWidth () { return _tankWidth ; }
uint16_t FreshWaterGetTankLength() { return _tankLength; }

void     FreshWaterSetTankWidth (uint16_t value) { _tankWidth  = value; EepromSaveU16(EEPROM_FRESH_TANK_WIDTH_U16 , value); }
void     FreshWaterSetTankLength(uint16_t value) { _tankLength = value; EepromSaveU16(EEPROM_FRESH_TANK_LENGTH_U16, value); }

void FreshWaterInit()
{
    FreshSensorInit();
    FreshDepthInit();
    
    _tankWidth  = EepromReadU16(EEPROM_FRESH_TANK_WIDTH_U16 );
    _tankLength = EepromReadU16(EEPROM_FRESH_TANK_LENGTH_U16);
    
    if (_tankWidth  >= 2000) _tankWidth  = 0;
    if (_tankLength >= 2000) _tankLength = 0;
}

int16_t FreshWaterGetLitres()
{
    int16_t mm = FreshDepthGetMmCompensatedForTilt();
    int32_t mm3 = (int32_t)mm * _tankLength * _tankWidth / 100 / 100 / 100;
    return (int16_t)mm3;
}
