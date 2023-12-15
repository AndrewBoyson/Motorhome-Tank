#include <stdint.h>

#include "../eeprom.h"
#include "../1-wire-roms.h"

#include "eeprom-this.h"

static uint64_t _outsideRom = 0;
static uint64_t _heatingRom = 0;

uint64_t AmbientGetOutsideRom() { return _outsideRom; }
uint64_t AmbientGetHeatingRom() { return _heatingRom; }
void     AmbientSetOutsideRom(uint64_t value) { _outsideRom = value; EepromSaveU64(EEPROM_AMBIENT_OUTSIDE_ROM_U64, value); }
void     AmbientSetHeatingRom(uint64_t value) { _heatingRom = value; EepromSaveU64(EEPROM_AMBIENT_HEATING_ROM_U64, value); }

int16_t  AmbientGetOutsideTemp16ths() { return OneWireRomDataFromCode(_outsideRom); }
int16_t  AmbientGetHeatingTemp16ths() { return OneWireRomDataFromCode(_heatingRom); }

void     AmbientInit()
{
    _outsideRom = EepromReadU64(EEPROM_AMBIENT_OUTSIDE_ROM_U64);
    _heatingRom = EepromReadU64(EEPROM_AMBIENT_HEATING_ROM_U64);
}