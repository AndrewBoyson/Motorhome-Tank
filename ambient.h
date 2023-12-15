#include <stdint.h>

extern uint64_t AmbientGetOutsideRom(void);
extern uint64_t AmbientGetHeatingRom(void);
extern void     AmbientSetOutsideRom(uint64_t value);
extern void     AmbientSetHeatingRom(uint64_t value);

extern int16_t  AmbientGetOutsideTemp16ths(void);
extern int16_t  AmbientGetHeatingTemp16ths(void);

extern void     AmbientInit(void);