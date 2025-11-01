#include <firmware_apis.h>

#define SRAM0_OFFSET 0
#define SRAM1_OFFSET 4096
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
        USER_writeWord(0xAAAAAAAA + i, SRAM0_OFFSET + i);
        USER_writeWord(0x55555555 + i, SRAM1_OFFSET + i);
    }
    
    for (int i = 0; i < TEST_SIZE; i++) {
        uint32_t sram0_val = USER_readWord(SRAM0_OFFSET + i);
        uint32_t sram1_val = USER_readWord(SRAM1_OFFSET + i);
        
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
