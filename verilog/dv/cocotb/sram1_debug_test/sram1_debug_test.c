#include <firmware_apis.h>

#define SRAM1_BASE 0x30010000
#define USER_SPACE_ADDR 0x30000000

static void send_pulse(int count);

void main(void) {
    ManagmentGpio_outputEnable();
    ManagmentGpio_write(0);
    enableHkSpi(false);

    GPIOs_loadConfigs();
    User_enableIF();
    
    send_pulse(1);  // Signal firmware start
    
    // Calculate word offset for SRAM1
    // SRAM1_BASE = 0x30010000, USER_SPACE_ADDR = 0x30000000
    // Byte offset = 0x10000 (65536)
    // Word offset = 0x4000 (16384)
    
    const uint32_t word_offset = 0x4000;  // Hardcode for clarity
    
    send_pulse(1);  // Signal before write
    
    // Try ONE write
    USER_writeWord(0xABCD1234, word_offset);
    
    send_pulse(1);  // Signal after write
    
    // Try ONE read
    uint32_t value = USER_readWord(word_offset);
    
    send_pulse(1);  // Signal after read
    
    // Check result
    if (value == 0xABCD1234) {
        send_pulse(2);  // PASS
    } else {
        send_pulse(1);  // FAIL
    }
}

static void send_pulse(int count) {
    for (int i = 0; i < count; i++) {
        ManagmentGpio_write(1);
        ManagmentGpio_write(0);
    }
}
