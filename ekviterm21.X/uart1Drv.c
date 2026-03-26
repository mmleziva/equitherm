// UART Configuration
#include "uart1Drv.h"

signed int bci, bco, ni, no;
unsigned char bufout[BUFMAX+1],bufin[BUFMAX+1];
unsigned char   *pbufout,*pbufin;
UARTFLAGS uarf1;
unsigned int Npaus; 



// UART Configuration
void cfgUart1(void)
{    
    setU1pins();    
    pbufin=&bufin[0];
    pbufout=&bufout[0];       
	U1MODEbits.STSEL = 0;			// 1-stop bit
	//U1MODEbits.PDSEL = 0b10;			// Odd Parity, 8-data bits
	//U1MODEbits.ABAUD = 0;			// Autobaud Disabled
    U1MODEbits.ABAUD = 1;			//t Autobaud Disabled
    U1MODEbits.BRGH =  1;//t
	U1BRG = BRGVAL;					// BAUD Rate Setting for 115200


	//********************************************************************************
	//  STEP 1:
	//  Configure UART for DMA transfers
	//********************************************************************************/
	U1STAbits.UTXISEL0 = 0;		// interrupt after transfer completing
	U1STAbits.UTXISEL1 = 1;		//!t    interrupt after transfer to TSR              
	U1STAbits.URXISEL  = 0;			// Interrupt after one RX character is received

	//********************************************************************************
	//  STEP 2:
	//  Enable UART Rx and Tx
	//********************************************************************************/
	U1MODEbits.UARTEN   = 1;		// Enable UART
	U1STAbits.UTXEN     = 1;		// Enable UART Tx
    IEC4bits.U1EIE = 1;             //enable error com interrupt
    _U1RXIF=0;
    _U1RXIE=1;				//enable receiver interrupt
    _U1TXIF=0;
    _U1TXIE=1;
        
}

void startU1tr(int n)//start transmit of this card data to UART1->USB 
{
            pbufout=&bufout[0];
            bco =*pbufout;  
            no=n;
            uarf1.ErrPar=0;
            U1TXREG= bco;    
}

void Autos(void)
{
  Npaus++;
  if(Npaus > NP)
  {
   Npaus=0;
   uarf1.Fred= 0;  
   U1MODEbits.ABAUD = 1;	
  }
}

//receive data ISR
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt (void)
{
 Npaus=0;   
 if(!uarf1.Fred)
 {
     uarf1.Fred= 1;
     U1TXREG= 0xaa;//t
 }
 else
 {
  //if(!T4CONbits.TON)
  {
   uarf1.ErrPar=0;
  T4CONbits.TON = 1;
  // pbufin= &bufin[0]; //!t
  }
  TMR4 = 0;			  
  _T4IF=0;
  if(U1STAbits.OERR)U1STAbits.OERR=0;		// Enable UART Tx 
 // _U1RXIF=0;
  bci=(char)U1RXREG;
  *pbufin= bci;
  ni++;           //number of chars received 
  pbufin++;
 }
 _U1RXIF=0;//t
 return;
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt (void)
{
   _U1TXIF=0; 
   if((no > 0))
    {            //transmition continue
     no--;
     pbufout++;
     bco =*pbufout; 
     U1TXREG= bco;
   }
}

void __attribute__ ((interrupt, no_auto_psv)) _U1ErrInterrupt(void)
{
    	IFS4bits.U1EIF = 0; // Clear the UART1 Error Interrupt Flag 
        U1STAbits.OERR  = 0;     
        if(U1STAbits.PERR)
        uarf1.ErrPar=1;
}
