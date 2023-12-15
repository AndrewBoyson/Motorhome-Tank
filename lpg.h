#include <stdint.h>

extern void LpgInit(void);
extern void LpgMain(void);

extern int16_t LpgGetResistanceMin16ths(void);
extern int16_t LpgGetResistanceMax16ths(void);
extern int16_t LpgGetVolumeMinMl       (void);
extern int16_t LpgGetVolumeMaxMl       (void);

extern void    LpgSetResistanceMin16ths(int16_t value);
extern void    LpgSetResistanceMax16ths(int16_t value);
extern void    LpgSetVolumeMinMl       (int16_t value);
extern void    LpgSetVolumeMaxMl       (int16_t value);

extern int16_t LpgGetMvRaw(void);

extern int16_t LpgGetResistance16ths(void);
extern int8_t  LpgGetVolumePercent(void);
extern int16_t LpgGetVolumeMl(void);