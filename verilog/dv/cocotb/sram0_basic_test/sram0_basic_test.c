#include <firmware_apis.h>

#define SRAM0_BASE 0x30000000
#define TEST_SIZE 256

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
        uint32_t test_data = 0xA5A5A5A5 + i;
        uint32_t addr = SRAM0_BASE + (i << 2);
        USER_writeWord(test_data, addr);
    }
    
    for (int i = 0; i < TEST_SIZE; i++) {
        uint32_t expected = 0xA5A5A5A5 + i;
        uint32_t addr = SRAM0_BASE + (i << 2);
        uint32_t read_val = USER_readWord(addr);
        if (read_val != expected) {
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
