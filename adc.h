
extern void AdcInit(void);
extern char AdcHadInterrupt(void);
extern void AdcHandleInterrupt(void);

extern char AdcValue0IsValid;
extern char AdcValue1IsValid;
extern char AdcValue2IsValid;

extern uint16_t AdcGetValue0(void);
extern uint16_t AdcGetValue1(void);
extern uint16_t AdcGetValue2(void);

extern int16_t AdcGetMv0(void);
extern int16_t AdcGetMv1(void);
extern int16_t AdcGetMv2(void);

#define ADC_MV_OFFSET_0 -11
#define ADC_MV_OFFSET_1 -14
#define ADC_MV_OFFSET_2 -14

#define ADC_VREF_MV 2038
#define ADC_BITS 16
