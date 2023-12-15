#include <stdint.h>
extern int16_t AccelerometerGetX(void);
extern int16_t AccelerometerGetY(void);
extern int16_t AccelerometerGetZ(void);
extern int16_t AccelerometerGetXFlat(void);
extern int16_t AccelerometerGetYFlat(void);
extern int16_t AccelerometerGetZFlat(void);

extern void    AccelerometerSetXFlat(int16_t value);
extern void    AccelerometerSetYFlat(int16_t value);
extern void    AccelerometerSetZFlat(int16_t value);

extern char    AccelerometerIsValid;

extern void    AccelerometerInit(void);
extern void    AccelerometerMain(void);