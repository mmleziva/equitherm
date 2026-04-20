#include "encoder.h" 
#define Toa T[0]
#define Tcw T[1]
#define Trw T[2]
#define INKMAX 0xff
#define NEKDELAY 100
#define INTQ16  (0x10000) 
#define DELTEQ  (2*INTQ16)
#define DELTREV (4*INTQ16)
#define DELTREVSUP (10*INTQ16)
#define OPENTIME 30000  //ms
#define FULOPTIME 20000  //ms
#define CLOSETIME -5000    //ms
//#define OPENBITS (OPENTIME / 0.1392)
//#define CLOSEBITS (CLOSETIME / 0.1392)
static const long OPENBITS=  ((long)(((int64_t)OPENTIME * 10000) / 1392));
static const long FULOPBITS=  ((long)(((int64_t)FULOPTIME * 10000) / 1392));
static const long CLOSEBITS=  ((long)(((int64_t)CLOSETIME * 10000) / 1392));
volatile fixed T[4],Tac;
long int opentime;
bool INK1O,SWOL,CONT,INK1F,INK1FO,INK2F,INK2O;
bool CW,CCW, E, EKENA,EPW, TSCAN,RETE,REGRET, LARGEDIF ,SYNCLED;
volatile PARAMETERS par;
unsigned int j, stroke, nek;
int volatile inkrem, a, minim;
 _Q16  delte,delterev,del;
 fixed Eqshift,Eqsteep;
_prog_addressT pq,qq;
int __attribute__((space(prog),aligned(_FLASH_PAGE*2))) dat[_FLASH_PAGE]={100,0,20,4,0,0,64};
//inline 
void initEncoder(void)
{
  //  LED1=LED2=1;
    CNPDBbits.CNPDB13=1;//pull down
    CNPDBbits.CNPDB12=1;//pull down
    CNPDAbits.CNPDA10=1;//pull down
    TMR4 = 0;
    PR4 = 0x800;//t 2048 ticks
    T4CONbits.TCKPS = 0;		//1   PRESCALLER 
    T4CONbits.TON = 1;		// turn on timer4 
    _T4IF=0;
    _T4IE=1;
    _T4IP=5;    //priority
    _init_prog_address(pq, dat);  /* get address in program space */
    qq= pq;
    qq= _memcpy_p2d16(par.A, qq, NVPAR*2);
    wristrLCD(3);//t
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
    }
    else
    {
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
  {         //if a new measurement was scanned
     TSCAN= false;
     if(k>=NVPAR)
     {      //if the temperature is displayed
      inkrem= par.A[k];
      RETE= true;
     }
  }
  if(!PUMPIN)
  {     //if PUMPing servo enable 
      EKENA= true;
      nek=0;
  }
  else
  {
   if(EKENA)
   {
    nek++;
    if(nek > NEKDELAY)
    {       //If it is not pumped, the servo is deactivated with a delay.
     EKENA= false;
     nek=0;
    }
   }
  }
  if(!EKENA & LED2 & SYNCLED)
  {
      if(opentime <=  CLOSEBITS)  //allready full closed
          LED2= false;
      else 
         opentime -= 8;
  }
       
}

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt (void)//256*65500/((7370/4)kHz)= 8850ms 
{  
  _T5IF=0;
  Eqsteep.IF= ((_Q16)par.eqsteep)*(INTQ16/100);//11;
  Eqshift.I= par.eqshift;
  delterev= INTQ16 * par.Trev - Trw.IF; //desired and actual temperature diference
                 //four-way valve set to heating water regulating
  if(!E)   
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
   {    
       Y1=0;
       del= delte;
   }
   else
   {
       Y1=1;
       del = -delterev;
   }
   //stroke= PID(delte,(par.P<<6),(par.I<<4), (par.D<<7), (par.Lim)<<8);
    stroke= PID(del,(par.P<<6),(par.I<<4), (par.D<<7), (par.Lim)<<8);
  }
 // if(!EKENA || LARGEDIF)
  if(!EKENA)
  {         //close heating
    SYNCLED= true;  
    LED1= true;
    shut_servo(true);  
    integ=0;
    difla=0;
  }    
  else if(!signum)
  {         //open heating
    SYNCLED= false;  
    if(!LED2)//if servo is closed
    {
         LED2= true;// no shine, servo is not closed
         opentime=0; //zero position
    }
    else
    if(LED1) // if servo is not full open
    {
        if (opentime >= OPENBITS)//allready full open
        {
            shut_servo(false); //switch servo off
            LED1= false;        //LED1 shine,servo is open
        }
        else  
        {
            OC2_active(stroke+4); //opening
            opentime+= stroke;//t= 256  / ((7370/4)kHz) * stroke = (1024/7370)ms* stroke = 0,1392 ms * stroke
        }
     }
  }
  else      //servo closing
  {
     SYNCLED= false;
     if(!LED1)//if servo is open
    {
         LED1= true;    //no shine, servo is not open
         opentime= FULOPBITS;//FULL OPEN POSITION
    }
    else
    if(LED2) //servo is not full closed
    {
      //close heating
        if(opentime <=  CLOSEBITS)  //allready full closed
        {
            shut_servo(false);  //switch servo off
            LED2= false;        //LED2 shine, servo is closed
        }
        else
        {
            OC1_active(stroke+4);//closing
            opentime -= stroke; 
           }
       }
  }
}

/*auxial LEDs switch on*/
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt (void)//4*65500/((7370/4)kHz)= 136ms 
{
  _T3IF=0;
  /*
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
   */ 
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
   Y2 = (par.Tcwi > 120) | (par.Tcwi < -10) | (par.Trwi > 120) | (par.Trwi < -10) | (par.Toai > 70) | (par.Toai < -50) ;//temp fault
   Y3 = !Y3;            //blik
}

/*void __attribute__((interrupt, no_auto_psv)) _OC3Interrupt (void)
{
  _OC3IF=0;
  Y1=Y2=Y3=Y6=Y7=0;
}
*/