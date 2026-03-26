
#include <p33EV128GM104.h>

inline void setU1pins(void)
{
    RPINR18bits.U1RXR = 20; // U1RX on RP20
    RPOR1bits.RP36R = 0x01; // U1TX on RP36
}
