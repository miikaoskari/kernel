/**
 * @file uart.c
 * @brief UART driver implementation for BCM2711.
 *
 * This file contains the implementation of the UART driver for the BCM2711
 * peripheral. It provides the necessary functions to initialize and
 * communicate using the UART interface.
 * 
 * @note From https://wiki.osdev.org/Raspberry_Pi_Bare_Bones
 */
#include <stddef.h>
#include <stdint.h>

#include "delay/delay.h"
#include "mmio/mmio.h"

enum {
  // The offsets for reach register.
  GPIO_BASE = 0x200000,

  // Controls actuation of pull up/down to ALL GPIO pins.
  GPPUD = (GPIO_BASE + 0x94),

  // Controls actuation of pull up/down for specific GPIO pin.
  GPPUDCLK0 = (GPIO_BASE + 0x98),

  // The base address for UART.
  UART0_BASE = (GPIO_BASE + 0x1000), // for raspi4 0xFE201000, raspi2 & 3
                                     // 0x3F201000, and 0x20201000 for raspi1

  // The offsets for reach register for the UART.
  UART0_DR = (UART0_BASE + 0x00),
  UART0_RSRECR = (UART0_BASE + 0x04),
  UART0_FR = (UART0_BASE + 0x18),
  UART0_ILPR = (UART0_BASE + 0x20),
  UART0_IBRD = (UART0_BASE + 0x24),
  UART0_FBRD = (UART0_BASE + 0x28),
  UART0_LCRH = (UART0_BASE + 0x2C),
  UART0_CR = (UART0_BASE + 0x30),
  UART0_IFLS = (UART0_BASE + 0x34),
  UART0_IMSC = (UART0_BASE + 0x38),
  UART0_RIS = (UART0_BASE + 0x3C),
  UART0_MIS = (UART0_BASE + 0x40),
  UART0_ICR = (UART0_BASE + 0x44),
  UART0_DMACR = (UART0_BASE + 0x48),
  UART0_ITCR = (UART0_BASE + 0x80),
  UART0_ITIP = (UART0_BASE + 0x84),
  UART0_ITOP = (UART0_BASE + 0x88),
  UART0_TDR = (UART0_BASE + 0x8C),

  // The offsets for Mailbox registers
  MBOX_BASE = 0xB880,
  MBOX_READ = (MBOX_BASE + 0x00),
  MBOX_STATUS = (MBOX_BASE + 0x18),
  MBOX_WRITE = (MBOX_BASE + 0x20)
};

// A Mailbox message with set clock rate of PL011 to 3MHz tag
// starting from Raspberry Pi 3, the SoC is changed to BCM2837 and PL011 clock (UART0) is not fixed any more
volatile unsigned int __attribute__((aligned(16))) mbox[9] = {
    9 * 4, 0, 0x38002, 12, 8, 2, 3000000, 0, 0};

/**
 * @brief Initializes the UART peripheral for the specified Raspberry Pi model.
 *
 * This function sets up the UART peripheral for communication based on the
 * Raspberry Pi model specified by the parameter.
 *
 * @param raspi An integer representing the Raspberry Pi model.
 *              For example, 3 for Raspberry Pi 3, 4 for Raspberry Pi 4, etc.
 */
void uart_init(int raspi) {
  mmio_init(raspi);

  // Disable UART0.
  mmio_write(UART0_CR, 0x00000000);
  // Setup the GPIO pin 14 && 15.

  // Disable pull up/down for all GPIO pins & delay for 150 cycles.
  mmio_write(GPPUD, 0x00000000);
  delay(150);

  // Disable pull up/down for pin 14,15 & delay for 150 cycles.
  mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
  delay(150);

  // Write 0 to GPPUDCLK0 to make it take effect.
  mmio_write(GPPUDCLK0, 0x00000000);

  // Clear pending interrupts.
  mmio_write(UART0_ICR, 0x7FF);

  // Set integer & fractional part of baud rate.
  // Divider = UART_CLOCK/(16 * Baud)
  // Fraction part register = (Fractional part * 64) + 0.5
  // Baud = 115200.

  // For Raspi3 and 4 the UART_CLOCK is system-clock dependent by default.
  // Set it to 3Mhz so that we can consistently set the baud rate
  if (raspi >= 3) {
    // UART_CLOCK = 30000000;
    unsigned int r = (((unsigned int)(&mbox) & ~0xF) | 8);
    // wait until we can talk to the VC
    while (mmio_read(MBOX_STATUS) & 0x80000000) {
    }
    // send our message to property channel and wait for the response
    mmio_write(MBOX_WRITE, r);
    while ((mmio_read(MBOX_STATUS) & 0x40000000) || mmio_read(MBOX_READ) != r) {
    }
  }

  // Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
  mmio_write(UART0_IBRD, 1);
  // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
  mmio_write(UART0_FBRD, 40);

  // Enable FIFO & 8 bit data transmission (1 stop bit, no parity).
  mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

  // Mask all interrupts.
  mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) |
                             (1 << 8) | (1 << 9) | (1 << 10));

  // Enable UART0, receive & transfer part of UART.
  mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

/**
 * @brief Sends a character via UART.
 *
 * This function transmits a single character over the UART interface.
 *
 * @param c The character to be sent.
 */
void uart_putc(unsigned char c) {
  // Wait for UART to become ready to transmit.
  while (mmio_read(UART0_FR) & (1 << 5)) {
  }
  mmio_write(UART0_DR, c);
}

/**
 * @brief Receives a character from the UART.
 *
 * This function waits for a character to be received from the UART and returns it.
 *
 * @return The received character.
 */
unsigned char uart_getc() {
  // Wait for UART to have received something.
  while (mmio_read(UART0_FR) & (1 << 4)) {
  }
  return mmio_read(UART0_DR);
}

/**
 * @brief Sends a null-terminated string via UART.
 *
 * This function transmits each character in the provided string
 * until the null terminator is encountered.
 *
 * @param str The null-terminated string to be sent.
 */
void uart_puts(const char *str) {
  for (size_t i = 0; str[i] != '\0'; i++)
  {
    uart_putc((unsigned char)str[i]);
  }
}
