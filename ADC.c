#include "msp.h"
#include "ADC.h"

void ADC14_init(void) 
{
    ADC14->CTL0 &= ~(ADC14_CTL0_ENC             // disable the enable and start options
                    | ADC14_CTL0_SC);

    ADC14->CTL0 |= (ADC14_CTL0_SSEL__SMCLK      // configure settings for ADC
                    | ADC14_CTL0_CONSEQ_0
                    | ADC14_CTL0_SHS_0
                    | ADC14_CTL0_SHP
                    | ADC14_CTL0_SHT0_0
                    | ADC14_CTL0_SHT1_0
                    | ADC14_CTL0_ON);

    ADC14->CTL1 |= (ADC14_CTL1_RES_3);          // 14 bit resolution
    ADC14->MCTL[0] |= (ADC14_MCTLN_INCH_0       // analog pin and voltage reference select
                    | ADC14_MCTLN_VRSEL_0);

    ANALOG_PORT->SEL0 |= (ANALOG_IN);           // configure the analog port
    ANALOG_PORT->SEL1 |= (ANALOG_IN);
    
    ADC14->CTL0 |= (ADC14_CTL0_ENC);            // enable the ADC
}