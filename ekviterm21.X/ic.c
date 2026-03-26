
#include "ic.h"
inline void setICpins(void)
{
    RPINR7bits.IC1R = 45; // IC1 on RP45
}

inline void initIC(void)
{
    setICpins();  
    CNPDBbits.CNPDB13=1;//pull down
    CNPDBbits.CNPDB12=1;//pull down
    CNPDAbits.CNPDA10=1;//pull down
    CNENBbits.CNIEB13=1;
    //CNPUBbits.CNPUB13=1;
    IC1CON1bits.ICM= 0;//capture rising  edge
    //_IC1IE=1;
    _CNIE=1;
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void)
{
 _CNIF=0;   
}
