#include <firmware_apis.h>
#include <defs.h>

#define SRAM0_BASE 0x30000000
#define SRAM1_BASE 0x30010000
#define USER_SPACE_ADDR 0x30000000
#define SRAM_SIZE_WORDS 1024
#define BANK_SIZE 256

void main(void)
{
    ManagmentGpio_write(0);
    ManagmentGpio_outputEnable();
    
    uint32_t sram0_offset_base = (SRAM0_BASE - USER_SPACE_ADDR) >> 2;
    uint32_t sram1_offset_base = (SRAM1_BASE - USER_SPACE_ADDR) >> 2;
    
    uint32_t errors = 0;
    
    ManagmentGpio_write(1);
    
    for (uint32_t bank = 0; bank < 4; bank++) {
        for (uint32_t word_in_bank = 0; word_in_bank < BANK_SIZE; word_in_bank++) {
            uint32_t addr = (bank * BANK_SIZE) + word_in_bank;
            uint32_t test_data = (bank << 24) | (word_in_bank & 0xFF);
            
            uint32_t offset = sram0_offset_base + addr;
            USER_writeWord(test_data, offset);
        }
    }
    
    for (uint32_t bank = 0; bank < 4; bank++) {
        for (uint32_t word_in_bank = 0; word_in_bank < BANK_SIZE; word_in_bank++) {
            uint32_t addr = (bank * BANK_SIZE) + word_in_bank;
            uint32_t expected = (bank << 24) | (word_in_bank & 0xFF);
            
            uint32_t offset = sram0_offset_base + addr;
            uint32_t read_data = USER_readWord(offset);
            
            if (read_data != expected) {
                errors++;
            }
        }
    }
    
    for (uint32_t word_in_bank = 0; word_in_bank < BANK_SIZE; word_in_bank++) {
        for (uint32_t bank = 0; bank < 4; bank++) {
            uint32_t addr = (bank * BANK_SIZE) + word_in_bank;
            uint32_t test_data = 0xF0000000 | (word_in_bank << 16) | (bank << 8) | word_in_bank;
            
            uint32_t offset = sram0_offset_base + addr;
            USER_writeWord(test_data, offset);
        }
    }
    
    for (uint32_t word_in_bank = 0; word_in_bank < BANK_SIZE; word_in_bank++) {
        for (uint32_t bank = 0; bank < 4; bank++) {
            uint32_t addr = (bank * BANK_SIZE) + word_in_bank;
            uint32_t expected = 0xF0000000 | (word_in_bank << 16) | (bank << 8) | word_in_bank;
            
            uint32_t offset = sram0_offset_base + addr;
            uint32_t read_data = USER_readWord(offset);
            
            if (read_data != expected) {
                errors++;
            }
        }
    }
    
    for (uint32_t bank = 0; bank < 4; bank++) {
        for (uint32_t word_in_bank = 0; word_in_bank < BANK_SIZE; word_in_bank++) {
            uint32_t addr = (bank * BANK_SIZE) + word_in_bank;
            uint32_t test_data = 0xAA000000 | (bank << 16) | word_in_bank;
            
            uint32_t offset = sram1_offset_base + addr;
            USER_writeWord(test_data, offset);
        }
    }
    
    for (uint32_t bank = 0; bank < 4; bank++) {
        for (uint32_t word_in_bank = 0; word_in_bank < BANK_SIZE; word_in_bank++) {
            uint32_t addr = (bank * BANK_SIZE) + word_in_bank;
            uint32_t expected = 0xAA000000 | (bank << 16) | word_in_bank;
            
            uint32_t offset = sram1_offset_base + addr;
            uint32_t read_data = USER_readWord(offset);
            
            if (read_data != expected) {
                errors++;
            }
        }
    }
    
    for (uint32_t word_in_bank = 0; word_in_bank < 128; word_in_bank++) {
        uint32_t test_data_sram0 = 0x11110000 | word_in_bank;
        uint32_t test_data_sram1 = 0x22220000 | word_in_bank;
        
        for (uint32_t bank = 0; bank < 4; bank++) {
            uint32_t addr = (bank * BANK_SIZE) + word_in_bank;
            
            uint32_t offset0 = sram0_offset_base + addr;
            USER_writeWord(test_data_sram0, offset0);
            
            uint32_t offset1 = sram1_offset_base + addr;
            USER_writeWord(test_data_sram1, offset1);
        }
    }
    
    for (uint32_t word_in_bank = 0; word_in_bank < 128; word_in_bank++) {
        uint32_t expected_sram0 = 0x11110000 | word_in_bank;
        uint32_t expected_sram1 = 0x22220000 | word_in_bank;
        
        for (uint32_t bank = 0; bank < 4; bank++) {
            uint32_t addr = (bank * BANK_SIZE) + word_in_bank;
            
            uint32_t offset0 = sram0_offset_base + addr;
            uint32_t read_data0 = USER_readWord(offset0);
            if (read_data0 != expected_sram0) {
                errors++;
            }
            
            uint32_t offset1 = sram1_offset_base + addr;
            uint32_t read_data1 = USER_readWord(offset1);
            if (read_data1 != expected_sram1) {
                errors++;
            }
        }
    }
    
    for (uint32_t bank = 0; bank < 4; bank++) {
        for (uint32_t word_in_bank = 0; word_in_bank < 16; word_in_bank++) {
            uint32_t addr = (bank * BANK_SIZE) + word_in_bank;
            
            uint32_t offset0 = sram0_offset_base + addr;
            uint32_t offset1 = sram1_offset_base + addr;
            
            USER_writeWord(0xDEADBEEF, offset0);
            USER_writeWord(0xCAFEBABE, offset1);
            
            uint32_t read0 = USER_readWord(offset0);
            uint32_t read1 = USER_readWord(offset1);
            
            if (read0 != 0xDEADBEEF || read1 != 0xCAFEBABE) {
                errors++;
            }
        }
    }
    
    ManagmentGpio_write(0);
    
    if (errors == 0) {
        ManagmentGpio_write(1);
        ManagmentGpio_write(0);
    }
    
    return;
}
