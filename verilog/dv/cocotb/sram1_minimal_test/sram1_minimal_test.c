#include <firmware_apis.h>

#define SRAM1_BASE 0x30010000

void main() {
    ManagmentGpio_inputEnable();
    ManagmentGpio_outputEnable();
    ManagmentGpio_write(0);

    // Signal firmware ready
    ManagmentGpio_write(1);
    ManagmentGpio_write(0);
    
    // Do ONE write to SRAM1
    volatile unsigned int *sram1 = (unsigned int *)SRAM1_BASE;
    sram1[0] = 0x12345678;
    
    // Do ONE read from SRAM1
    unsigned int value = sram1[0];
    
    // Check if it matches
    if (value == 0x12345678) {
        // Pass
        ManagmentGpio_write(2);
        ManagmentGpio_write(0);
    } else {
        // Fail
        ManagmentGpio_write(1);
        ManagmentGpio_write(0);
    }
    
    return;
}
