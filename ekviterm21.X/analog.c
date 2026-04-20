//#include <xc.h>
#include "user.h"
bool NORMAL;
int ad[4],ji;
SHORT daf;
volatile fixed fil[4];
_Q16 deltafil[4];

void initAdc1(void)
{
 ANSELAbits.ANSA0 = 1; // Ensure AN0/RA0 is analog  
 ANSELAbits.ANSA1 = 1; // Ensure AN1/RA1 is analog  
 ANSELBbits.ANSB0 = 1; // Ensure AN2/RB0 is analog  
 ANSELBbits.ANSB1 = 1; // Ensure AN3/RB1 is analog  
/* Initialize and enable ADC module */
 // Enable simultaneous sampling and auto-sample
AD1CON1bits.ASAM=1;         //sample auto start
AD1CON1bits.SSRCG=0;            //auto convert
AD1CON1bits.SSRC=7;            //auto convert
AD1CON1bits.FORM=0b11;      //signed int format
AD1CON1bits.AD12B=1;        //12 bit ADC
AD1CON2bits.BUFM= 0;        //starts filling buffer
AD1CON2bits.ALTS= 0;        //Always MUXA input

AD1CON2bits.CHPS=0B0;         //CONVERTS CH0
AD1CON2bits.CSCNA=1;        // Scan CH0+ input
 
AD1CON2bits.SMPI=0b011;      //int 1x to 4
//AD1CON3bits.ADCS= 0x7;       //conv. clk.
AD1CON3bits.ADCS= 0x40;       //conv. clk.
//AD1CON3bits.SAMC=0x9;         //sampl. time
AD1CON3bits.SAMC=0b11111;         //sampl. time
AD1CHS0bits.CH0SA=0;       //AN0 input select
AD1CON4 = 0x0000;
AD1CSSLbits.CSS0=1; //select AN0 for input scan
AD1CSSLbits.CSS1=1; //select AN1 for input scan
AD1CSSLbits.CSS2=1; //select AN2 for input scan
AD1CSSLbits.CSS3=1; //select AN3 for input scan
AD1CON1bits.ADON = 1;
_AD1IP=6;  //high int priority
_AD1IE=1;
//Delay_us(20);
}

void __attribute__((interrupt, no_auto_psv)) _AD1Interrupt (void)
{  
  //  bool B= false;
  _AD1IF=0;
  
  ad[0]=ADC1BUF0;
  ad[1]=ADC1BUF1;
  ad[2]=ADC1BUF2;  
  ad[3]=ADC1BUF3;
  if(!NORMAL)
  {                 //first AI
      NORMAL= true;//t
      for (ji=0; ji<4; ji++)  
      {
        fil[ji].I= ad[ji]; 
        fil[ji].F=0;
      }
  }
  else
  {                 //simple filtering
      for (ji=0; ji<4; ji++ )
      {        
          deltafil[ji] = ((_Q16)(ad[ji])<<16)- fil[ji].IF;  
          fil[ji].IF += (deltafil[ji]>>8);
          
      }       
  }
}