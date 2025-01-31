#include <stdint.h>
#include <stdbool.h>

#include "peripherals/bcm2711/bcm2711_lpa.h"

/* The Broadcom Serial Control (BSC) controller is a master, fast-mode (400Kb/s) BSC controller. The Broadcom Serial
 * Control bus is a proprietary bus compliant with the Philips® I2C bus/interface version 2.1 January 2000.
 */

static BSC0_Type bsc0;

void i2c_init(void)
{
    /* enable i2c */
    bsc0.C_b.I2CEN = true;

    /* set clock divider, just use 150 MHz*/
    bsc0.DIV_b.CDIV = 150;

    /* set some timeout */
    bsc0.CLKT_b.TOUT = 0x3ff;
}

void i2c_enable_interrupts(void)
{
    /* enable interrupt based I2C */
    bsc0.C_b.INTR = true;
    bsc0.C_b.INTT = true;
}

void i2c_write(uint8_t *data, uint8_t slave_addr, uint16_t len)
{
    bsc0.DLEN = len;
    bsc0.A = slave_addr;

    /* write data to fifo */
    for (int i = 0; i < len; i++)
    {
        bsc0.FIFO = data[i];
    }

    /* start transfer */
    bsc0.C_b.ST = true;

    /* wait for transfer to complete */
    while (!bsc0.S_b.DONE) {}

    /* clear status */
    bsc0.S_b.DONE = true;

    return;
}

void i2c_read(uint8_t *data, uint8_t slave_addr, uint16_t len)
{
    bsc0.DLEN = len;
    bsc0.A = slave_addr;
    bsc0.C_b.ST = true;

    /* read data from fifo */
    for (int i = 0; i < len; i++)
    {
        data[i] = bsc0.FIFO;
    }

    /* wait for transfer to complete */
    while (!bsc0.S_b.DONE) {}

    /* clear status */
    bsc0.S_b.DONE = true;

    return;
}
