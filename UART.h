#ifndef UART_H_
#define UART_H_

#define UART_PORT P1

#define UART_RXD BIT2
#define UART_TXD BIT3

void uart_init(void);
void uart_write(uint8_t uart_data);
void uart_print(uint8_t *string);
void uart_esc_code(uint8_t *string);

#endif /* UART_H_ */