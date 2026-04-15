//#include "user.h"
#include "regulators.h"
#include "analog.h"
#include "lcdser.h"
#ifndef ENCODER_H
#define ENCODER_H
#define INK1 _RB12
#define INK2 _RB13
//#define FLASH 0x1000 //address of dataflash in program memory
#define SW _RA10
//#include <xc.h> // include processor files - each processor file is guarded.  
extern volatile PARAMETERS par;
extern int volatile inkrem;
extern bool CONT,RETE;
extern bool EPW;
extern unsigned int j;
//extern int k;
extern _prog_addressT pq,qq;
void initEncoder(void);
#endif	/* ENCODER_H */

