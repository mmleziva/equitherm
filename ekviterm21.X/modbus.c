#include "modbus.h"

unsigned int ADR0COILS;    
unsigned int ADR0DISCIN; 
unsigned int ADR0INPREG; 
unsigned int ADR0HOLDREG; 

UARTFLAGS mbu;//*puf;
int N;
int numrec;
SHORT aux;
//unsigned int recrc,trcrc; 
int shaddr;
unsigned char *pbi, *pbo, *pbufo;
void initT4(void);
 int Regs;

WORD HOLDREGSR[NREGS], HOLDREGSW[NREGS],INPREGS[INREGS];


MODBUSTRUCT mbm,mbs;  //modbus structures

void cfgModbus(void)
{
    initT4();
        #ifdef MBUART2
    cfgUart2();  
        #endif
    HOLDREGSW[0].W=0x1234;//t
    HOLDREGSW[1].W=0x5678;
    HOLDREGSW[2].W=0x9abc;
    HOLDREGSW[3].W=0xdef0;
}

inline void initT4(void)   //modbus pause among packets
{
 TMR4 = 0;
 PR4 = TIMPAUSE;			
 T4CONbits.TCKPS = 1;		//8   PRESCALLER 
 _T4IF=0;
 _T4IE=1;
} 

void __attribute__ ((interrupt, no_auto_psv)) _T4Interrupt(void)
{
    _T4IF=0; 
   //  T4CONbits.TON = 0;//!t  
    #ifdef MBUART1 
    if(ni>0)
    {
     mbu.ErrPar= uarf1.ErrPar; 
     mbu.Rec= 1;
     mbu.Uart1=1;
       // *puf= uarf1;
       // puf-> Rec=1;
       // puf->Works=1;
     numrec= ni;
     pbi= &bufin[0]; 
     pbufin= &bufin[0];//t 
     pbufo= &bufout[0];
     ni=0;
    }
    #endif
    #ifdef MBUART2 
    if(ni2>0)
    {
     mbu.ErrPar= uarf2.ErrPar; 
     mbu.Rec= 1;
     mbu.Uart2=1;
    // *puf= uarf2;
     //puf-> Rec=1;
     //puf->Works=1;
     numrec= ni2-1;
     pbi= &bufin2[0]; 
     pbufo= &bufout2[0];
     ni2=0;
    }
    #endif
    else
    {
      Autos();        
    }
}

unsigned char * mbrcopy(unsigned char *p, WORD *q, int n )//read n words from char array pointered by p to word array pointered by q 
{
    int i=n;
    while(i> 0)
    {
     q->H = *p;
     p++;
     q->L = *p;
     p++;
     q++;
     i--;
    }
    return p;
}

unsigned char * mbwcopy(WORD *q, unsigned char *p,  int n )//write n words from word array pointered by q to char array pointered by p 
{
    int i=n;
    while(i> 0)
    {
     *p= q->H;
     p++;
     *p= q->L;
     p++;
     q++;
     i--;
    }
    N += n;
    N += n;
    return p;
}

void mbsr() //modbus slave read
{
  mbs.errcode=0;
  //mbu.ErrPar =0;
  mbs.CRC= CRC16( pbi, numrec );
 // if((mbs.CRC !=0)|| mbu.ErrPar)
  if((mbs.CRC !=0))//t
      mbs.errcode= 3;   //ILLEGAL DATA
  mbs.unitId= *pbi;
  pbi++;
  mbs.funCode= *pbi;
  pbi++;
  if(mbs.errcode == 0)
  {
   switch(mbs.funCode) 
   {
      case 0x03:                                 
         mbs.startAdr.H= *pbi; 
         pbi++;
         mbs.startAdr.L= *pbi;
         pbi++;
         mbs.regCount.H= *pbi;
         pbi++;
         mbs.regCount.L= *pbi;     
         pbi++;
      break;
      case 0x04:                                 
         mbs.startAdr.H= *pbi; 
         pbi++;
         mbs.startAdr.L= *pbi;
         pbi++;
         mbs.regCount.H= *pbi;
         pbi++;
         mbs.regCount.L= *pbi;     
         pbi++;
      break;
      case 0x10:
         mbs.startAdr.H= *pbi; 
         pbi++;
         mbs.startAdr.L= *pbi;
         pbi++;
         mbs.regCount.H= *pbi;
         pbi++;
         mbs.regCount.L= *pbi;  
         pbi++;
         mbs.byteCount= *pbi;
         pbi++;
         shaddr= (mbs.startAdr.W - ADR0HOLDREG);
         if((shaddr < 0)||((shaddr + mbs.regCount.W)>= NREGS))
                 mbs.errcode= 2;
         else
         mbrcopy(pbi, &HOLDREGSR[shaddr],mbs.regCount.W);      
      break;
      default:
          mbs.errcode=1;
   }       
  }
  
}

//modbus master read
void mbmr()
{
  mbm.errcode=0;
  mbm.CRC= CRC16( pbi, numrec );
  if((mbm.CRC !=0) || mbu.ErrPar )
      mbm.errcode=3; //ILLEGAL DATA
  mbm.unitId= *pbi;
  pbi++;
  mbm.funCode= *pbi;
  pbi++;
  switch(mbm.funCode) 
  {
      case 0x03:                
        mbm.byteCount=  *pbi; 
        pbi++;
        shaddr= (mbs.startAdr.W - ADR0HOLDREG);
        pbi= mbrcopy(pbi, &HOLDREGSR[shaddr],mbs.regCount.W);          
      break;
       case 0x04:                
        mbm.byteCount=  *pbi; 
        pbi++;
        shaddr= (mbs.startAdr.W - ADR0INPREG);
        pbi= mbrcopy(pbi, &INPREGS[shaddr],mbs.regCount.W);          
      break;
      case 0x10:
         mbm.startAdr.H= *pbi; 
         pbi++;
         mbm.startAdr.L= *pbi;
         pbi++;
         mbm.regCount.H= *pbi;
         pbi++;
         mbm.regCount.L= *pbi;    
       default:
         mbm.errcode=1;
      
      break;
  }
  
}
//modbus master write

void mbmw(char UnitAddr, char Func, int Faddr, int Regs)
{
  pbo=pbufo;
  N=0;
  mbm.errcode=0;
  mbm.unitId=UnitAddr;
  mbm.funCode= Func;
  *pbo= mbm.unitId;
  pbo++; N++;
  *pbo=mbm.funCode; 
  pbo++; N++;
  switch(mbm.funCode) 
  {
      case 0x03:            
         mbm.startAdr.W= Faddr;
         mbm.regCount.W = Regs;         
         *pbo=mbm.startAdr.H; 
         pbo++;N++;
         *pbo=mbm.startAdr.L;
         pbo++;N++;
         *pbo=mbm.regCount.H;
         pbo++;N++;
         *pbo=mbm.regCount.L; 
         pbo++;N++;     
      break;
        case 0x04:            
         mbm.startAdr.W= Faddr;
         mbm.regCount.W = Regs;         
         *pbo=mbm.startAdr.H; 
         pbo++;N++;
         *pbo=mbm.startAdr.L;
         pbo++;N++;
         *pbo=mbm.regCount.H;
         pbo++;N++;
         *pbo=mbm.regCount.L; 
         pbo++;N++;     
      break;
      case 0x10:
      {
         mbm.startAdr.W= Faddr;
         mbm.regCount.W = Regs;         
         *pbo=mbm.startAdr.H; 
         pbo++;N++;
         *pbo=mbm.startAdr.L;
         pbo++;N++;
         *pbo=mbm.regCount.H;
         pbo++;N++;
         *pbo=mbm.regCount.L;
         pbo++;N++;         
         mbm.byteCount = Regs <<1 ;
         *pbo=mbm.byteCount;
         pbo++; N++;
         shaddr= (mbm.startAdr.W - ADR0HOLDREG);
         int Regs= mbm.regCount.W;
         
         //pbo= mbwcopy(&HOLDREGSW[shaddr],pbo,mbm.regCount.W);
         pbo= mbwcopy(&HOLDREGSW[shaddr],pbo, Regs);
         N+= (Regs <<1);
         //pbo++; N++;
     //    N= 7+2*Regs;
        
      }
      break;
      default:
          mbm.errcode=1;
  }
  aux.UI= CRC16(pbufo, N);  
  *pbo=aux.UB[0];
  pbo++; N++;
  *pbo= aux.UB[1];
#ifdef MBUART1
  if(mbu.Uart1)
      startU1tr(N+1);
#endif
  #ifdef MBUART2
  if(mbu.Uart2)
     startU2tr(N+1);
#endif
    
}

//modbus slave write
void mbsw() //modbus slave write
{
  pbo=pbufo;
  N=0;
  *pbo=mbs.unitId;
  pbo++;N++;
   if(mbs.errcode != 0)
   {
       *pbo=mbs.funCode +0x80;
       pbo++; N++;
       *pbo=mbs.errcode; 
       pbo++; N++;
   }     
   else
   {
    *pbo=mbs.funCode; 
    pbo++; N++;
    switch(mbs.funCode) 
    {
      case 0x03:                     
        mbs.byteCount= mbs.regCount.W << 1;      
        *pbo=mbs.byteCount; 
        pbo++; N++;
        shaddr= (mbs.startAdr.W - ADR0HOLDREG);
         int Regs= mbs.regCount.W;
         pbo= mbwcopy(&HOLDREGSR[shaddr],pbo,Regs);      
      break;
      case 0x04:                     
        mbs.byteCount= mbs.regCount.W << 1;      
        *pbo=mbs.byteCount; 
        pbo++; N++;
        shaddr= (mbs.startAdr.W - ADR0INPREG);
        Regs= mbs.regCount.W;
         pbo= mbwcopy(&INPREGS[shaddr],pbo,Regs);      
      break;
      case 0x10:              
         *pbo=mbs.startAdr.H; 
         pbo++; N++;
         *pbo=mbs.startAdr.L;
         pbo++; N++;
         *pbo=mbs.regCount.H;
         pbo++; N++;
         *pbo=mbs.regCount.L;
         pbo++; N++;        
      break;
      default:
          mbs.errcode=1;
    }
  }
  aux.UI= CRC16(pbufo, N);
  *pbo=aux.UB[0];
  pbo++; N++;
  *pbo= aux.UB[1];
 #ifdef MBUART1
  if(mbu.Uart1)   
      startU1tr(N);
#endif
  #ifdef MBUART2
  if(mbu.Uart2)
    // startU2tr(N+1);
      startU2tr(N);
#endif
}


void modrec(void)
{
    if(mbu.Rec)
    {
        mbu.Rec=0;
      //  if(mbu.Master)
        {
            mbu.Master=0;
            mbsr();        
            mbsw();            
        }
    //    else 
    //        mbmr();
    }
}

void modtra(void)
{  
#ifdef MBUART2
    mbu= uarf2;
    pbufo=&bufout2[0];  
    mbu.Master=1;
    //puf-> Works=1;
    mbu.Uart2=1;
    //mbmw(5, 0x10,40000,4 );//test
    mbmw(6, 0x3,40001,4 );//test
            //aux.UI= CRC_ChecksumByte( &bufout2[0], N, 0 ); 
#endif
    
}
