
#ifndef TIMERS_H
#define	TIMERS_H

#include <xc.h> // include processor files - each processor file is guarded.  
typedef struct
{
  unsigned int *pSet;
  unsigned int Rem;
  struct{
         unsigned STOP:1;
         unsigned GO:1;
         unsigned unused:14;
        };  
}TIMER;

void StartTimer (TIMER *pt);
void GoTimer (TIMER *pt);
void ResetTimer(TIMER *pt);

#endif	/* TIMERS_H */

