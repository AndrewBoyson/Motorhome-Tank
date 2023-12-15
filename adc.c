#include <stdint.h>
#include <xc.h>

#include "../mstimer.h"

#include "adc.h"

#define OVERSAMPLE_RATE_0 14
#define OVERSAMPLE_RATE_1 14
#define OVERSAMPLE_RATE_2 14

static uint32_t _total0 = 0;
static uint16_t _count0 = 0;
static uint16_t _value0 = 0;

static uint32_t _total1 = 0;
static uint16_t _count1 = 0;
static uint16_t _value1 = 0;

static uint32_t _total2 = 0;
static uint16_t _count2 = 0;
static uint16_t _value2 = 0;

char     AdcValue0IsValid = 0;
char     AdcValue1IsValid = 0;
char     AdcValue2IsValid = 0;

uint16_t AdcGetValue0()
{
    uint16_t value;
    di();
        value = _value0;
    ei();
    return value;
}
uint16_t AdcGetValue1()
{
    uint16_t value;
    di();
        value = _value1;
    ei();
    return value;
}
uint16_t AdcGetValue2()
{
    uint16_t value;
    di();
        value = _value2;
    ei();
    return value;
}

int16_t AdcGetMv0()
{
    if (!AdcValue0IsValid) return 0;
    int32_t adcMv = (ADC_VREF_MV * (int32_t)AdcGetValue0()) >> ADC_BITS;
    return (int16_t)adcMv + ADC_MV_OFFSET_0;
}

int16_t AdcGetMv1()
{
    if (!AdcValue1IsValid) return 0;
    int32_t adcMv = (ADC_VREF_MV * (int32_t)AdcGetValue1()) >> ADC_BITS;
    return (int16_t)adcMv + ADC_MV_OFFSET_1;
}
int16_t AdcGetMv2()
{
    if (!AdcValue2IsValid) return 0;
    int32_t adcMv = (ADC_VREF_MV * (int32_t)AdcGetValue2()) >> ADC_BITS;
    return (int16_t)adcMv + ADC_MV_OFFSET_2;
}
char AdcHadInterrupt()
{
    return ADIF;
}
void AdcHandleInterrupt()
{
    uint16_t value = ((uint16_t)ADRESH << 8) + ADRESL;
    
    switch (ADCON0bits.CHS)
    {
        case 0:
        {
            _total0 += value;    //Will contain 12 bit value * 4096 ==> 24 bit value
            _count0++;
            if (_count0 >= (1 << OVERSAMPLE_RATE_0))
            {
                _count0 = 0;
                _value0 = (uint16_t)(_total0 >> (12 + OVERSAMPLE_RATE_0 - 16)); //Decimate to 16 bits
                _total0 = 0;
                AdcValue0IsValid = 1;
            }
            ADCON0bits.CHS = 1;
            break;
        }
        case 1:
        {
            _total1 += value;    //Will contain 12 bit value * 4096 ==> 24 bit value
            _count1++;
            if (_count1 >= (1 << OVERSAMPLE_RATE_1))
            {
                _count1 = 0;
                _value1 = (uint16_t)(_total1 >> (12 + OVERSAMPLE_RATE_1 - 16)); //Decimate to 16 bits
                _total1 = 0;
                AdcValue1IsValid = 1;
            }
            ADCON0bits.CHS = 2;
            break;
        }
        case 2:
        {
            _total2 += value;    //Will contain 12 bit value * 4096 ==> 24 bit value
            _count2++;
            if (_count2 >= (1 << OVERSAMPLE_RATE_1))
            {
                _count2 = 0;
                _value2 = (uint16_t)(_total2 >> (12 + OVERSAMPLE_RATE_2 - 16)); //Decimate to 16 bits
                _total2 = 0;
                AdcValue2IsValid = 1;
            }
            ADCON0bits.CHS = 0;
            break;
        }
    }
    ADCON0bits.GO = 1; //Start conversion
    ADIF = 0;          //Clear the interrupt bit
}
void AdcInit(void)
{
	ADCON2bits.ADFM  = 1; // Right justified into lsb
	ADCON2bits.ACQT  = 7; // 20TAD
    
    ADCON2bits.ADCS  = 2; //TAD = 32 Tosc - A bit more than 16 recommended for 20MHz
	
	ADCON1bits.VCFG  = 2; //Vr+ uses internal 2.048v reference
	ADCON1bits.VNCFG = 0; //Vr- uses AVss
	ADCON1bits.CHSN  = 0; //-ve input <-- AVss
	
	ANCON0            = 0;
	ANCON0bits.ANSEL0 = 1;//AN0 configured as analogue
	ANCON0bits.ANSEL1 = 1;//AN1 configured as analogue
	ANCON0bits.ANSEL2 = 1;//AN2 configured as analogue
	ANCON0bits.ANSEL3 = 0;//AN3 configured as digital
	ANCON0bits.ANSEL4 = 0;//AN4 configured as digital
    ANCON1            = 0;//AN8-14 configured as digital
			
    ADCON0bits.CHS    = 0; //+ve input <-- AN0
    
	ADCON0bits.ADON  = 1; //Enable adc
    
    ADIF = 0;             //Clear the interrupt bit
    ADIE = 1;             //Enable interrupts
    ADCON0bits.GO = 1;    //Start conversion
    
}