#include <stdint.h>

extern int16_t FreshDepthGetSensorPosnFront(void);
extern int16_t FreshDepthGetSensorPosnRight(void);
extern void    FreshDepthSetSensorPosnFront( int16_t value);
extern void    FreshDepthSetSensorPosnRight( int16_t value);

extern int16_t FreshDepthGetMmRaw(void);
extern int16_t FreshDepthGetMmCompensatedForTilt(void);

extern void    FreshDepthInit(void);