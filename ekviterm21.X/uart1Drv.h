
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UART1DRV_H
#define	UART1DRV_H
#include "user.h"
#include <xc.h> // include processor files - each processor file is guarded. 
#define NP 1000
extern unsigned char bufout[],bufin[],*pbufout,*pbufin;
void cfgUart1(void);
void startU1tr(int n);
extern UARTFLAGS uarf1; 
extern  signed int ni;
inline void setU1pins(void);
extern unsigned int Npaus; 
extern void Autos(void);

#endif	/* UART1DRV_H */

