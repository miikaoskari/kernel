/**
 * @file i2c.c
 * @brief I2C peripheral driver implementation for BCM2711.
 *
 * This file contains the implementation of the I2C peripheral driver
 * for the BCM2711 chip. It provides functions to initialize and
 * communicate with I2C devices.
 *
 * The Broadcom Serial Control (BSC) controller is a master, fast-mode (400Kb/s) BSC controller. The Broadcom Serial
 * Control bus is a proprietary bus compliant with the Philips® I2C bus/interface version 2.1 January 2000.
 */
#include <stdbool.h>
#include <stdint.h>

#include "peripherals/bcm2711/bcm2711_lpa.h"

static BSC0_Type bsc0;

/**
 * @brief Initializes the I2C peripheral for the BCM2711.
 *
 * This function sets up the necessary configurations and initializes the
 * I2C hardware to be ready for communication. It should be called before
 * any I2C transactions are performed.
 */
void i2c_init(void)
{
    /* enable i2c */
    bsc0.C_b.I2CEN = true;

    /* set clock divider, just use 150 MHz*/
    bsc0.DIV_b.CDIV = 150;

    /* set some timeout */
    bsc0.CLKT_b.TOUT = 0x3ff;
}

/**
 * @brief Enables the I2C interrupts.
 *
 * This function configures and enables the necessary interrupts for the I2C
 * peripheral on the BCM2711. It ensures that the I2C module can properly
 * handle interrupt-driven events.
 */
void i2c_enable_interrupts(void)
{
    /* enable interrupt based I2C */
    bsc0.C_b.INTR = true;
    bsc0.C_b.INTT = true;
}

/**
 * @brief Writes data to an I2C slave device.
 *
 * This function sends a sequence of bytes to a specified I2C slave device.
 *
 * @param data Pointer to the data buffer to be sent.
 * @param slave_addr The address of the I2C slave device.
 * @param len The length of the data buffer.
 */
void i2c_write(uint8_t* data, uint8_t slave_addr, uint16_t len)
{
    bsc0.DLEN = len;
    bsc0.A = slave_addr;

    /* write data to fifo */
    for (int i = 0; i < len; i++) {
        bsc0.FIFO = data[i];
    }

    /* start transfer */
    bsc0.C_b.ST = true;

    /* wait for transfer to complete */
    while (!bsc0.S_b.DONE) { }

    /* clear status */
    bsc0.S_b.DONE = true;

    return;
}

/**
 * @brief Reads data from an I2C slave device.
 *
 * This function reads a specified number of bytes from an I2C slave device
 * into the provided data buffer.
 *
 * @param data Pointer to the buffer where the read data will be stored.
 * @param slave_addr The address of the I2C slave device to read from.
 * @param len The number of bytes to read from the I2C slave device.
 */
void i2c_read(uint8_t* data, uint8_t slave_addr, uint16_t len)
{
    bsc0.DLEN = len;
    bsc0.A = slave_addr;
    bsc0.C_b.ST = true;

    /* read data from fifo */
    for (int i = 0; i < len; i++) {
        data[i] = bsc0.FIFO;
    }

    /* wait for transfer to complete */
    while (!bsc0.S_b.DONE) { }

    /* clear status */
    bsc0.S_b.DONE = true;

    return;
}
