/**
 * @file mmio.c
 * @brief Memory-Mapped I/O (MMIO) implementation.
 *
 * This file contains the implementation for memory-mapped I/O operations.
 * It includes necessary headers and defines functions to interact with
 * hardware registers via MMIO.
 */
#include <stddef.h>
#include <stdint.h>

static uint32_t MMIO_BASE;

/**
 * @brief Initialize the MMIO base address based on the Raspberry Pi model.
 *
 * @param type The type of Raspberry Pi:
 * - 2 or 3: Sets MMIO_BASE to 0x3F000000
 * - 4: Sets MMIO_BASE to 0xFE000000
 * - Default: Sets MMIO_BASE to 0x20000000 (for Raspberry Pi 1, Raspberry Pi Zero, etc.)
 */
void mmio_init(int type) {
  switch (type) {
  case 2:
  case 3:
    MMIO_BASE = 0x3F000000;
    break; // for raspi2 & 3
  case 4:
    MMIO_BASE = 0xFE000000;
    break; // for raspi4
  default:
    MMIO_BASE = 0x20000000;
    break; // for raspi1, raspi zero etc.
  }
}

/**
 * @brief Write a 32-bit value to a MMIO register.
 *
 * @param reg The register offset from the MMIO base address.
 * @param data The 32-bit data to write to the register.
 */
void mmio_write(uint32_t reg, uint32_t data) {
  *(volatile uint32_t *)(MMIO_BASE + reg) = data;
}

/**
 * @brief Read a 32-bit value from a MMIO register.
 *
 * @param reg The register offset from the MMIO base address.
 * @return The 32-bit value read from the register.
 */
uint32_t mmio_read(uint32_t reg) {
  return *(volatile uint32_t *)(MMIO_BASE + reg);
}
