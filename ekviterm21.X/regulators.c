#include "regulators.h"
#include "user.h"
#define IMAX 0x0fff
#define MINPULS 0x0400//t
#define KT_INV (1/0.0039083)
bool signum;
_Q16 integ,difla;

/*calc temperature of Pt1000 sensor with pull up resistor 1000ohm about -50 ..+100°C*/

_Q16 Pt1000(_Q16 adc)
/*
 * u= U/Um= Rt/(R+Rt); R=1000ohm,  Rt=(1+kt*T)*1000ohm, Pt1000: kt=0.0039083;
 * u=   (1 + kt*T)/ (2 + kt*T); 
 * kt*T= (2u-1)/(1-u) = 2 * u' / (1/2 - u') = = 4 * u' / (1 - 2 * u') =  4 * u' * (1 + 2 * u' + ...);  u' = u - 1/2;
 * T ~= (1 / kt)* (1 + 2 * u'); (1 / kt)= KT_INV = (1/0.0039083);
 */
{
     fixed aux, coef,delta,pre;
     delta.IF=adc>>16;
     pre.IF= _Q16mpy(delta.IF, delta.IF);
     aux.IF= delta.IF + 2*pre.IF;
     coef.F=0;
     coef.I=(4*KT_INV);
     aux.IF= _Q16mpy(aux.IF, coef.IF);
     return aux.IF;
}

_Q16 equitherm(_Q16 slope ,_Q16 Toa, _Q16 shift)
{                   //equitherm: Tc= 25+ shift;  dT= Tc - Toa ; result = Tc + slope * dT;
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
{   //result= Kp*delta + Ki*(integ= integ + delta)+ (-dif+ (dif=Kd * delta));   
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
