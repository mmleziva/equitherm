#include "timers.h"

void GoTimer (TIMER *pt)        //1 step of timer
{
    if(pt->Rem > 0){
        
        pt->Rem--;       
        if((pt->Rem==0))
        {   pt->GO =0;
            pt->STOP =1;         
        }
        else pt->STOP=0;
    }
    else if(pt->GO)
    {   pt->GO =0;
        pt->STOP =1;         
    }
}

void StartTimer (TIMER *pt)// preset and start timer
{  
     pt->Rem= *(pt->pSet);
     if(pt->Rem > 0)
     {
      pt->STOP=0;
      pt->GO=1;
     }
     else
     {
      pt->STOP=1;
      pt->GO=0;  
     }   
}

void ResetTimer(TIMER *pt)
{   pt->GO=0;
    pt->STOP=0;
    pt->Rem= 0;
}
        
