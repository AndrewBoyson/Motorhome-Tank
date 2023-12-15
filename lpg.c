#include <xc.h>
#include <stdint.h>

#include "../eeprom.h"

#include "adc.h"
#include "eeprom-this.h"

#define CONSTANT_CURRENT_UA 10380

static int16_t _resistanceMin16ths = 0;
static int16_t _resistanceMax16ths = 0;
static int16_t _volumeMinMl = 0;
static int16_t _volumeMaxMl = 0;

int16_t LpgGetResistanceMin16ths() { return _resistanceMin16ths; }
int16_t LpgGetResistanceMax16ths() { return _resistanceMax16ths; }
int16_t LpgGetVolumeMinMl       () { return _volumeMinMl;        }
int16_t LpgGetVolumeMaxMl       () { return _volumeMaxMl;        }

void    LpgSetResistanceMin16ths(int16_t value) { _resistanceMin16ths = value; EepromSaveS16(EEPROM_LPG_RESISTANCE_MIN_16THS_S16, value); }
void    LpgSetResistanceMax16ths(int16_t value) { _resistanceMax16ths = value; EepromSaveS16(EEPROM_LPG_RESISTANCE_MAX_16THS_S16, value); }
void    LpgSetVolumeMinMl       (int16_t value) { _volumeMinMl        = value; EepromSaveS16(EEPROM_LPG_VOLUME_MIN_ML_S16       , value); }
void    LpgSetVolumeMaxMl       (int16_t value) { _volumeMaxMl        = value; EepromSaveS16(EEPROM_LPG_VOLUME_MAX_ML_S16       , value); }

void LpgInit(void)
{
    TRISCbits.TRISC2 = 0;	//RC2 (pin 13) output
    PORTCbits.RC2 = 0;      //Turn off power
    
    _resistanceMin16ths = EepromReadS16(EEPROM_LPG_RESISTANCE_MIN_16THS_S16);
    _resistanceMax16ths = EepromReadS16(EEPROM_LPG_RESISTANCE_MAX_16THS_S16);
    _volumeMinMl        = EepromReadS16(EEPROM_LPG_VOLUME_MIN_ML_S16       );
    _volumeMaxMl        = EepromReadS16(EEPROM_LPG_VOLUME_MAX_ML_S16       );
}

static void powerSet(char onoff)
{
    PORTCbits.RC2 = onoff;
}
static char powerGet()
{
    return PORTCbits.RC2;
}

int16_t LpgGetMvRaw()
{
    return AdcGetMv1();
}

int16_t LpgGetResistance16ths()
{
    int32_t resistance = (int32_t)16 * 1000 * LpgGetMvRaw() / CONSTANT_CURRENT_UA; //Biggest is 4 + 10 + 16 = 30bit
    if (resistance > 110 * 16) resistance = 110 * 16;
    if (resistance < -10 * 16) resistance = -10 * 16;
    return (int16_t)resistance;
}

int8_t LpgGetVolumePercent()
{
    if (_resistanceMax16ths == _resistanceMin16ths) return 100; //Ensure no subsequent division by zero
    
    int32_t percent = 100L * LpgGetResistance16ths() / (_resistanceMax16ths - _resistanceMin16ths);
    //int32_t percent = 100L * LpgGetResistance16ths() / _resistanceFull / 16;
    
    if (percent > 110) percent = 110;
    if (percent < -10) percent = -10;
    return (int8_t) percent;
}

int16_t LpgGetVolumeMl() //Actually 1024ths of a litre
{
    if (_resistanceMax16ths == _resistanceMin16ths) return 30*1024; //Ensure no subsequent division by zero
    
    int32_t ml = _volumeMinMl + (int32_t)(_volumeMaxMl - _volumeMinMl) * LpgGetResistance16ths() / (_resistanceMax16ths - _resistanceMin16ths);
    //int32_t ml = (int32_t)CAPACITY_LITRES * 1024 / 16 * LpgGetResistance16ths() / _resistanceFull; // Biggest is 5 + 6 + 16 = 27bit;
    
    if (ml >  30*1024) ml =  30*1024;
    if (ml < -10*1024) ml = -10*1024;
    return (int16_t) ml;
}

void LpgMain(void)
{
    powerSet(1);
}