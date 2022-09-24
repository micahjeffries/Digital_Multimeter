#include "msp.h"
#include "DCO.h"
#include "ADC.h"
#include "UART.h"
#include <math.h>

int i = 0;
uint16_t array[10000];
uint32_t avg = 0;
uint64_t RMS = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    
    set_DCO(FREQ_24_MHZ);                           // set MCLK to 24 MHz
    ADC14_init();                                   // initialize the ADC
    uart_init();                                    // initialize the eUSCI peripheral
    __enable_irq();                                 // global interrupt enable

    NVIC->ISER[0] = (1 << ADC14_IRQn);              // enable ADC interrupt
    ADC14->IER0 |= (ADC14_IER0_IE0);                // enable ADC interrupt for memory location 0
    ADC14->CLRIFGR0 |= (ADC14_IFGR0_IFG0);          // clear interrupt flag for memory location 0
    ADC14->CLRIFGR0 &= ~(ADC14_IFGR0_IFG0);

    NVIC->ISER[0] = (1 << TA0_0_IRQn);              // enable CCTL0.CCIFG
    TA0CCTL0 |= (TIMER_A_CCTLN_CCIE);               // enable CCIFG interrupt
    TA0CCTL0 &= ~(TIMER_A_CTL_IFG);                 // clear interrupt flag

    TA0CCR0 = PERIOD;                               // configure TimerA to trigger interrupts every 100 us
    TIMER_A0->CTL = (TIMER_A_CTL_SSEL__SMCLK        // set timerA0 clock source to SMCLK
                    | TIMER_A_CTL_MC__UP);          // set timerA0 counting mode to up

    uart_esc_code("[1m\0");                         // turn on bold mode
    uart_esc_code("[32m\0");                        // change text color to green
    uart_esc_code("[2J\0");                         // clear the screen

    ADC14->CTL0 |= (ADC14_CTL0_SC);                 // start the conversion

    while(1);
}

void displayFrequency(uint32_t frequency) 
{
    uint8_t digit_1 = 0, digit_2 = 0, digit_3 = 0, digit_4 = 0;
    uint8_t string[7] = {'0', '0', '0', '0', 'H', 'z', '\0'};

    digit_1 = frequency/1000;                       // calculate digits from frequency value to be sent to serial terminal
    digit_2 = (frequency%1000)/100;
    digit_3 = ((frequency%1000)%100)/10;
    digit_4 = ((frequency%1000)%100)%10;

    string[0] = digit_1 + '0';                      // modify the initiated string
    string[1] = digit_2 + '0';
    string[2] = digit_3 + '0';
    string[3] = digit_4 + '0';

    uart_esc_code("[1B\0");                         // move cursor down 1
    uart_esc_code("[20D\0");                        // move cursor left 20
    uart_print("Frequency: \0");                    // print frequency to the terminal
    uart_print(string);
    uart_esc_code("[2B\0");                         // move cursor down 2
    uart_esc_code("[17D\0");                        // move cursor left 17
}

void displayDCvoltage(uint32_t avg) 
{
    int j = 0;
    uint8_t digit_1 = 0, digit_2 = 0, digit_3 = 0, digit_4 = 0;
    uint8_t string[7] = {'0', '.', '0', '0', '0', 'V', '\0'};

    digit_1 = avg/1000;                             // calculate digits from DC voltage value to be sent to serial terminal
    digit_2 = (avg%1000)/100;
    digit_3 = ((avg%1000)%100)/10;
    digit_4 = ((avg%1000)%100)%10;

    string[0] = digit_1 + '0';                      // modify the initiated string
    string[2] = digit_2 + '0';
    string[3] = digit_3 + '0';
    string[4] = digit_4 + '0';

    uart_esc_code("[H\0");                          // move cursor to top left
    uart_esc_code("[13B\0");                        // move cursor down 13
    uart_esc_code("[36m\0");                        // change text color to light blue

    uart_print("________________________________________________________________________________\0");

    uart_esc_code("[32m\0");                        // change text color to green
    uart_esc_code("[80D\0");                        // move cursor left 80
    uart_esc_code("[2B\0");                         // move cursor down 2
    uart_print("DC Measurements: \0");

    uart_esc_code("[17D\0");                        // move cursor left 17
    uart_esc_code("[2B\0");                         // move cursor down 2
    uart_print("DC Voltage Bar Graph (V): \0");

    uart_esc_code("[26D\0");                        // move cursor left 26
    uart_esc_code("[1B\0");                         // move cursor down 1
    uart_print("|\0");
    uart_esc_code("[1D\0");                         // move cursor left 1
    uart_esc_code("[1B\0");                         // move cursor down 1
    uart_print("|\0");
    uart_esc_code("[1D\0");                         // move cursor left 1
    uart_esc_code("[1B\0");                         // move cursor down 1
    uart_print("|\0");
    uart_esc_code("[1D\0");                         // move cursor left 1
    uart_esc_code("[1B\0");                         // move cursor down 1

    uart_print("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\0");

    uart_esc_code("[80D\0");                        // move cursor left 80
    uart_esc_code("[1B\0");                         // move cursor down 1
    uart_print("0 0.5 1 1.5 2 2.5 3 \0");

    uart_esc_code("[80D\0");                        // move cursor left 80
    uart_esc_code("[3A\0");                         // move cursor up 3
    uart_esc_code("[1C\0");                         // move cursor right 1
    uart_esc_code("[0K\0");                         // clear the line from the cursor to the right
    uart_esc_code("[36m\0");                        // change text color to light blue

    for (j = 0; j < (avg / 40 - 1); j++)            // display bar graph with length proportional to DC voltage
        uart_print("I\0");

    uart_esc_code("[32m\0");                        // change text color to green
    uart_esc_code("[H\0");                          // move cursor to top left
    uart_esc_code("[25B\0");                        // move cursor down 25
    uart_print("DC Voltage: \0");                   // print DC voltage to the terminal
    uart_print(string);
}

void displayPK2PK(uint32_t Pk2pk) 
{
    uint8_t digit_1 = 0, digit_2 = 0, digit_3 = 0, digit_4 = 0;
    uint8_t string[7] = {'0', '.', '0', '0', '0', 'V', '\0'};

    digit_1 = Pk2pk/1000;                           // calculate digits from peak-to-peak value to be sent to serial terminal
    digit_2 = (Pk2pk%1000)/100;
    digit_3 = ((Pk2pk%1000)%100)/10;
    digit_4 = ((Pk2pk%1000)%100)%10;

    string[0] = digit_1 + '0';                      // modify the initiated string
    string[2] = digit_2 + '0';
    string[3] = digit_3 + '0';
    string[4] = digit_4 + '0';

    uart_esc_code("[1B\0");                         // move cursor down 1
    uart_esc_code("[11D\0");                        // move cursor left 11
    uart_print("Peak-to-Peak: \0");                 // print peak-to-peak voltage to the terminal
    uart_print(string);
}

void displayRMS(uint64_t RMS) 
{
    int j = 0;
    uint8_t digit_1 = 0, digit_2 = 0, digit_3 = 0, digit_4 = 0;
    uint8_t string[7] = {'0', '.', '0', '0', '0', 'V', '\0'};

    digit_1 = RMS/1000;                             // calculate digits from RMS value to be sent to serial terminal
    digit_2 = (RMS%1000)/100;
    digit_3 = ((RMS%1000)%100)/10;
    digit_4 = ((RMS%1000)%100)%10;

    string[0] = digit_1 + '0';                      // modify the initiated string
    string[2] = digit_2 + '0';
    string[3] = digit_3 + '0';
    string[4] = digit_4 + '0';

    uart_esc_code("[H\0");                          // move cursor to top left
    uart_print("AC Measurements: \0");

    uart_esc_code("[H\0");                          // move cursor to top left
    uart_esc_code("[2B\0");                         // move cursor down 2
    uart_print("RMS Bar Graph (V): \0");

    uart_esc_code("[H\0");                          // move cursor to top left
    uart_esc_code("[3B\0");                         // move cursor down 3
    uart_print("|\0");
    uart_esc_code("[1D\0");                         // move cursor left 1
    uart_esc_code("[1B\0");                         // move cursor down 1
    uart_print("|\0");
    uart_esc_code("[1D\0");                         // move cursor left 1
    uart_esc_code("[1B\0");                         // move cursor down 1
    uart_print("|\0");
    uart_esc_code("[1D\0");                         // move cursor left 1
    uart_esc_code("[1B\0");                         // move cursor down 1

    uart_print("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\0");

    uart_esc_code("[80D\0");                        // move cursor left 80
    uart_esc_code("[1B\0");                         // move cursor down 1
    uart_print("0 0.5 1 1.5 2 2.5 3 \0");

    uart_esc_code("[H\0");                          // move cursor to top left
    uart_esc_code("[4B\0");                         // move cursor down 4
    uart_esc_code("[1C\0");                         // move cursor right 1
    uart_esc_code("[0K\0");                         // clear the screen from the cursor to the right
    uart_esc_code("[36m\0");                        // change the text color to light blue
    
    for (j = 0; j < (RMS / 40 - 1); j++)            // display bar graph with length proportional to RMS
        uart_print("I\0");

    uart_esc_code("[32m\0");                        // change the text color to green
    uart_esc_code("[H\0");                          // move cursor to top left
    uart_esc_code("[9B\0");                         // move cursor down 9
    uart_print("RMS: \0");                          // print RMS to the terminal
    uart_print(string);
}

void processResults(void) 
{
    int j = 0;
    uint32_t frequency = 0, min = 0xFFFFFFFF, max = 0;

    typedef enum {                                  // define voltage states
        POSITIVE,
        NEGATIVE
    } VSTATE;

    VSTATE Vstate = POSITIVE;                       // start in the positive state
    avg = avg / 10000;                              // calculate the DC offset
    RMS = sqrt(RMS / 10000);                        // calculate the RMS
    RMS = 0.2 * RMS - 20;                           // convert digital RMS to voltage RMS

    for (j = 0; j < 10000; j++) {                   // calculate the frequency (count how many times the voltage crosses the DC voltage in one sec)
        switch(Vstate) {
            case POSITIVE:
                if (array[j] < avg - 500) {
                    Vstate = NEGATIVE;
                }
                break;

            case NEGATIVE:
                if (array[j] > avg + 500) {
                    frequency += 1;
                    Vstate = POSITIVE;
                }
                break;
    
            default:
                Vstate = POSITIVE;
        }

        if (array[j] > max)                         // find the max value
            max = array[j];

        if (array[j] < min)                         // find the min value
            min = array[j];
    }

    avg = avg * 0.2 - 20;                           // convert digital avg to voltage avg
    max = max * 0.2 - 20;                           // convert digital max to voltage max
    min = min * 0.2 - 20;                           // convert digital min to voltage min

    displayRMS(RMS);                                // display the RMS on the terminal
    displayPK2PK(max - min);                        // display the peak-to-peak voltage on the terminal
    displayFrequency(frequency);                    // display the frequency on the terminal
    displayDCvoltage(avg);                          // display the DC voltage on the terminal

    avg = RMS = 0;                                  // reset avg and RMS
}

void TA0_0_IRQHandler (void) 
{
    TA0CCTL0 &= ~(TIMER_A_CTL_IFG);                 // clear interrupt flag
    ADC14->CTL0 |= (ADC14_CTL0_SC);                 // start the conversion
}

void ADC14_IRQHandler(void) 
{
    if (i == 10000) {                               // is the array ready for processing?
        i = 0;
        processResults();
    }

    array[i++] = ADC14->MEM[0];                     // store the converted value in the array for further processing
    avg += ADC14->MEM[0];                           // increment DC voltage for later processing
    RMS += ADC14->MEM[0] * ADC14->MEM[0];           // increment RMS for later processing
}