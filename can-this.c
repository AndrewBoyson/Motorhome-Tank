
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
    {  int16_t value = FreshSensorGetTemperature16ths(); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_TEMPERATURE   , sizeof(value), &value); }
    { uint64_t value = FreshSensorGetRom             (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_ROM           , sizeof(value), &value); }
    {  int16_t value = FreshSensorGetSupplyMv        (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_SUPPLY_MV     , sizeof(value), &value); }
    {  int16_t value = FreshSensorGetBaseTemp16ths   (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_BASE_TEMP     , sizeof(value), &value); }
    {  int16_t value = FreshSensorGetBaseMv          (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_BASE_MV       , sizeof(value), &value); }
    {  int16_t value = FreshSensorGetUvPer16th       (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_UV_PER_16TH   , sizeof(value), &value); }
    {  int16_t value = FreshSensorGetMvRaw           (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_MV            , sizeof(value), &value); }
    {  int16_t value = FreshDepthGetMmRaw            (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_DEPTH_MM      , sizeof(value), &value); }
    {  int16_t value = FreshWaterGetLitres           (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_LITRES        , sizeof(value), &value); }
    
    {  int16_t value = FreshDepthGetSensorPosnFront  (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_SENSOR_FRONT  , sizeof(value), &value); }
    {  int16_t value = FreshDepthGetSensorPosnRight  (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_SENSOR_RIGHT  , sizeof(value), &value); }
    { uint16_t value = FreshWaterGetTankWidth        (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_TANK_WIDTH    , sizeof(value), &value); }
    { uint16_t value = FreshWaterGetTankLength       (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_FRESH_TANK_LENGTH   , sizeof(value), &value); }
    
    {  int16_t value = AccelerometerGetXFlat         (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_ACCELEROMETER_X_FLAT, sizeof(value), &value); }
    {  int16_t value = AccelerometerGetYFlat         (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_ACCELEROMETER_Y_FLAT, sizeof(value), &value); }
    {  int16_t value = AccelerometerGetZFlat         (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_ACCELEROMETER_Z_FLAT, sizeof(value), &value); }
    {  int16_t value = AccelerometerGetX             (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_ACCELEROMETER_X     , sizeof(value), &value); }
    {  int16_t value = AccelerometerGetY             (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_ACCELEROMETER_Y     , sizeof(value), &value); }
    {  int16_t value = AccelerometerGetZ             (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_ACCELEROMETER_Z     , sizeof(value), &value); }
    
    { uint64_t value = OneWireRom[0]                   ; static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_1WIRE_ROM_0         , sizeof(value), &value); }
    { uint64_t value = OneWireRom[1]                   ; static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_1WIRE_ROM_1         , sizeof(value), &value); }
    { uint64_t value = OneWireRom[2]                   ; static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_1WIRE_ROM_2         , sizeof(value), &value); }
    { uint64_t value = OneWireRom[3]                   ; static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_1WIRE_ROM_3         , sizeof(value), &value); }
    {  int16_t value = OneWireRomData[0]               ; static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_1WIRE_ROM_DATA_0    , sizeof(value), &value); }
    {  int16_t value = OneWireRomData[1]               ; static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_1WIRE_ROM_DATA_1    , sizeof(value), &value); }
    {  int16_t value = OneWireRomData[2]               ; static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_1WIRE_ROM_DATA_2    , sizeof(value), &value); }
    {  int16_t value = OneWireRomData[3]               ; static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_1WIRE_ROM_DATA_3    , sizeof(value), &value); }
    
    {  int16_t value = LpgGetMvRaw                   (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_LPG_MV              , sizeof(value), &value); }
    {  int16_t value = LpgGetResistance16ths         (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_LPG_RESISTANCE      , sizeof(value), &value); }
    {  int16_t value = LpgGetResistanceMin16ths      (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_LPG_RESISTANCE_MIN  , sizeof(value), &value); }
    {  int16_t value = LpgGetResistanceMax16ths      (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_LPG_RESISTANCE_MAX  , sizeof(value), &value); }
    {  int16_t value = LpgGetVolumeMinMl             (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_LPG_VOLUME_MIN      , sizeof(value), &value); }
    {  int16_t value = LpgGetVolumeMaxMl             (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_LPG_VOLUME_MAX      , sizeof(value), &value); }
    {  int16_t value = LpgGetVolumeMl                (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_LPG_VOLUME          , sizeof(value), &value); }
    
    { uint64_t value = AmbientGetOutsideRom          (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_AMBIENT_OUTSIDE_ROM , sizeof(value), &value); }
    { uint64_t value = AmbientGetHeatingRom          (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_AMBIENT_HEATING_ROM , sizeof(value), &value); }
    {  int16_t value = AmbientGetOutsideTemp16ths    (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_AMBIENT_OUTSIDE_TEMP, sizeof(value), &value); }
    {  int16_t value = AmbientGetHeatingTemp16ths    (); static struct CanTransmitState state; CanTransmitOnChange(&state, CAN_ID_TANK, CAN_ID_AMBIENT_HEATING_TEMP, sizeof(value), &value); }
    
}
