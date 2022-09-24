#include "msp.h"
#include "DCO.h"

void set_DCO(uint32_t frequency){
    CS->KEY = CS_KEY_VAL;                   // unlock CS registers

    switch (frequency) {
        case FREQ_15_MHZ:
            CS->CTL0 = (CS_CTL0_DCORSEL_0); // set DCO to 1.5 MHz
            break;

        case FREQ_3_MHZ:
            CS->CTL0 = (CS_CTL0_DCORSEL_1); // set DCO to 3 MHz
            break;
    
        case FREQ_6_MHZ:
            CS->CTL0 = (CS_CTL0_DCORSEL_2); // set DCO to 6 MHz
        break;
    
        case FREQ_12_MHZ:
            CS->CTL0 = (CS_CTL0_DCORSEL_3); // set DCO to 12 MHz
            break;
    
        case FREQ_24_MHZ:
            CS->CTL0 = (CS_CTL0_DCORSEL_4); // set DCO to 24 MHz
            break;
    
        default: break;
    }

    CS->CTL1 = (CS_CTL1_DIVM__1 |           // MCLK / 1
    CS_CTL1_SELS__DCOCLK |                  // SMCLK / HSMCLK using DCO
    CS_CTL1_SELM__DCOCLK);                  // MCLK using DCO
    CS->KEY = 0;                            // lock CS registers
}