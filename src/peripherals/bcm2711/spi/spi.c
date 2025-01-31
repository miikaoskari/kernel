#include <stdint.h>
#include <stdbool.h>

#include "peripherals/bcm2711/bcm2711_lpa.h"
#include "peripherals/bcm2711/spi/spi.h"


/* 9.6.2. Interrupt
 * 1. Set INTR and INTD. These can be left set over multiple operations.
 * 2. Set CS, CPOL, CPHA as required and set TA = 1. This will immediately trigger a first interrupt with DONE = 1.
 * 3. On interrupt:
 * - If DONE is set and data to write (this means it is the first interrupt), write up to 64 bytes to SPI_FIFO. If DONE is
 * set and no more data, set TA = 0. Read trailing data from SPI_FIFO until RXD is 0.
 * - If RXR is set read 48 bytes data from SPI_FIFO and if more data to write, write up to 48 bytes to SPI_FIFO.
 */

static SPI0_Type spi0;

/*
 * Initialize SPI0 peripheral as interrupt driven
 */
void spi_init(void)
{
    /* Set CS, CPOL, CPHA as required */
    spi0.CS_b.CS = 0;
    spi0.CS_b.CPOL = 0;
    spi0.CS_b.CPHA = 0;
}

void spi_enable_interrupts(void) {
    spi0.CS_b.INTR = true;
    spi0.CS_b.INTD = true;
}

void spi_write(uint8_t *data, uint16_t len) {
    for (int i = 0; i < len; i++) {
        /* wait until tx fifo has space */
        while (!spi0.CS_b.TXD);

        /* write data to FIFO */
        spi0.FIFO = data[i];
    }
    /* set transfer active */
    spi0.CS_b.TA = true;

    /* wait for transfer to complete */
    while (!spi0.CS_b.DONE);

    /* clear status */
    spi0.CS_b.TA = false;
}

void spi_read(uint8_t *data, uint16_t len) {
    for (int i = 0; i < len; i++) {
        /* set transfer active */
        spi0.CS_b.TA = true;

        /* wait until rx fifo has data */
        while (!spi0.CS_b.RXD);

        /* read data from FIFO */
        data[i] = spi0.FIFO;
    }

    /* wait for transfer to complete */
    while (!spi0.CS_b.DONE);

    /* clear status */
    spi0.CS_b.TA = false;
}
