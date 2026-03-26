
#ifndef FEPROM_H
#define	FEPROM_H
//#include "libpic30.h"
#ifndef _FLASH_PAGE
#define _FLASH_PAGE   512
#endif

#ifndef _FLASH_ROW
#define _FLASH_ROW     64
#endif

#ifndef __dsPIC33E__
#define __dsPIC33E__
#endif

#define _FLASH_ERASE_CODE                 0x4003
#define _FLASH_WRITE_ROW_CODE             0x4002
#define _FLASH_WRITE_WORD_CODE            0x4001




typedef unsigned long _prog_addressT;
#define _init_prog_address(a,b)  (a = __builtin_tbladdress(&b))
void _erase_flash(_prog_addressT dst);
#define _erase_flash(dst)  _flash_helper1(dst, _FLASH_ERASE_CODE)
_prog_addressT _memcpy_p2d16(void *dest, _prog_addressT src,
                             unsigned int len);

#define _memcpy_p2d16(dest, src, len)  _memcpy_helper(src, dest, len, 0) 
void _write_flash16(_prog_addressT dst, int *src);
#define _write_flash16(dst, src)  _flash_helper2(dst, src, _FLASH_ROW)
void _write_flash_word32(_prog_addressT dst, int dat1, int dat2);
#define _write_flash_word32(dst, dat1, dat2) _flash_helper10(dst, dat1, 0, dat2)

extern void _flash_helper1 (_prog_addressT dst, int code);
extern void _flash_helper2 (_prog_addressT dst, int *src, int len);
extern void _flash_helper3 (_prog_addressT dst, long *src, int len);
extern void _flash_helper10 (_prog_addressT dst, int dat1, int empty, int dat2);
extern _prog_addressT _memcpy_helper(_prog_addressT src, void *dst,
                                     unsigned int len, int flags);



//typedef unsigned long _prog_addressT;//t
//void InitFEPROM(long int *da,long int *ta);  //Init FEPROM
//void WriteFEPROM(long int *da,long int *ta);  //Init FEPROM
//void InitFEPROM(int *da); //Init FEPROM
//void WriteFEPROM(int *da, int *ta, int n);  //write FEPROM
void WriteFEPROW(int *da);//write 1 row
void EraseFEPROM(void);
void WriteFEP2W(int da, int ta);
//void ReadFEPROM(int *da);  //Init FEPROM
_prog_addressT InitFEPROM();
_prog_addressT ReadFEPROM(int *da, _prog_addressT q);

#endif	/* FEPROM_H */
