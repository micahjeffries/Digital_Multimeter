#include "msp.h"
#include "UART.h"

void uart_init(void) 
{
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST;             // put the eUSCI into sowftware reset
    EUSCI_A0->CTLW0 = (EUSCI_A_CTLW0_MODE_0
                    | EUSCI_A_CTLW0_SSEL__SMCLK
                    | EUSCI_A_CTLW0_SWRST);

    EUSCI_A0->BRW = 13;                                 // clock divider at 13
    EUSCI_A0->MCTLW |= EUSCI_A_MCTLW_OS16;              // enable oversampling
    EUSCI_A0->MCTLW |= ((0 << EUSCI_A_MCTLW_BRF_OFS)    // Set first modulation stage
                    & EUSCI_A_MCTLW_BRF_MASK);
    EUSCI_A0->MCTLW |= ((0x25 << EUSCI_A_MCTLW_BRS_OFS) // Set second modulation stage
                    & EUSCI_A_MCTLW_BRS_MASK);

    UART_PORT->SEL0 |= (UART_RXD | UART_TXD);           // configure UART pins
    UART_PORT->SEL1 &= ~(UART_RXD | UART_TXD);
    EUSCI_A0->CTLW0 &= ~(EUSCI_A_CTLW0_SWRST);          // clear software reset
}

void uart_write(uint8_t uart_data) {
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));        // wait for TXIFG to be set (TXBUF is empty)
        EUSCI_A0->TXBUF = uart_data;

    __delay_cycles(20);                                 // delay after transmission
}

void uart_print(uint8_t *string) {
    int i = 0;
    while (string[i] != '\0') {
        uart_write(string[i++]);
    }
}

void uart_esc_code(uint8_t *string) {
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));        // wait for TXIFG to be set (TXBUF is empty)
        EUSCI_A0->TXBUF = 0x1B;
        
    __delay_cycles(20);                                 // delay after transmission
    uart_print(string);
}