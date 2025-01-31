/**
 * @file delay.c
 * @brief This file contains the implementation for delay functions.
 */
#include <stdint.h>

/**
 * @brief Delays the execution for a specified amount of time.
 *
 * @param count The number of iterations to delay. Higher values result in longer delays.
 */
void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
               : "=r"(count)
               : [count] "0"(count)
               : "cc");
}
