
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCDSER_H
#define	LCDSER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "user.h"
extern bool TRUN,REQ;
#define LONG 19
extern char disp[2*LONG];
inline void cfgLCD(void);
void wristrLCD(int N);
void strc(int k);
void strp(int ipar, bool NASTAV);
extern int k;
extern int decimals[NPAR];
#endif	/* LCDSER_H */

