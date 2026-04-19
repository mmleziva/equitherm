#ifndef USER_H
#define	USER_H
#include <xc.h>
#include <libq.h>
#include <stdbool.h>
#include <libpic30.h>
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#define Y1    _LATC3
#define Y2    _LATC4
#define Y3    _LATC5
#define Y4    _LATB7
#define Y5    _LATB8
#define Y6    _LATB9
#define Y7    _LATC6
#define DI1     _RC0
#define DI2     _RC1
#define DI3     _RC2
#define DI4     _RA8
#define DI5     _RA9

#define LED1 _LATC7
#define LED2 _LATC8

#define  PUMPIN DI2
#define SERVO_OPENING Y4
#define SERVO_CLOSING Y5


#define FCY (7370E3/4)
#define BAUDRATE 9600//115200
#define BRGVAL   (((FCY/BAUDRATE)/4)-1) 
#define BUFMAX 64
#define TIMBIT ((BRGVAL+1)*4) 
#define PACKETPAUSE 10  // number of bytes among pakets
#define TIMPAUSE ((((TIMBIT/4)*10)/2)*PACKETPAUSE)
#define NPAR 11//10
#define NVPAR 7
typedef  union 
{
 unsigned char UB[2];
 unsigned int  UI;
 signed char B[2];
 signed int  I;
}SHORT ;

typedef  union 
{
 unsigned int W;
 struct
 {
     unsigned char L;
     unsigned char H;
 };
}WORD;

typedef  union 
{
 unsigned int A[NPAR];
 struct{ 
  unsigned int eqsteep;   //equitherm 
  int eqshift;   //equitherm
  unsigned int Trev;   //min. reverse temperature
  unsigned int P;   //proportional reg. constant
  unsigned int I;   //integral reg. constant
  unsigned int D;   //derivative reg. constant
  unsigned int Lim;   //limited steep
  int Tcwi;         //whole part of heating water temp
  int Trwi;         //whole part of reverse water temp
  int Toai;         //whole part of outside ambient temp
  int Taci;         //whole part of equitherm temp
 };
}PARAMETERS;


typedef  union 
{
 unsigned char UB[4];
 unsigned long int UL;
 unsigned int  UI[2];
 unsigned char B[4];
 long int L;
 int  I[2];
 float F;
}FLUI;

typedef  union 
{
 _Q16 IF;
 struct
 {
  unsigned  int F;
  signed int  I;
 };
 signed char B[4];
}fixed;

typedef struct 
{
 unsigned Pre  :1; 
 unsigned Rec  :1; 
 unsigned ErrPar: 1; 
 unsigned Master: 1; 
 unsigned Uart1: 1; 
 unsigned Uart2: 1; 
 unsigned Fred:  1;
 
}UARTFLAGS;

typedef struct 
{
 unsigned SAVE  :1;
 unsigned QUIT  :1;  
}FLAGS;

typedef union 
{
  unsigned char c;   
}BYTE;

#endif	/* USER_H */