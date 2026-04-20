#include "lcdser.h"
//#define TEPS 6
#define DMA     //if DMA is used 
__attribute__((aligned(2))) char disp[2*LONG];
char  *pt;
int NT;
bool TRUN,REQ;
int jk;
int k;
int decimals[NPAR];
const char napis[NPAR][LONG-2]={
    "Ekviterma-sklon ",
    "Ekviterma posun ",
    "Min.tep.zpatecka",
    "PID regulace: P ",
    "PID regulace: I ",
    "PID regulace: D ",
    "Maxim. puls rele ",
    "Teplota vody byt",
    "Teplota zpatecka",
    "Teplota venkovni",
    "Vypoc.ekvit.tep."
};

//inline 
void setU1pins(void)
{
    RPINR18bits.U1RXR = 20; // U1RX on RP20
    RPOR1bits.RP36R = 0x01; // U1TX on RP36
}
// UART Configuration
//inline
void cfgUart1(void)
{    
    setU1pins();   
	U1MODEbits.STSEL = 0;			// 1-stop bit
	//U1MODEbits.PDSEL = 0b10;			// Odd Parity, 8-data bits
	U1MODEbits.ABAUD = 0;			// Autobaud Disabled
    U1MODEbits.BRGH =  1;//t
	U1BRG = BRGVAL;					// BAUD Rate Setting for 9600bps
	U1STAbits.UTXISEL0 = 0;		// interrupt after 
	U1STAbits.UTXISEL1 = 0;		//!t    interrupt after transfer to TSR   
	U1STAbits.URXISEL  = 0;			// Interrupt after one RX character is received
	U1MODEbits.UARTEN   = 1;		// Enable UART
	U1STAbits.UTXEN     = 1;		// Enable UART Tx
   _U1TXIF=0;
#ifndef DMA
 //   IEC4bits.U1EIE = 1;             //enable error com interrupt
    _U1TXIP=5;  //high int priority
 //  _U1RXIE=1;				//enable receiver interrupt
   _U1TXIE=1; //t 
 
#endif
}

void cfgDMA0(void)
{ 
 //DMA0CONbits.CHEN=1;
 DMA0CONbits.SIZE=1;//byte
 //   DMA0CONbits.SIZE=0;//word
 DMA0CONbits.DIR=1; //RAM-to-Peripheral
 DMA0CONbits.AMODE=0;// Post-Increment
 DMA0CONbits.MODE=1;// One-Shot
 //DMA0CNT = 2*LONG-1;
// DMA0CNT = 1;//2bytes
 DMA0REQ = 0b1100;// Select UART1 Transmitter
 DMA0PAD = (volatile unsigned int) &U1TXREG;
 DMA0STAL = __builtin_dmaoffset(disp);
 DMA0STAH = __builtin_dmapage(disp);
 //DMA0STAH = 0;
 _DMA0IP=1;     //low int priority
 IFS0bits.DMA0IF = 0; // Clear DMA Interrupt Flag
 IEC0bits.DMA0IE = 1;// Enable DMA Interrupt
 //DMA0CONbits.CHEN=1;
}
 
void __attribute__((__interrupt__,no_auto_psv)) _DMA0Interrupt(void)
{  
    IFS0bits.DMA0IF = 0;   // Clear the DMA0 Interrupt Flag
     TRUN= false;//t
} 
/*
void __attribute__((__interrupt__,no_auto_psv)) _U1TXInterrupt(void)
{
    NT--;
   _U1TXIF=0;
   if(NT>0)
   {
       pt++;
       U1TXREG=*pt;
   }
   else TRUN= false;
}
*/

inline void cfgLCD(void)
{
                        //config sequence for 2.row
 disp[0]=0xfe;     
// disp[1]= 0x41;//display on
 disp[1]=0x45;//set cursor
 disp[2]=0x40;//2.row
 //cfgDMA0();   
 cfgUart1();
#ifdef DMA
 cfgDMA0();
#endif
 //DMA0CONbits.CHEN=1;//start transfer
 // wristrLCD(2);//display on
                        //config sequence for 1.row
 disp[LONG]=0xfe;
 disp[LONG+1]=0x45;//set cursor
 disp[LONG+2]=0x0;//1.row
// disp[1]=0x45;//set cursor
// disp[2]=0x40;//2.row
 REQ= true;
}

void strc(int k)
{                           //write label to  memory for 2st line of the of display  
 memcpy(&disp[LONG+3],&napis[k][0],16); 
}

void strp(int ipar, bool NASTAV)
{                           //write data to memory for the 1st line of the display and mark "NASTAV" if the setting mode is
    if(NASTAV)
     sprintf(&disp[3],"NASTAV: ");
    else
     sprintf(&disp[3],"        ");
     sprintf(&disp[11],"%4d",ipar);
    if((k==1)||(k==2)||(k>=NVPAR))
    {                               //write temperatures 
     disp[15]='.';
     disp[16]=decimals[k]+0x30;//t
     disp[LONG-2]= 0xb2;//'°';
     disp[LONG-1]= 'C';     
    }
    else
    {
     disp[15]=' ';
     disp[16]=' ';
     disp[LONG-2]= ' ';
     disp[LONG-1]= ' ';    
    }   
}

void wristrLCD(int N)
{
    TRUN= true;
#ifndef DMA
     NT=N;
    pt= disp;
    U1TXREG= *pt; //t
//    DMA0CNT = 2*LONG-1;
 //   strcpy(&disp[LONG+3],&napis[N][0]);
  //  _U1TXIF=0;
#else
    DMA0CNT = N -1;
    DMA0CONbits.CHEN=1;//ENABLE dma
    DMA0REQbits.FORCE=1;//start transfer//T
#endif
}
