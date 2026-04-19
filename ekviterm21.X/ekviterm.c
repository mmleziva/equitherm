#include "oc.h"
#include "encoder.h"

#pragma config PWMLOCK=OFF
#pragma config ICS=PGD1
//#pragma config FNOSC = FRCPLL
#pragma config FNOSC = FRCDIVN      //!t
#pragma config IESO = OFF
#pragma config OSCIOFNC = OFF
#pragma config FWDTEN = ON  //t
#pragma config GWRP = OFF
#pragma config DMTEN = DISABLE//t

//#define PLL 63

//#define  PUMPIN DI2
//#define SERVO_OPENING Y4
//#define SERVO_CLOSING Y5
//#define NREGS 10

#include "analog.h"

//unsigned long int talpre;
//inline
void initOsc()
{
     //  OSCCONbits.NOSC= 7; //internal RC divided by N
    CLKDIVbits.FRCDIV=1;//FRC divided by 2
   // PLLFBDbits.PLLDIV=PLL; 
   // CLKDIVbits.PLLPOST=0;
   // CLKDIVbits.DOZE=1;  //
    //REFOCONbits.ROON=1;    //!t     
}
/*
//Init T2 timer				// input filter timer
void initT2(void)
{
 TMR2 = 0;
 PR2 = 27500;//0XFFFF;				//8Hz
 //PR2 = 0X1FF;//t				//1000pulses divide 20kHz  to 20Hz
 T2CONbits.TCKPS = 3;		//256   PRESCALLER 
 T2CONbits.TON = 1;		// turn on timer 1 
 _T2IF=0;
} 
*/

void initIO(void)
{
    ANSELA=ANSELB=ANSELC=0;
    
    _CNPUC0= 1;     //pull up IN1
    _CNPUC1= 1;     //pull up IN2
    _CNPUC2= 1;     //pull up IN3
    _CNPUA8= 1;     //pull up IN4
    _CNPUA9= 1;     //pull up IN5
    
    _TRISC3= 0;     //enable Y1
    _TRISC4= 0;     //enable Y2
    _TRISC5= 0;     //enable Y3
    _TRISB7= 0;     //enable Y4
    _TRISB8= 0;     //enable Y5
    _TRISB9= 0;     //enable Y6
    _TRISC6= 0;     //enable Y7
    
    _TRISC7= 0;     //enable LED1
    _TRISC8= 0;     //enable LED2    
}



int main(void)
{
    initOsc();//config oscilator
    initIO(); //config inputs and outputs
    //initT2(); //config timer
    initAdc1();//config A/D converter
    initOC();//config output compare as pwm for servo 
    _GIE=1;
    cfgLCD();
    initEncoder();  
   // SERVO_CLOSING=1;
    while(1)
    {       //if TRUN writing parameters ino LCD
      asm("clrwdt");
      if(REQ && !TRUN)  //an request to display next parameter or value
      {
          REQ= false;
          if(CONT)
          {                         //setting value
             strp(inkrem, true);
             wristrLCD(LONG); 
          }
          else
          {                         // next param
            strc(k);
            strp(inkrem, false);
            wristrLCD(2*LONG);
          }
      }
      else if(RETE && !TRUN)
      {                 //an request to display new scanned temperature
              RETE= false;
              strp(inkrem, false);
              wristrLCD(LONG); 
      }
      else if(EPW && !TRUN)//switch between changing of parameters and values 
      {
       EPW= false;
       if(CONT)
       {        //after switch to value setting  mode  display  value
             strp(inkrem, true);
             wristrLCD(LONG); 
       }
       else
       {        // after turn  to parameter setting also write to flash  
        qq=pq;
        _erase_flash(qq);
        for(j=0; j< NVPAR; j+=2)
        {
         _write_flash_word32(qq, par.A[j], par.A[j+1]);
         qq +=4;
        }
        strp(inkrem, false);
        wristrLCD(LONG);
       }
      }
    }
    return 0;
}
