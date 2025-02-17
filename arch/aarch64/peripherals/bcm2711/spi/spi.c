/**
 * @file spi.c
 * @brief SPI driver implementation for BCM2711 peripherals.
 *
 * This file contains the implementation of the SPI driver for the BCM2711
 * peripherals. It provides functions to initialize and communicate with SPI
 * devices.
 */
#include <stdbool.h>
#include <stdint.h>

#include "peripherals/bcm2711/bcm2711_lpa.h"
#include "peripherals/bcm2711/spi/spi.h"

static SPI0_Type spi0;

/**
 * @brief Initializes the SPI peripheral on the BCM2711.
 *
 * This function sets up the SPI peripheral for communication by configuring
 * the necessary registers and settings. It should be called before any SPI
 * communication is attempted.
 */
void spi_init(void)
{
    /* Set CS, CPOL, CPHA as required */
    spi0.CS_b.CS = 0;
    spi0.CS_b.CPOL = 0;
    spi0.CS_b.CPHA = 0;
}

/**
 * @brief Enables SPI interrupts.
 *
 * This function configures and enables the SPI interrupts for the BCM2711 peripheral.
 * It ensures that the SPI module is ready to handle interrupt-driven communication.
 */
void spi_enable_interrupts(void)
{
    spi0.CS_b.INTR = true;
    spi0.CS_b.INTD = true;
}

/**
 * @brief Writes data to the SPI bus.
 *
 * This function sends a specified number of bytes over the SPI bus.
 *
 * @param data Pointer to the data buffer to be sent.
 * @param len Length of the data buffer in bytes.
 */
void spi_write(uint8_t* data, uint16_t len)
{
    for (int i = 0; i < len; i++) {
        /* wait until tx fifo has space */
        while (!spi0.CS_b.TXD)
            ;

        /* write data to FIFO */
        spi0.FIFO = data[i];
    }
    /* set transfer active */
    spi0.CS_b.TA = true;

    /* wait for transfer to complete */
    while (!spi0.CS_b.DONE)
        ;

    /* clear status */
    spi0.CS_b.TA = false;
}

/**
 * @brief Reads data from the SPI peripheral.
 *
 * This function reads a specified number of bytes from the SPI peripheral
 * into the provided data buffer.
 *
 * @param data Pointer to the buffer where the read data will be stored.
 * @param len The number of bytes to read from the SPI peripheral.
 */
void spi_read(uint8_t* data, uint16_t len)
{
    for (int i = 0; i < len; i++) {
        /* set transfer active */
        spi0.CS_b.TA = true;

        /* wait until rx fifo has data */
        while (!spi0.CS_b.RXD)
            ;

        /* read data from FIFO */
        data[i] = spi0.FIFO;
    }

    /* wait for transfer to complete */
    while (!spi0.CS_b.DONE)
        ;

    /* clear status */
    spi0.CS_b.TA = false;
}
