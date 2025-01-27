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

/*
 * Initialize SPI0 peripheral as interrupt driven
 */
void spi_init(void)
{
    SPI0_Type spi0;

    /* Set INTR and INTD */
    spi0.CS_b.INTR = true;
    spi0.CS_b.INTD = true;

    /* Set CS, CPOL, CPHA as required */
    spi0.CS_b.CS = 0;
    spi0.CS_b.CPOL = 0;
    spi0.CS_b.CPHA = 0;

    /* Set TA */
    spi0.CS_b.TA = true;
}
