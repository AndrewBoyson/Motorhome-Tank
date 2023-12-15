#include <stdint.h>
#include <stdbool.h>

#include "../mstimer.h"
#include "../eeprom.h"
#include "../i2c.h"

#include "i2c-this.h"
#include "eeprom-this.h"


static int16_t _accelerometerX = 0;
static int16_t _accelerometerY = 0;
static int16_t _accelerometerZ = 0;

static int16_t _accelerometerXFlat = 0;
static int16_t _accelerometerYFlat = 0;
static int16_t _accelerometerZFlat = 0;
char AccelerometerIsValid = 0;

int16_t AccelerometerGetX(void) { return _accelerometerX - _accelerometerXFlat; }
int16_t AccelerometerGetY(void) { return _accelerometerY - _accelerometerYFlat; }
int16_t AccelerometerGetZ(void) { return _accelerometerZ - _accelerometerZFlat; }

int16_t AccelerometerGetXFlat() { return _accelerometerXFlat; }
int16_t AccelerometerGetYFlat() { return _accelerometerYFlat; }
int16_t AccelerometerGetZFlat() { return _accelerometerZFlat; }

void    AccelerometerSetXFlat(int16_t value) { _accelerometerXFlat = value; EepromSaveS16(EEPROM_ACCELEROMETER_X_FLAT_S16, value); }
void    AccelerometerSetYFlat(int16_t value) { _accelerometerYFlat = value; EepromSaveS16(EEPROM_ACCELEROMETER_Y_FLAT_S16, value); }
void    AccelerometerSetZFlat(int16_t value) { _accelerometerZFlat = value; EepromSaveS16(EEPROM_ACCELEROMETER_Z_FLAT_S16, value); }

void AccelerometerInit()
{
    _accelerometerXFlat = EepromReadS16(EEPROM_ACCELEROMETER_X_FLAT_S16);
    _accelerometerYFlat = EepromReadS16(EEPROM_ACCELEROMETER_Y_FLAT_S16);
    _accelerometerZFlat = EepromReadS16(EEPROM_ACCELEROMETER_Z_FLAT_S16);
}

#define OVERSAMPLE_RATE 6
static void addSample(int16_t x, int16_t y, int16_t z)
{
    static int32_t totalX = 0;
    static int32_t totalY = 0;
    static int32_t totalZ = 0;
    static uint16_t count = 0;
    static char     valid = 0;
    
    AccelerometerIsValid = 1;
    totalX += x;
    totalY += y;
    totalZ += z;
    count++;
    if (count >= (1 << OVERSAMPLE_RATE))
    {
        _accelerometerX = (int16_t)(totalX >> OVERSAMPLE_RATE);
        _accelerometerY = (int16_t)(totalY >> OVERSAMPLE_RATE);
        _accelerometerZ = (int16_t)(totalZ >> OVERSAMPLE_RATE);
        totalX = 0;
        totalY = 0;
        totalZ = 0;
        count = 0;
        valid = 1;
    }
    else
    {
        if (!valid)
        {
            _accelerometerX = (int16_t)(totalX / count);
            _accelerometerY = (int16_t)(totalY / count);
            _accelerometerZ = (int16_t)(totalZ / count);
        }
    }
}

static int setNormalPower()
{
    int result = 0;
    uint8_t bytes[2];
    
    bytes[0] = 0x11; //Power mode register
    bytes[1] = 0x1E; //PWR_MODE = Normal; LOW_POWER_BW = 500Hz

    //Write register address to read from
    I2CSend(I2C_ADDRESS_ACCELEROMETER, 2, bytes, &result);
    if (result) return result;
    
    return 0;
}
static int readReg(uint8_t regadd, uint8_t* pByte) // return 0 on success
{
    int result = 0;

    //Write register address to read from
    I2CSend(I2C_ADDRESS_ACCELEROMETER, 1, &regadd, &result);
    if (result) return result;

    //Read byte from register
    I2CReceive(I2C_ADDRESS_ACCELEROMETER, 1, pByte, &result);
    if (result) return result;

    return 0;
}
void AccelerometerMain()
{
    static uint32_t msTimerRepetitive = 0;
    if (MsTimerRepetitive(&msTimerRepetitive, 100UL))
    {   
        uint8_t byte = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t z = 0;
        
        if (setNormalPower()) return;
        
        //Read x
        if (readReg(0x02, &byte)) return;
        x  = byte;
        if (readReg(0x03, &byte)) return;
        x |= byte << 8;
        
        //Read y
        if (readReg(0x04, &byte)) return;
        y  = byte;
        if (readReg(0x05, &byte)) return;
        y |= byte << 8;
        
        //Read z
        if (readReg(0x06, &byte)) return;
        z  = byte;
        if (readReg(0x07, &byte)) return;
        z |= byte << 8;
        
        addSample(x, y, z);
    }
}