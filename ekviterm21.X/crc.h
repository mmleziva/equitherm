  
#include <xc.h>
#include <stdint.h>

#define POLYLEN 0x000F  // Length of polynomial-1"
#define POLY    0b1000000000000101  // Generator Polynomial

    
    uint16_t    CRC_ChecksumWord( uint16_t *, uint16_t, uint16_t );
    uint16_t    CRC_ChecksumByte( uint8_t *, uint8_t, uint16_t );
    uint16_t    CRC( uint8_t *, uint8_t );
    uint16_t    CRC16( uint8_t *, uint8_t );
    //void      CRC_Init( void );

