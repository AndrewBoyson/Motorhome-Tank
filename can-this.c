#include "../mstimer.h"
#include "../msticker.h"
#include "../can.h"
#include "../canids.h"
#include "../1-wire-roms.h"

#include "fresh-water.h"
#include "fresh-sensor.h"
#include "fresh-depth.h"
#include "accelerometer.h"
#include "lpg.h"
#include "ambient.h"

#define BASE_MS 1000

static void receive(uint16_t id, uint8_t length, void* pData)
{
    switch(id)
    {
        case CAN_ID_SERVER + CAN_ID_TIME:               MsTickerRegulate            (*(uint32_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_FRESH_BASE_TEMP:      FreshSensorSetBaseTemp16ths (*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_FRESH_BASE_MV:        FreshSensorSetBaseMv        (*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_FRESH_UV_PER_16TH:    FreshSensorSetUvPer16th     (*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_FRESH_SENSOR_FRONT:   FreshDepthSetSensorPosnFront(*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_FRESH_SENSOR_RIGHT:   FreshDepthSetSensorPosnRight(*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_FRESH_TANK_WIDTH:     FreshWaterSetTankWidth      (*(uint16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_FRESH_TANK_LENGTH:    FreshWaterSetTankLength     (*(uint16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_ACCELEROMETER_X_FLAT: AccelerometerSetXFlat       (*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_ACCELEROMETER_Y_FLAT: AccelerometerSetYFlat       (*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_ACCELEROMETER_Z_FLAT: AccelerometerSetZFlat       (*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_FRESH_ROM:            FreshSensorSetRom           (*(uint64_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_LPG_RESISTANCE_MIN:   LpgSetResistanceMin16ths    (*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_LPG_RESISTANCE_MAX:   LpgSetResistanceMax16ths    (*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_LPG_VOLUME_MIN:       LpgSetVolumeMinMl           (*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_LPG_VOLUME_MAX:       LpgSetVolumeMaxMl           (*( int16_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_AMBIENT_OUTSIDE_ROM:  AmbientSetOutsideRom        (*(uint64_t*)pData); break;
        case CAN_ID_TANK + CAN_ID_AMBIENT_HEATING_ROM:  AmbientSetHeatingRom        (*(uint64_t*)pData); break;
    }
}

void CanThisInit(void)
{
    CanReceive = &receive;
}
void CanThisMain(void)
{
    static uint32_t msTimerFreshTemperature   = 0;
    static uint32_t msTimerFreshRom           = 0;
    static uint32_t msTimerFreshSupplyMv      = 0;
    static uint32_t msTimerFreshBaseTemp      = 0;
    static uint32_t msTimerFreshBaseMv        = 0;
    static uint32_t msTimerFreshUvPer16th     = 0;
    static uint32_t msTimerFreshSensorMv      = 0;
    static uint32_t msTimerFreshDepthMm       = 0;
    static uint32_t msTimerFreshLitres        = 0;
    static uint32_t msTimerFreshPosnFront     = 0;
    static uint32_t msTimerFreshPosnRight     = 0;
    static uint32_t msTimerFreshTankWidth     = 0;
    static uint32_t msTimerFreshTankLength    = 0;
    static uint32_t msTimerAccelerometerXFlat = 0;
    static uint32_t msTimerAccelerometerYFlat = 0;
    static uint32_t msTimerAccelerometerZFlat = 0;
    static uint32_t msTimerAccelerometerX     = 0;
    static uint32_t msTimerAccelerometerY     = 0;
    static uint32_t msTimerAccelerometerZ     = 0;
    static uint32_t msTimerRom0               = 0;
    static uint32_t msTimerRom1               = 0;
    static uint32_t msTimerRom2               = 0;
    static uint32_t msTimerRom3               = 0;
    static uint32_t msTimerRomData0           = 0;
    static uint32_t msTimerRomData1           = 0;
    static uint32_t msTimerRomData2           = 0;
    static uint32_t msTimerRomData3           = 0;
    static uint32_t msTimerLpgMv              = 0;
    static uint32_t msTimerLpgResistance      = 0;
    static uint32_t msTimerLpgResistanceMin   = 0;
    static uint32_t msTimerLpgResistanceMax   = 0;
    static uint32_t msTimerLpgVolumeMin       = 0;
    static uint32_t msTimerLpgVolumeMax       = 0;
    static uint32_t msTimerLpgVolume          = 0;
    static uint32_t msTimerOutsideRom         = 0;
    static uint32_t msTimerHeatingRom         = 0;
    static uint32_t msTimerOutsideTemp        = 0;
    static uint32_t msTimerHeatingTemp        = 0;
    
    static char sendFreshTemperature   = 0;
    static char sendFreshRom           = 0;
    static char sendFreshSupplyMv      = 0;
    static char sendFreshBaseTemp      = 0;
    static char sendFreshBaseMv        = 0;
    static char sendFreshUvPer16th     = 0;
    static char sendFreshSensorMv      = 0;
    static char sendFreshDepthMm       = 0;
    static char sendFreshLitres        = 0;
    static char sendFreshPosnFront     = 0;
    static char sendFreshPosnRight     = 0;
    static char sendFreshTankWidth     = 0;
    static char sendFreshTankLength    = 0;
    static char sendAccelerometerXFlat = 0;
    static char sendAccelerometerYFlat = 0;
    static char sendAccelerometerZFlat = 0;
    static char sendAccelerometerX     = 0;
    static char sendAccelerometerY     = 0;
    static char sendAccelerometerZ     = 0;
    static char sendRom0               = 0;
    static char sendRom1               = 0;
    static char sendRom2               = 0;
    static char sendRom3               = 0;
    static char sendRomData0           = 0;
    static char sendRomData1           = 0;
    static char sendRomData2           = 0;
    static char sendRomData3           = 0;
    static char sendLpgMv              = 0;
    static char sendLpgResistance      = 0;
    static char sendLpgResistanceMin   = 0;
    static char sendLpgResistanceMax   = 0;
    static char sendLpgVolumeMin       = 0;
    static char sendLpgVolumeMax       = 0;
    static char sendLpgVolume          = 0;
    static char sendOutsideRom         = 0;
    static char sendHeatingRom         = 0;
    static char sendOutsideTemp        = 0;
    static char sendHeatingTemp        = 0;
    
    
    if (MsTimerRepetitive(&msTimerFreshTemperature  , BASE_MS + CAN_ID_FRESH_TEMPERATURE   )) sendFreshTemperature   = 1;
    if (MsTimerRepetitive(&msTimerFreshRom          , BASE_MS + CAN_ID_FRESH_ROM           )) sendFreshRom           = 1;
    if (MsTimerRepetitive(&msTimerFreshSupplyMv     , BASE_MS + CAN_ID_FRESH_SUPPLY_MV     )) sendFreshSupplyMv      = 1;
    if (MsTimerRepetitive(&msTimerFreshBaseTemp     , BASE_MS + CAN_ID_FRESH_BASE_TEMP     )) sendFreshBaseTemp      = 1;
    if (MsTimerRepetitive(&msTimerFreshBaseMv       , BASE_MS + CAN_ID_FRESH_BASE_MV       )) sendFreshBaseMv        = 1;
    if (MsTimerRepetitive(&msTimerFreshUvPer16th    , BASE_MS + CAN_ID_FRESH_UV_PER_16TH   )) sendFreshUvPer16th     = 1;
    if (MsTimerRepetitive(&msTimerFreshSensorMv     , BASE_MS + CAN_ID_FRESH_MV            )) sendFreshSensorMv      = 1;
    if (MsTimerRepetitive(&msTimerFreshDepthMm      , BASE_MS + CAN_ID_FRESH_DEPTH_MM      )) sendFreshDepthMm       = 1;
    if (MsTimerRepetitive(&msTimerFreshLitres       , BASE_MS + CAN_ID_FRESH_LITRES        )) sendFreshLitres        = 1;
    
    if (MsTimerRepetitive(&msTimerFreshPosnFront    , BASE_MS + CAN_ID_FRESH_SENSOR_FRONT  )) sendFreshPosnFront     = 1;
    if (MsTimerRepetitive(&msTimerFreshPosnRight    , BASE_MS + CAN_ID_FRESH_SENSOR_RIGHT  )) sendFreshPosnRight     = 1;
    if (MsTimerRepetitive(&msTimerFreshTankWidth    , BASE_MS + CAN_ID_FRESH_TANK_WIDTH    )) sendFreshTankWidth     = 1;
    if (MsTimerRepetitive(&msTimerFreshTankLength   , BASE_MS + CAN_ID_FRESH_TANK_LENGTH   )) sendFreshTankLength    = 1;
    
    if (MsTimerRepetitive(&msTimerAccelerometerXFlat, BASE_MS + CAN_ID_ACCELEROMETER_X_FLAT)) sendAccelerometerXFlat = 1;
    if (MsTimerRepetitive(&msTimerAccelerometerYFlat, BASE_MS + CAN_ID_ACCELEROMETER_Y_FLAT)) sendAccelerometerYFlat = 1;
    if (MsTimerRepetitive(&msTimerAccelerometerZFlat, BASE_MS + CAN_ID_ACCELEROMETER_Z_FLAT)) sendAccelerometerZFlat = 1;
    if (MsTimerRepetitive(&msTimerAccelerometerX    , BASE_MS + CAN_ID_ACCELEROMETER_X     )) sendAccelerometerX     = 1;
    if (MsTimerRepetitive(&msTimerAccelerometerY    , BASE_MS + CAN_ID_ACCELEROMETER_Y     )) sendAccelerometerY     = 1;
    if (MsTimerRepetitive(&msTimerAccelerometerZ    , BASE_MS + CAN_ID_ACCELEROMETER_Z     )) sendAccelerometerZ     = 1;
    
    if (MsTimerRepetitive(&msTimerRom0              , BASE_MS + CAN_ID_1WIRE_ROM_0         )) sendRom0               = 1;
    if (MsTimerRepetitive(&msTimerRom1              , BASE_MS + CAN_ID_1WIRE_ROM_1         )) sendRom1               = 1;
    if (MsTimerRepetitive(&msTimerRom2              , BASE_MS + CAN_ID_1WIRE_ROM_2         )) sendRom2               = 1;
    if (MsTimerRepetitive(&msTimerRom3              , BASE_MS + CAN_ID_1WIRE_ROM_3         )) sendRom3               = 1;
    if (MsTimerRepetitive(&msTimerRomData0          , BASE_MS + CAN_ID_1WIRE_ROM_DATA_0    )) sendRomData0           = 1;
    if (MsTimerRepetitive(&msTimerRomData1          , BASE_MS + CAN_ID_1WIRE_ROM_DATA_1    )) sendRomData1           = 1;
    if (MsTimerRepetitive(&msTimerRomData2          , BASE_MS + CAN_ID_1WIRE_ROM_DATA_2    )) sendRomData2           = 1;
    if (MsTimerRepetitive(&msTimerRomData3          , BASE_MS + CAN_ID_1WIRE_ROM_DATA_3    )) sendRomData3           = 1;
    
    if (MsTimerRepetitive(&msTimerLpgMv             , BASE_MS + CAN_ID_LPG_MV              )) sendLpgMv              = 1;
    if (MsTimerRepetitive(&msTimerLpgResistance     , BASE_MS + CAN_ID_LPG_RESISTANCE      )) sendLpgResistance      = 1;
    if (MsTimerRepetitive(&msTimerLpgResistanceMin  , BASE_MS + CAN_ID_LPG_RESISTANCE_MIN  )) sendLpgResistanceMin   = 1;
    if (MsTimerRepetitive(&msTimerLpgResistanceMax  , BASE_MS + CAN_ID_LPG_RESISTANCE_MAX  )) sendLpgResistanceMax   = 1;
    if (MsTimerRepetitive(&msTimerLpgVolumeMin      , BASE_MS + CAN_ID_LPG_VOLUME_MIN      )) sendLpgVolumeMin       = 1;
    if (MsTimerRepetitive(&msTimerLpgVolumeMax      , BASE_MS + CAN_ID_LPG_VOLUME_MAX      )) sendLpgVolumeMax       = 1;
    if (MsTimerRepetitive(&msTimerLpgVolume         , BASE_MS + CAN_ID_LPG_VOLUME          )) sendLpgVolume          = 1;
    
    if (MsTimerRepetitive(&msTimerOutsideRom        , BASE_MS + CAN_ID_AMBIENT_OUTSIDE_ROM )) sendOutsideRom         = 1;
    if (MsTimerRepetitive(&msTimerHeatingRom        , BASE_MS + CAN_ID_AMBIENT_HEATING_ROM )) sendHeatingRom         = 1;
    if (MsTimerRepetitive(&msTimerOutsideTemp       , BASE_MS + CAN_ID_AMBIENT_OUTSIDE_TEMP)) sendOutsideTemp        = 1;
    if (MsTimerRepetitive(&msTimerHeatingTemp       , BASE_MS + CAN_ID_AMBIENT_HEATING_TEMP)) sendHeatingTemp        = 1;
    
    
    if (sendFreshTemperature  ) { int16_t value = FreshSensorGetTemperature16ths(); sendFreshTemperature   = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_TEMPERATURE   , sizeof(value), &value); }
    if (sendFreshRom          ) {uint64_t value = FreshSensorGetRom             (); sendFreshRom           = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_ROM           , sizeof(value), &value); }
    if (sendFreshSupplyMv     ) { int16_t value = FreshSensorGetSupplyMv        (); sendFreshSupplyMv      = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_SUPPLY_MV     , sizeof(value), &value); }
    if (sendFreshBaseTemp     ) { int16_t value = FreshSensorGetBaseTemp16ths   (); sendFreshBaseTemp      = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_BASE_TEMP     , sizeof(value), &value); }
    if (sendFreshBaseMv       ) { int16_t value = FreshSensorGetBaseMv          (); sendFreshBaseMv        = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_BASE_MV       , sizeof(value), &value); }
    if (sendFreshUvPer16th    ) { int16_t value = FreshSensorGetUvPer16th       (); sendFreshUvPer16th     = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_UV_PER_16TH   , sizeof(value), &value); }
    if (sendFreshSensorMv     ) { int16_t value = FreshSensorGetMvRaw           (); sendFreshSensorMv      = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_MV            , sizeof(value), &value); }
    if (sendFreshDepthMm      ) { int16_t value = FreshDepthGetMmRaw            (); sendFreshDepthMm       = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_DEPTH_MM      , sizeof(value), &value); }
    if (sendFreshLitres       ) { int16_t value = FreshWaterGetLitres           (); sendFreshLitres        = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_LITRES        , sizeof(value), &value); }
    
    if (sendFreshPosnFront    ) { int16_t value = FreshDepthGetSensorPosnFront  (); sendFreshPosnFront     = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_SENSOR_FRONT  , sizeof(value), &value); }
    if (sendFreshPosnRight    ) { int16_t value = FreshDepthGetSensorPosnRight  (); sendFreshPosnRight     = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_SENSOR_RIGHT  , sizeof(value), &value); }
    if (sendFreshTankWidth    ) {uint16_t value = FreshWaterGetTankWidth        (); sendFreshTankWidth     = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_TANK_WIDTH    , sizeof(value), &value); }
    if (sendFreshTankLength   ) {uint16_t value = FreshWaterGetTankLength       (); sendFreshTankLength    = CanTransmit(CAN_ID_TANK + CAN_ID_FRESH_TANK_LENGTH   , sizeof(value), &value); }
    
    if (sendAccelerometerXFlat) { int16_t value = AccelerometerGetXFlat         (); sendAccelerometerXFlat = CanTransmit(CAN_ID_TANK + CAN_ID_ACCELEROMETER_X_FLAT, sizeof(value), &value); }
    if (sendAccelerometerYFlat) { int16_t value = AccelerometerGetYFlat         (); sendAccelerometerYFlat = CanTransmit(CAN_ID_TANK + CAN_ID_ACCELEROMETER_Y_FLAT, sizeof(value), &value); }
    if (sendAccelerometerZFlat) { int16_t value = AccelerometerGetZFlat         (); sendAccelerometerZFlat = CanTransmit(CAN_ID_TANK + CAN_ID_ACCELEROMETER_Z_FLAT, sizeof(value), &value); }
    if (sendAccelerometerX    ) { int16_t value = AccelerometerGetX             (); sendAccelerometerX     = CanTransmit(CAN_ID_TANK + CAN_ID_ACCELEROMETER_X     , sizeof(value), &value); }
    if (sendAccelerometerY    ) { int16_t value = AccelerometerGetY             (); sendAccelerometerY     = CanTransmit(CAN_ID_TANK + CAN_ID_ACCELEROMETER_Y     , sizeof(value), &value); }
    if (sendAccelerometerZ    ) { int16_t value = AccelerometerGetZ             (); sendAccelerometerZ     = CanTransmit(CAN_ID_TANK + CAN_ID_ACCELEROMETER_Z     , sizeof(value), &value); }
    
    if (sendRom0              ) {uint64_t value = OneWireRom[0]                   ; sendRom0               = CanTransmit(CAN_ID_TANK + CAN_ID_1WIRE_ROM_0         , sizeof(value), &value); }
    if (sendRom1              ) {uint64_t value = OneWireRom[1]                   ; sendRom1               = CanTransmit(CAN_ID_TANK + CAN_ID_1WIRE_ROM_1         , sizeof(value), &value); }
    if (sendRom2              ) {uint64_t value = OneWireRom[2]                   ; sendRom2               = CanTransmit(CAN_ID_TANK + CAN_ID_1WIRE_ROM_2         , sizeof(value), &value); }
    if (sendRom3              ) {uint64_t value = OneWireRom[3]                   ; sendRom3               = CanTransmit(CAN_ID_TANK + CAN_ID_1WIRE_ROM_3         , sizeof(value), &value); }
    if (sendRomData0          ) { int16_t value = OneWireRomData[0]               ; sendRomData0           = CanTransmit(CAN_ID_TANK + CAN_ID_1WIRE_ROM_DATA_0    , sizeof(value), &value); }
    if (sendRomData1          ) { int16_t value = OneWireRomData[1]               ; sendRomData1           = CanTransmit(CAN_ID_TANK + CAN_ID_1WIRE_ROM_DATA_1    , sizeof(value), &value); }
    if (sendRomData2          ) { int16_t value = OneWireRomData[2]               ; sendRomData2           = CanTransmit(CAN_ID_TANK + CAN_ID_1WIRE_ROM_DATA_2    , sizeof(value), &value); }
    if (sendRomData3          ) { int16_t value = OneWireRomData[3]               ; sendRomData3           = CanTransmit(CAN_ID_TANK + CAN_ID_1WIRE_ROM_DATA_3    , sizeof(value), &value); }
    
    if (sendLpgMv             ) { int16_t value = LpgGetMvRaw                   (); sendLpgMv              = CanTransmit(CAN_ID_TANK + CAN_ID_LPG_MV              , sizeof(value), &value); }
    if (sendLpgResistance     ) { int16_t value = LpgGetResistance16ths         (); sendLpgResistance      = CanTransmit(CAN_ID_TANK + CAN_ID_LPG_RESISTANCE      , sizeof(value), &value); }
    if (sendLpgResistanceMin  ) { int16_t value = LpgGetResistanceMin16ths      (); sendLpgResistanceMin   = CanTransmit(CAN_ID_TANK + CAN_ID_LPG_RESISTANCE_MIN  , sizeof(value), &value); }
    if (sendLpgResistanceMax  ) { int16_t value = LpgGetResistanceMax16ths      (); sendLpgResistanceMax   = CanTransmit(CAN_ID_TANK + CAN_ID_LPG_RESISTANCE_MAX  , sizeof(value), &value); }
    if (sendLpgVolumeMin      ) { int16_t value = LpgGetVolumeMinMl             (); sendLpgVolumeMin       = CanTransmit(CAN_ID_TANK + CAN_ID_LPG_VOLUME_MIN      , sizeof(value), &value); }
    if (sendLpgVolumeMax      ) { int16_t value = LpgGetVolumeMaxMl             (); sendLpgVolumeMax       = CanTransmit(CAN_ID_TANK + CAN_ID_LPG_VOLUME_MAX      , sizeof(value), &value); }
    if (sendLpgVolume         ) { int16_t value = LpgGetVolumeMl                (); sendLpgVolume          = CanTransmit(CAN_ID_TANK + CAN_ID_LPG_VOLUME          , sizeof(value), &value); }
    
    if (sendOutsideRom        ) {uint64_t value = AmbientGetOutsideRom          (); sendOutsideRom         = CanTransmit(CAN_ID_TANK + CAN_ID_AMBIENT_OUTSIDE_ROM , sizeof(value), &value); }
    if (sendHeatingRom        ) {uint64_t value = AmbientGetHeatingRom          (); sendHeatingRom         = CanTransmit(CAN_ID_TANK + CAN_ID_AMBIENT_HEATING_ROM , sizeof(value), &value); }
    if (sendOutsideTemp       ) { int16_t value = AmbientGetOutsideTemp16ths    (); sendOutsideTemp        = CanTransmit(CAN_ID_TANK + CAN_ID_AMBIENT_OUTSIDE_TEMP, sizeof(value), &value); }
    if (sendHeatingTemp       ) { int16_t value = AmbientGetHeatingTemp16ths    (); sendHeatingTemp        = CanTransmit(CAN_ID_TANK + CAN_ID_AMBIENT_HEATING_TEMP, sizeof(value), &value); }
}
