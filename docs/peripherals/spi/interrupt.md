# BCM2711 SPI

## Interrupt
 1. Set INTR and INTD. These can be left set over multiple operations.
 2. Set CS, CPOL, CPHA as required and set TA = 1. This will immediately trigger a first interrupt with DONE = 1.
 3. On interrupt:
    - If DONE is set and data to write (this means it is the first interrupt), write up to 64 bytes to SPI_FIFO. If DONE is
 set and no more data, set TA = 0. Read trailing data from SPI_FIFO until RXD is 0.
    - If RXR is set read 48 bytes data from SPI_FIFO and if more data to write, write up to 48 bytes to SPI_FIFO.