 
#ifndef OC_H
#define	OC_H
#include "user.h"
void OC1_active(unsigned int st);
void OC2_active(unsigned int st);
void shut_servo(void);
//#else
//#endif
#include <xc.h> // include processor files - each processor file is guarded. 

void initOC(void);
#endif	/* OC_H */

