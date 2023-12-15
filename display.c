#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <xc.h>
#include <string.h>
#include <stdlib.h>

#include "../mstimer.h"
#include "../msticker.h"
#include "../eeprom.h"
#include "../lcd-1602.h"

#include "fresh-sensor.h"
#include "fresh-depth.h"
#include "fresh-water.h"
#include "accelerometer.h"
#include "tilt.h"
#include "adc.h"
#include "keypad.h"
#include "eeprom-this.h"
#include "lpg.h"

#define REPEAT_TIME_MS    1000

#define PAGE_NONE  0
#define PAGE_HOME  1
#define PAGE_FRESH 2
#define PAGE_TILT  3
#define PAGE_TEMP  4
#define PAGE_LPG   5
#define MAX_PAGE 5

#define LINE_LENGTH 16

static char line0[17];
static char line1[17];

static uint8_t _displayOnTime  = 0;
static  int8_t _page           = 1; //0 == lcd off
static  int8_t _setting        = 0; //0 == display page

void DisplayInit()
{
    _displayOnTime = EepromReadU8(EEPROM_DISPLAY_ON_TIME_U8);
}

static uint32_t getStayOnTimeSeconds()
{
    switch (_displayOnTime)
    {
        case  1: return 10UL;
        case  2: return 30UL;
        case  3: return  1UL * 60;
        case  4: return 10UL * 60;
        case  5: return 30UL * 60;
        case  6: return  1UL * 60 * 60;
        case  7: return  3UL * 60 * 60;
        case  8: return  6UL * 60 * 60;
        case  9: return 12UL * 60 * 60;
        case 10: return  1UL * 24 * 60 * 60;
        case 11: return  3UL * 24 * 60 * 60;
        case 12: return  1UL * 7 * 24 * 60 * 60;
        case 13: return  2UL * 7 * 24 * 60 * 60;
        case 14: return  4UL * 7 * 24 * 60 * 60;
        default: return 0UL;
    }
}
static void incrementStayOnFactor()
{
    if (_displayOnTime < 14) _displayOnTime++;
    else                     _displayOnTime = 14;
    EepromSaveU8(EEPROM_DISPLAY_ON_TIME_U8, _displayOnTime);
}
static void decrementStayOnFactor()
{
    if (_displayOnTime > 0) _displayOnTime--;
    EepromSaveU8(EEPROM_DISPLAY_ON_TIME_U8, _displayOnTime);
}
static int addStayOnTimeText(char* p) //Returns a length of 10
{
    switch (_displayOnTime)
    {
        case  1: strcpy(p, "10 seconds"); break;
        case  2: strcpy(p, "30 seconds"); break;
        case  3: strcpy(p, "1 minute  "); break;
        case  4: strcpy(p, "10 minutes"); break;
        case  5: strcpy(p, "30 minutes"); break;
        case  6: strcpy(p, "1 hour    "); break;
        case  7: strcpy(p, "3 hours   "); break;
        case  8: strcpy(p, "6 hours   "); break;
        case  9: strcpy(p, "12 hours  "); break;
        case 10: strcpy(p, "1 day     "); break;
        case 11: strcpy(p, "3 days    "); break;
        case 12: strcpy(p, "1 week    "); break;
        case 13: strcpy(p, "2 weeks   "); break;
        case 14: strcpy(p, "4 weeks   "); break;
        default: strcpy(p, "Indefinite"); break;
    }
    return 10;
}
static int addCurrent(char* p, int32_t ma) //Puts current into a string of 6 characters followed by a NUL -> 7 in total
{
    char polarity = ma >= 0;
    if (!polarity) ma = -ma;
    if      (ma == 0 ) p[0] = ' ';
    else if (polarity) p[0] = '+';
    else               p[0] = '-';
    if (ma > 999999L)
    {
        p[1] = '>';
        p[2] = '9';
        p[3] = '9';
        p[4] = '9';
        p[5] = 'A';
        p[6] = 0;
    }
    else
    {
        p++;
        int32_t amps = ma / 1000;
        int32_t rem  = ma % 1000;
        if      (amps > 99) snprintf(p, 6, "%3ldA "      , amps           ); //"120A "
        else if (amps >  9) snprintf(p, 6, "%2ld.%01ldA" , amps, rem / 100); //"99.1A"
        else if (amps >  0) snprintf(p, 6, "%1ld.%02ldA" , amps, rem /  10); //"3.75A"
        else                snprintf(p, 6, "%3ldmA"      , ma             ); //"533mA"
    }
    return 6;
}
static int addVoltage(char* p, int16_t mv) //Puts voltage into a string of 7 characters
{
    snprintf(p, 8, "%2d.%03dV", mv / 1000, mv % 1000);
    return 7;
}
static int addTenths(char* p, int16_t tenths) //Puts tenths into a string of 5 characters '-99.9' to ' 99.9'
{
    char positive = tenths >= 0;
    int16_t units = tenths / 10; //Could be positive or negative
    int16_t rem = (positive ? tenths : -tenths) % 10; //Always positive
    if (units)
    {
        snprintf(p, 6, "%3d.%d", units, rem);
    }
    else
    {
        if (positive) snprintf(p, 6, "  0.%d", rem);
        else          snprintf(p, 6, " -0.%d", rem);
    }
    return 5;
    
}
static int addTemperatureTenths(char* p, int16_t tenths) //Puts temperature into a string of 5 characters '-9.9*' to '99.9*'
{
    char positive = tenths >= 0;
    if (positive)
    {
        snprintf(p, 5, "%2d.%d", tenths / 10, tenths % 10);
    }
    else
    {
        tenths = -tenths;
        snprintf(p, 5, "-%1d.%d", tenths / 10, tenths % 10); //NB format is not el dee but one dee
    }
    *(p+4) = 0xdf; //lcd degree symbol
    return 5;
}
static int addTemperature12bit(char*p, int16_t temp12bit)
{
    int tenths = temp12bit * 10 / 16;
    return addTemperatureTenths(p, tenths);
}
static int addPercentSigned(char* p, int8_t percent) //Puts a percentage into a string of 4 characters
{
    snprintf(p, 5, "%3d%%", percent);
    return 4;
}
static int addPercent(char* p, uint8_t percent) //Puts a percentage into a string of 4 characters
{
    snprintf(p, 5, "%3u%%", percent);
    return 4;
}
static int addString(char* p, const char* pText)
{
    int length = 0;
    while(*pText && length < LINE_LENGTH)
    {
        *p++ = *pText++;
        length++;
    }
    return length;
}
static void displayHome0()
{
    int16_t waterLitres = FreshWaterGetLitres();
    int16_t lpgTenths   = (int16_t)((int32_t)LpgGetVolumeMl() * 10 / 1024);
    int16_t pitchTenths = TiltGetPitchAsTenths();
    int16_t rollTenths  = TiltGetRollAsTenths();
    
    char* p = line0;
    p += snprintf(p, 8, "W%4d", waterLitres);
    p += addString(p, "   G");
    p += addTenths(p, lpgTenths);
    
    p = line1;
    p += addString(p, "P");
    p += addTenths(p, pitchTenths);
    *p++ = 0xdf; //lcd degree symbol
    p += addString(p, " R");
    p += addTenths(p, rollTenths);
    *p++ = 0xdf; //lcd degree symbol
}
static void displayHome1()
{
    addString(line0, "Display off?");
}
static void displayHome2()
{
    addString(line0, "Display on time?");
    addStayOnTimeText(line1);
}
static void displayHome3()
{
    snprintf(line0, 17, "Tick len? %u", MsTickerGetLength());
    snprintf(line1, 17, "Ext-Int %+ldms", MsTickerGetExtMinusIntMs());
}

static void displayFresh0()
{
    int16_t mvRaw = FreshSensorGetMvRaw();
    int16_t mvComp = FreshSensorGetMvCompensatedForTemperature();
    int16_t mm = FreshDepthGetMmRaw();
    char* p = line0;
    p += addString(p, "Fresh mV");
    p += snprintf(p, 10, "%4d%4d", mvRaw, mvComp);
    p = line1;
    p += addString(p, "      mm");
    p += snprintf(p, 5, "%4d", mm);
}
static void displayFresh1()
{
    char* p = line0;
    p += snprintf(p, 16, "Width?  %4d", FreshWaterGetTankWidth());
    p = line1;
    p += snprintf(p, 16, "Length  %4d", FreshWaterGetTankLength());
}
static void displayFresh2()
{
    char* p = line0;
    p += snprintf(p, 16, "Width   %4d", FreshWaterGetTankWidth());
    p = line1;
    p += snprintf(p, 16, "Length? %4d", FreshWaterGetTankLength());
}
static void displayTilt0()
{
    int16_t x = AccelerometerGetX();
    int16_t y = AccelerometerGetY();
    int16_t z = AccelerometerGetZ();
    char* p = line0;
    p += addString(p, "Tilt");
    p = line1;
    p += snprintf(p, 16, "%4d,%4d,%4d", x, y, z);
}
static void displayTemp0()
{
    char* p = line0;
    p += addString(p, "Temp ");
    int16_t base16ths = FreshSensorGetBaseTemp16ths();
    int16_t uvPer16th = FreshSensorGetUvPer16th();
    p += snprintf(p, 17, "%4d %4d", base16ths, uvPer16th);
    
    p = line1;
}
static void displayLpg0()
{
    int16_t mv = LpgGetMvRaw();
    int16_t ohms = LpgGetResistance16ths() / 16;
    int16_t percent = LpgGetVolumePercent();
    int16_t litres = LpgGetVolumeMl() / 1024;
    char* p = line0;
    p += snprintf(p, 16, "Lpg %3d%%%3dL", percent, litres);
    p = line1;
    p += snprintf(p, 16, "%4dmV %4dR", mv, ohms);
}

static uint32_t addUint32(uint32_t oldValue, uint32_t amount) { return oldValue < (uint32_t)-amount ?           oldValue + amount  : (uint32_t)-1; }
static uint16_t addUint16(uint16_t oldValue, uint16_t amount) { return oldValue < (uint16_t)-amount ?           oldValue + amount  : (uint16_t)-1; }
static uint8_t  addUint8 (uint8_t  oldValue, uint8_t  amount) { return oldValue < (uint8_t )-amount ? (uint8_t)(oldValue + amount) : (uint8_t )-1; }
static uint32_t subUint32(uint32_t oldValue, uint32_t amount) { return oldValue >            amount ?           oldValue - amount  :            0; }
static uint16_t subUint16(uint16_t oldValue, uint16_t amount) { return oldValue >            amount ?           oldValue - amount  :            0; }
static uint8_t  subUint8 (uint8_t  oldValue, uint8_t  amount) { return oldValue >            amount ? (uint8_t)(oldValue - amount) :            0; }

static void adjustSetting(char increase, uint16_t amount)
{
    switch (_page)
    {
        case PAGE_HOME:
            switch (_setting)
            {
                case 1:
                {
                    if (increase) _page = PAGE_NONE; //Turn off display
                    _setting = 0;
                    break;
                }
                case 2:
                {
                    if (increase) incrementStayOnFactor();
                    else          decrementStayOnFactor();
                    break;
                }
                case 3:
                {
                    uint16_t newValue;
                    if (increase) newValue = addUint16(MsTickerGetLength(), amount);
                    else          newValue = subUint16(MsTickerGetLength(), amount);
                    MsTickerSetLength(newValue);
                    break;
                }
            }
            break;
        case PAGE_FRESH:
            switch (_setting)
            {
                case 1:
                {
                    uint16_t newValue;
                    if (increase) newValue = addUint16(FreshWaterGetTankWidth(), amount);
                    else          newValue = subUint16(FreshWaterGetTankWidth(), amount);
                    FreshWaterSetTankWidth(newValue);
                    break;
                }
                case 2:
                {
                    uint16_t newValue;
                    if (increase) newValue = addUint16(FreshWaterGetTankLength(), amount);
                    else          newValue = subUint16(FreshWaterGetTankLength(), amount);
                    FreshWaterSetTankLength(newValue);
                    break;
                }
            }
            break;
        case PAGE_TEMP:
            break;
    }
}
void DisplayMain()
{
    static uint32_t msStayOnTimer = 0;
    static uint32_t msRepetitiveTimer = 0;
    static int8_t     currentPage = 0;
    static int8_t     currentSetting = 0;
    
    char settingChanged = 0;
    unsigned int amount = 1;
    switch (KeypadMultiplier)
    {
        case 0:  amount =     1; break;
        case 1:  amount =    10; break;
        case 2:  amount =   100; break;
        default: amount =  1000; break;
    }
    
    if (KeypadOneShot) msStayOnTimer = MsTimerCount;
    uint32_t stayOnTimeMs = getStayOnTimeSeconds() * 1000;
    if (stayOnTimeMs && MsTimerRelative(msStayOnTimer, stayOnTimeMs)) _page = 0;
    
    static int8_t lastPage    = 1;
    static int8_t lastSetting = 0;
    
    if (KeypadOneShot)
    {
        if (!_page)
        {
            _page = lastPage ? lastPage : PAGE_HOME;
            _setting = lastSetting;
        }
        else
        {
            if (KeypadOneShot & 1) //Home
            {
                if (_setting) _setting = 0;
                else          _page = PAGE_HOME;
            }

            if (KeypadOneShot & 2) //Down
            {
                if (_setting)
                {
                    adjustSetting(0, amount);
                }
                else
                {
                    _page--;
                    if (_page < 1) _page = MAX_PAGE;
                }
            }

            if (KeypadOneShot & 4) //Up
            {
                if (_setting)
                {
                    adjustSetting(1, amount);
                }
                else
                {
                    _page++;
                    if (_page > MAX_PAGE) _page = 1;
                }
            }

            if (KeypadOneShot & 8) //Toggle setting and display mode
            {
                if (_setting)
                {
                    _setting++;
                    switch (_page)
                    {
                        case PAGE_HOME  : if (_setting > 3) _setting = 0; break;
                        case PAGE_FRESH : if (_setting > 2) _setting = 0; break;
                        case PAGE_TILT  : if (_setting > 0) _setting = 0; break;
                        case PAGE_TEMP  : if (_setting > 2) _setting = 0; break;
                        case PAGE_LPG   : if (_setting > 0) _setting = 0; break;
                    }
                }
                else
                {
                    _setting = 1;
                }
            }
            lastPage = _page;
            lastSetting = _setting;
        }
    }
    
    if (!_page &&  LcdIsOn()) LcdTurnOff();
    if ( _page && !LcdIsOn()) LcdTurnOn();
    
    if ( _page && 
         LcdIsReady() && 
         (MsTimerRepetitive(&msRepetitiveTimer, REPEAT_TIME_MS) || _page != currentPage || _setting != currentSetting || settingChanged))
    {
        for (int i = 0; i < 16; i++) line0[i] = ' ';
        for (int i = 0; i < 16; i++) line1[i] = ' ';
        switch (_page)
        {
            case PAGE_HOME:
            {
                switch (_setting)
                {
                    case 0: displayHome0(); break;
                    case 1: displayHome1(); break;
                    case 2: displayHome2(); break;
                    case 3: displayHome3(); break;
                }
               break;
            }
            case PAGE_FRESH:
            {
                switch (_setting)
                {
                    case 0: displayFresh0(); break;
                    case 1: displayFresh1(); break;
                    case 2: displayFresh2(); break;
                }
                break;
            }
            case PAGE_TILT:  displayTilt0(); break;
            case PAGE_TEMP:  displayTemp0(); break;
            case PAGE_LPG:   displayLpg0 (); break;
        }
       
       LcdSendText(line0, line1);
       currentPage = _page;
       currentSetting = _setting;
    }
}
