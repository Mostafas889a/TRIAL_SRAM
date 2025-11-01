#include <firmware_apis.h>
#include <defs.h>

#define SRAM0_BASE 0x30000000
#define SRAM1_BASE 0x30010000
#define USER_SPACE_ADDR 0x30000000
#define SRAM_SIZE_WORDS 1024

void main(void)
{
    ManagmentGpio_write(0);
    ManagmentGpio_outputEnable();
    
    uint32_t sram0_offset_base = (SRAM0_BASE - USER_SPACE_ADDR) >> 2;
    uint32_t sram1_offset_base = (SRAM1_BASE - USER_SPACE_ADDR) >> 2;
    
    uint32_t test_data, read_data;
    uint32_t errors = 0;
    
    ManagmentGpio_write(1);
    
    uint32_t test_addresses[] = {
        0,
        1,
        2,
        3,
        4,
        5,
        127,
        128,
        129,
        255,
        256,
        257,
        383,
        384,
        511,
        512,
        513,
        639,
        640,
        767,
        768,
        895,
        896,
        1020,
        1021,
        1022,
        1023
    };
    
    uint32_t num_test_addresses = sizeof(test_addresses) / sizeof(test_addresses[0]);
    
    for (uint32_t i = 0; i < num_test_addresses; i++) {
        uint32_t addr = test_addresses[i];
        test_data = 0xA5A50000 | addr;
        
        uint32_t offset = sram0_offset_base + addr;
        USER_writeWord(test_data, offset);
    }
    
    for (uint32_t i = 0; i < num_test_addresses; i++) {
        uint32_t addr = test_addresses[i];
        uint32_t expected = 0xA5A50000 | addr;
        
        uint32_t offset = sram0_offset_base + addr;
        read_data = USER_readWord(offset);
        
        if (read_data != expected) {
            errors++;
        }
    }
    
    for (uint32_t i = 0; i < num_test_addresses; i++) {
        uint32_t addr = test_addresses[i];
        test_data = 0x5A5A0000 | addr;
        
        uint32_t offset = sram1_offset_base + addr;
        USER_writeWord(test_data, offset);
    }
    
    for (uint32_t i = 0; i < num_test_addresses; i++) {
        uint32_t addr = test_addresses[i];
        uint32_t expected = 0x5A5A0000 | addr;
        
        uint32_t offset = sram1_offset_base + addr;
        read_data = USER_readWord(offset);
        
        if (read_data != expected) {
            errors++;
        }
    }
    
    for (uint32_t addr = 0; addr < SRAM_SIZE_WORDS; addr++) {
        test_data = (addr * 0x12345678) ^ 0xAAAAAAAA;
        
        uint32_t offset = sram0_offset_base + addr;
        USER_writeWord(test_data, offset);
    }
    
    for (uint32_t addr = 0; addr < SRAM_SIZE_WORDS; addr++) {
        uint32_t expected = (addr * 0x12345678) ^ 0xAAAAAAAA;
        
        uint32_t offset = sram0_offset_base + addr;
        read_data = USER_readWord(offset);
        
        if (read_data != expected) {
            errors++;
        }
    }
    
    for (uint32_t addr = 0; addr < SRAM_SIZE_WORDS; addr++) {
        test_data = (addr * 0x87654321) ^ 0x55555555;
        
        uint32_t offset = sram1_offset_base + addr;
        USER_writeWord(test_data, offset);
    }
    
    for (uint32_t addr = 0; addr < SRAM_SIZE_WORDS; addr++) {
        uint32_t expected = (addr * 0x87654321) ^ 0x55555555;
        
        uint32_t offset = sram1_offset_base + addr;
        read_data = USER_readWord(offset);
        
        if (read_data != expected) {
            errors++;
        }
    }
    
    ManagmentGpio_write(0);
    
    if (errors == 0) {
        ManagmentGpio_write(1);
        ManagmentGpio_write(0);
    }
    
    return;
}
