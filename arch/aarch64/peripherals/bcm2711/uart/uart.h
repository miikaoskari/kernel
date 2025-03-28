#ifndef UART_H
#define UART_H

#include <stddef.h>

void uart_init(int raspi);
void uart_putc(unsigned char c);
unsigned char uart_getc();
void uart_puts(const char* str);

#endif
