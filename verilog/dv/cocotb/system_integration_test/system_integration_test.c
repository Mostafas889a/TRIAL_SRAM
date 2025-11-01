#include <firmware_apis.h>

#define SRAM0_BASE 0x30000000
#define SRAM1_BASE 0x30010000

static void send_pulse(int count);

void main(void) {
    ManagmentGpio_outputEnable();
    ManagmentGpio_write(0);
    enableHkSpi(false);

    GPIOs_loadConfigs();
    User_enableIF();
    
    send_pulse(1);
    
    int pass = 1;
    
    USER_writeWord(0x12345678, SRAM0_BASE + (0 << 2));
    USER_writeWord(0x9ABCDEF0, SRAM0_BASE + (10 << 2));
    USER_writeWord(0xDEADBEEF, SRAM0_BASE + (100 << 2));
    
    uint32_t val1 = USER_readWord(SRAM0_BASE + (0 << 2));
    uint32_t val2 = USER_readWord(SRAM0_BASE + (10 << 2));
    uint32_t val3 = USER_readWord(SRAM0_BASE + (100 << 2));
    
    if (val1 != 0x12345678 || val2 != 0x9ABCDEF0 || val3 != 0xDEADBEEF) {
        pass = 0;
    }
    
    send_pulse(1);
    
    USER_writeWord(0xCAFEBABE, SRAM1_BASE + (0 << 2));
    USER_writeWord(0xFEEDFACE, SRAM1_BASE + (20 << 2));
    USER_writeWord(0xC0FFEE00, SRAM1_BASE + (200 << 2));
    
    val1 = USER_readWord(SRAM1_BASE + (0 << 2));
    val2 = USER_readWord(SRAM1_BASE + (20 << 2));
    val3 = USER_readWord(SRAM1_BASE + (200 << 2));
    
    if (val1 != 0xCAFEBABE || val2 != 0xFEEDFACE || val3 != 0xC0FFEE00) {
        pass = 0;
    }
    
    send_pulse(1);
    
    val1 = USER_readWord(SRAM0_BASE + (0 << 2));
    val2 = USER_readWord(SRAM0_BASE + (10 << 2));
    if (val1 != 0x12345678 || val2 != 0x9ABCDEF0) {
        pass = 0;
    }
    
    send_pulse(1);
    
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
