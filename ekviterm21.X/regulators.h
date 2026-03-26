#ifndef REGULATORS_H
#define	REGULATORS_H
#include "user.h"
#include <xc.h> // include processor files - each processor file is guarded. 
extern bool signum;
extern _Q16 integ,difla;
_Q16 equitherm(_Q16 slope ,_Q16 Toa, _Q16 shift);
_Q16 Pt1000(_Q16 adc);
unsigned int PID(_Q16 delta ,unsigned int Kp, unsigned int Ki,unsigned int Kd,unsigned int limit);
#endif	/* REGULATORS_H */

