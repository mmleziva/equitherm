#include "encoder.h" 
#define Toa T[0]
#define Tcw T[1]
#define Trw T[2]
#define INKMAX 0xff
#define NEKDELAY 100
#define INTQ16  (0x10000) 
#define DELTEQ  (2*INTQ16)
#define DELTREV (5*INTQ16)
volatile fixed T[4],Tac;
bool INK1O,SWOL,CONT,INK1F,INK1FO,INK2F,INK2O;
bool CW,CCW, E, EKENA,EPW, TSCAN,RETE,REGRET;
volatile PARAMETERS par;
unsigned int j, stroke, nek;
int inkrem, a, minim;
 _Q16  delte,delterev,del;
 fixed Eqshift,Eqsteep;
_prog_addressT pq,qq;
int __attribute__((space(prog),aligned(_FLASH_PAGE*2))) dat[_FLASH_PAGE]={100,0,20,4,0,0,64};
//inline 
void initEncoder(void)
{
    LED1=LED2=1;
    CNPDBbits.CNPDB13=1;//pull down
    CNPDBbits.CNPDB12=1;//pull down
    CNPDAbits.CNPDA10=1;//pull down
    TMR4 = 0;
    PR4 = 0x800;//t 2048 ticks
    T4CONbits.TCKPS = 0;		//1   PRESCALLER 
    T4CONbits.TON = 1;		// turn on timer4 
    _T4IF=0;
    _T4IE=1;
    _init_prog_address(pq, dat);  /* get address in program space */
    qq= pq;
    qq= _memcpy_p2d16(par.A, qq, NVPAR*2);
    wristrLCD(3);
    k=0;    
    inkrem= par.A[k];
    REQ= true;
}

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt (void)//2048/(7370/4)kHz= 1,1ms
{
  _T4IF=0;
  if(INK1==INK1O)INK1F= INK1;
  INK1O=INK1;
  if(INK2==INK2O)INK2F= INK2;
  INK2O=INK2;
  if(INK1F & !INK1FO)
  {
      LED1= !LED1;//t
     if(INK2F)
        CCW= true;
     else
        CW= true; 
  }    
  else if(!SW && SWOL && (k<NVPAR))
  {
    CONT= !CONT; 
    if(CONT)
    {
     LED1= true;   
    }
    else
    {
        LED1= false;;  
        par.A[k]= inkrem;
    }
    EPW= true;
  }
  INK1FO=INK1F;
  SWOL=SW;
  if(CW || CCW)
  {
     REQ= true; 
     if(CW)
     {
      CW= false;
      if(CONT)
      {
         if(inkrem < INKMAX) inkrem++; 
      }   
      else
      {
       k++;
       if(k>= NPAR)k=0;
       inkrem= par.A[k];
      }
     }
     else if(CCW)
     {
      CCW= false;
      if(CONT)
      {
        if(k==1)        //eqshift
            minim= -0x80;        
        else minim=0;
         if(inkrem > minim) inkrem--; 
      }  
      else
      {
       k--;
       if(k<0) k=(NPAR-1);
       inkrem= par.A[k];
      } 
     }
     OC3RS= (inkrem<<8)+1;//t
  }
  else if(TSCAN)
  {
     TSCAN= false;
     if(k>=NVPAR)
     {
      inkrem= par.A[k];
      RETE= true;
     }
  }
  if(!PUMPIN)
  {
      EKENA= true;
      nek=0;
  }
  else
  {
   if(EKENA)
   {
    nek++;
    if(nek > NEKDELAY)
    {
     EKENA= false;
     nek=0;
    }
   }
  }
}

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt (void)//256*65500/((7370/4)kHz)= 8850ms 
{  
  _T5IF=0;
  //E=EKENA;      //t
  //Eqsteep.IF= (((_Q16)par.eqsteep)<<16)/100;//11;
  Eqsteep.IF= ((_Q16)par.eqsteep)*(INTQ16/100);//11;
  Eqshift.I= par.eqshift;
  delterev= INTQ16 * par.Trev - Trw.IF;
  //   delte= Tac.IF- Tcw.IF;
  if(!E)        //four-way valve set to heating water regulating
  {
   if(delterev <  DELTEQ) 
   {    
       E= true;
       integ=0;
       difla=0;
   }
  }
  else      //four-way valve set to return water
  {
      if(delterev >  DELTREV)
  // if(!EKENA)   
    {
       E= false;
      integ=0;
       difla=0;
   }
  }
  {    
   Tac.IF= equitherm(Eqsteep.IF, Toa.IF, Eqshift.IF);
    delte= Tac.IF- Tcw.IF;
   if(E)
       del= delte;
   else
       del = -delterev;
   //stroke= PID(delte,(par.P<<6),(par.I<<4), (par.D<<7), (par.Lim)<<8);
    stroke= PID(del,(par.P<<6),(par.I<<4), (par.D<<7), (par.Lim)<<8);
  }
  if(!EKENA)
  {         //close heating
   OC2R=1;
   OC1R= 0xffff;
  }    
  else if(!signum)
  {         //open heating
   OC1R=0;
   OC2R= stroke;
  }
  else
  {         //close heating
   OC2R=0;
   OC1R= stroke;
  }
}

/*auxial LEDs switch on*/
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt (void)//4*65500/((7370/4)kHz)= 136ms 
{
  _T3IF=0;
  switch(k)
  {
            case 0:Y1=1;    //eqsteep equitherm 
            break;
            case 1:Y2=1;    //eqshift equitherm 
            break;
            case 2:Y3=1;    //min. reverse temperature
            break;
            case 3:Y6=1;    //proportional reg. constant
            break;
            case 4:Y7=1;    //integral reg. constant
            break;            
            case 5: ;      //derivative reg. constant
            break;
            case 6: ;     //limited steep
            break;
            case 7: ;      //whole part of heating water temp
            break;
            case 8: ;       //whole part of reverse water temp
            break;
            case 9: ;     //whole part of outside ambient
            break;           
            default: break;
   }
  Toa.IF= Pt1000(fil[0].IF);//outside air temperature 
  Tcw.IF= Pt1000(fil[1].IF);//heating water temperature
  Trw.IF= Pt1000(fil[2].IF);//reverse water temperature
  par.Tcwi= Tcw.I;
  decimals[NVPAR]= (uint16_t)(((uint32_t)Tcw.F *10 )>>16);
  par.Trwi= Trw.I;
  decimals[NVPAR+1]= (uint16_t)(((uint32_t)Trw.F *10 )>>16);
  par.Toai= Toa.I;
  decimals[NVPAR+2]= (uint16_t)(((uint32_t)Toa.F *10 )>>16);
  par.Taci=Tac.I;
  decimals[NVPAR+3]= (uint16_t)(((uint32_t)Tac.F *10 )>>16);
   TSCAN= true;
}

void __attribute__((interrupt, no_auto_psv)) _OC3Interrupt (void)
{
  _OC3IF=0;
  Y1=Y2=Y3=Y6=Y7=0;
}
