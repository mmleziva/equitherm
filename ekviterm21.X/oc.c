
#include "oc.h"
//inline
//void
void setOCpins(void)
{
    RPOR2bits.RP39R = 0x10; // OC1 on RP39
    RPOR3bits.RP40R = 0x11; // OC2 on RP40
   // RPOR6bits.RP55R = 0x12; // OC3 on RP55
}

//inline 
void initOC(void)
{
    setOCpins();
    OC1CON1bits.OCTSEL=3;//T5CK time source
    OC1R= 4;
    //OC1CON1bits.OCM= 7;//PWM
    // OC1CON2bits.SYNCSEL=0x0;//no trigger 
    OC1CON2bits.SYNCSEL=0x0f;//T5 trigger 
    OC1CON2bits.TRIGSTAT= 1;
    OC2CON1bits.OCTSEL=3;//T5CK time source
    OC2R= 4;
    //OC2CON1bits.OCM= 7;//PWM
  //  OC2CON2bits.SYNCSEL=0x0;//t  no trigger 
    OC2CON2bits.SYNCSEL=0x0f;//T5 trigger 
     OC2CON2bits.TRIGSTAT= 1;
    T5CONbits.TCKPS =3;//1:256 prescaller T5CLK
    _T5IF= 0;
    _T5IE= 1;
    _T5IP= 2;
    T5CONbits.TON= 1;
    /*
    OC3R= 0x00;
    OC3CON1bits.OCTSEL=1;//T3CK time source
    OC3CON1bits.OCM= 5;//PWM
    OC3CON2bits.SYNCSEL=0x0d;//T3 trigger 
    OC3CON2bits.TRIGSTAT= 1;
    OC3CON2bits.OCINV=1;
    _OC3IE=1;
    _OC3IP=3;//t int priority
     */ 
    //T3CONbits.TCKPS =2;//1:64 prescaller T3CLK
    T3CONbits.TCKPS =1;//1:8 prescaller T3CLK
    T3CONbits.TON= 1;
    _T3IF= 0;
    _T3IP= 3;
    _T3IE= 1;
}

void OC1_active(unsigned int st)
{
   Y5= 0; 
   OC2CON1bits.OCM= 0;//disactive 
   OC1RS= st;
   OC1CON1bits.OCM= 7;//PWM 
}

void OC2_active(unsigned int st)
{
   Y4= 0; 
   OC1CON1bits.OCM= 0;//disactive 
   OC2RS= st;
   OC2CON1bits.OCM= 7;//PWM 
}

void shut_servo(bool POWER)
{
   Y4= POWER; 
   Y5= 0;
   OC1CON1bits.OCM= 0;//disactive 
   OC2CON1bits.OCM= 0;//disactive 
}

void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt (void)
{
  _OC1IF=0;
  TMR5=0;
  OC2TMR=0;
}

