#ifndef P_SPI_H
#define P_SPI_H

#include <stdint.h>

void spi_init(void);
void spi_enable_interrupts(void);
void spi_write(uint8_t* data, uint16_t len);
void spi_read(uint8_t* data, uint16_t len);

#endif
