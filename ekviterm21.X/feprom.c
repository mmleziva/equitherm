#include <xc.h>
#include "feprom.h"



  _prog_addressT p,q,op; 
 int __attribute__((space(prog), aligned(_FLASH_PAGE*2))) dat[_FLASH_PAGE]={1,2,3,4,5,6,7,8};

int ie;


_prog_addressT InitFEPROM()  //Init FEPROM
{
    _init_prog_address(p,dat);   //t
   // p= p & 0x1ffff;
   // _erase_flash(p);//t
    return p;
       
}

void WriteFEP2W(int da, int ta)
{
    _erase_flash(p);
     _write_flash_word32(p, da, ta);    
}

/*
void WriteFEPROM(int *da, int *ta, int n)
{
    int i;
    _erase_flash(p);
    q=p;
    for(i=0; i < n; i++)
    {         
     _write_flash_word32(q, da[i], ta[i]);
     q +=4;
    } 
}
 */ 
void EraseFEPROM(void)
{
    q=p;
  _erase_flash(q);  
}

void WriteFEPROW(int *da)
{
    q=p; 
    _erase_flash(q);
   
       
    _write_flash16(q, da);
   
}

_prog_addressT ReadFEPROM(int *da, _prog_addressT q)  //Init FEPROM
{ 
   // _init_prog_address(p,dat);   //t
   // q=p;
    q= _memcpy_p2d16(da,q,2);
    return q;
    
}
       