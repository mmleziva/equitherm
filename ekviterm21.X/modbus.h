#ifndef MODBUS_H
#define	MODBUS_H
#define MBUART1
//#define MBUART2
#include "user.h"
#include "crc.h"
#ifdef MBUART1 
#include "uart1Drv.h"
#endif
#ifdef MBUART2 
#include "uart2Drv.h"
#endif
extern void cfgModbus(void);
extern void modtra(void);
extern void modrec(void);
#define NREGS 32
#define INREGS 32
extern WORD HOLDREGSR[NREGS], HOLDREGSW[NREGS],INPREGS[INREGS];
extern unsigned int ADR0COILS;    
extern unsigned int ADR0DISCIN; 
extern unsigned int ADR0INPREG; 
extern unsigned int ADR0HOLDREG; 
typedef struct 
{
 unsigned char unitId;
 unsigned char funCode;
 WORD startAdr;
 WORD regCount;
 char byteCount;
 WORD *pdat;    
 unsigned int CRC;
 unsigned char errcode; 
 //unsigned char master: 1; 
}MODBUSTRUCT;

#endif  //MODBUS_H