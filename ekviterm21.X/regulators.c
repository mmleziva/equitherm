#include "regulators.h"
#include "user.h"
#define IMAX 0x0fff
#define MINPULS 0x0400//t
bool signum;
_Q16 integ,difla;

/*calc temperature of Pt1000 sensor with pull up resistor 1000ohm about -50 ..+100°C*/
//_Q16 Pt1000(int adc)
_Q16 Pt1000(_Q16 adc)
{
     fixed aux, coef,delta;
     //aux.I=adc;
     //aux.F=0;
     aux.IF=adc;
     coef.I=1;
     coef.F=0;
     //delta.IF=  _Q16mpy(aux.IF, 0x20);
     delta.IF=  (aux.IF >> 15);
     coef.IF  += delta.IF;
     aux.IF= _Q16mpy(aux.IF, coef.IF);
     aux.IF= aux.IF >>6;
     return aux.IF;
}

_Q16 equitherm(_Q16 slope ,_Q16 Toa, _Q16 shift)
{
   fixed Tc;
   _Q16 dT,result;
   Tc.I=25;
   Tc.F=0;
   Tc.IF +=shift;
   dT= Tc.IF - Toa;
   //result= _Q16mpy(slope<<1,dT);
   result= _Q16mpy(slope,dT);
   result= result + Tc.IF;
   return result;
}

unsigned int PID(_Q16 delta ,unsigned int Kp, unsigned int Ki,unsigned int Kd,unsigned int limit)
{
   fixed fko, sgsum;
   _Q16  sum,dif;
   int result;
   fko.I=0;
   fko.F= Kp;
   sum = _Q16mpy(fko.IF, delta);
   fko.F= Ki;
   integ  += _Q16mpy(fko.IF, delta);
   if(integ > IMAX) integ= IMAX;
   else if(integ < -IMAX) integ= -IMAX;
   sum  += integ;
   fko.F= Kd;
   dif= _Q16mpy(fko.IF, delta);
   sum= sum + (dif- difla);
   difla= dif;
   if(sum< 0)
   {
       signum = true;
       sgsum.IF= -sum;
   }
   else 
   {signum= false;
    sgsum.IF= sum;  
   }
   if (sgsum.IF > limit)result= limit;
   else if(sgsum.IF < MINPULS)result=0;
   else result= sgsum.F;
   return result;
}
