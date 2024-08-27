#include <stddef.h>
#include <stdint.h>

static uint32_t MMIO_BASE;

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

void mmio_write(uint32_t reg, uint32_t data) {
  *(volatile uint32_t *)(MMIO_BASE + reg) = data;
}

uint32_t mmio_read(uint32_t reg) {
  return *(volatile uint32_t *)(MMIO_BASE + reg);
}
