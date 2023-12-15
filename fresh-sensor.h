
extern int16_t FreshSensorGetSupplyMv(void);
extern int16_t FreshSensorGetTemperature16ths(void);
extern int16_t FreshSensorGetMvRaw(void);
extern int16_t FreshSensorGetMvCompensatedForTemperature(void);

extern  int16_t FreshSensorGetBaseTemp16ths(void);
extern  int16_t FreshSensorGetBaseMv       (void);
extern  int16_t FreshSensorGetUvPer16th    (void);
extern uint64_t FreshSensorGetRom          (void);

extern void     FreshSensorSetBaseTemp16ths( int16_t value);
extern void     FreshSensorSetBaseMv       ( int16_t value);
extern void     FreshSensorSetUvPer16th    ( int16_t value);
extern void     FreshSensorSetRom          (uint64_t value);

extern void FreshSensorInit(void);

#define FRESH_SENSOR_MV_AT_ZERO_PSI   500
#define FRESH_SENSOR_MV_AT_MAX_PSI   4500
#define FRESH_SENSOR_MAX_PSI 10