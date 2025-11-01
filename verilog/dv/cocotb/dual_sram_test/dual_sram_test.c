#include <firmware_apis.h>

#define SRAM0_BASE 0x30000000
#define SRAM1_BASE 0x30010000
#define TEST_SIZE 128

static void send_pulse(int count);

void main(void) {
    ManagmentGpio_outputEnable();
    ManagmentGpio_write(0);
    enableHkSpi(false);

    GPIOs_loadConfigs();
    User_enableIF();
    
    send_pulse(1);
    
    int pass = 1;
    
    for (int i = 0; i < TEST_SIZE; i++) {
        uint32_t sram0_addr = SRAM0_BASE + (i << 2);
        uint32_t sram1_addr = SRAM1_BASE + (i << 2);
        USER_writeWord(0xAAAAAAAA + i, sram0_addr);
        USER_writeWord(0x55555555 + i, sram1_addr);
    }
    
    for (int i = 0; i < TEST_SIZE; i++) {
        uint32_t sram0_addr = SRAM0_BASE + (i << 2);
        uint32_t sram1_addr = SRAM1_BASE + (i << 2);
        uint32_t sram0_val = USER_readWord(sram0_addr);
        uint32_t sram1_val = USER_readWord(sram1_addr);
        
        if (sram0_val != (0xAAAAAAAA + i) || sram1_val != (0x55555555 + i)) {
            pass = 0;
            break;
        }
    }
    
    if (pass) {
        send_pulse(2);
    } else {
        send_pulse(1);
    }
}

static void send_pulse(int count) {
    for (int i = 0; i < count; i++) {
        ManagmentGpio_write(1);
        ManagmentGpio_write(0);
    }
}
