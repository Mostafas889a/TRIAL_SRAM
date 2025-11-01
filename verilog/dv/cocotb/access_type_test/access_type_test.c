#include <firmware_apis.h>
#include <defs.h>

#define SRAM0_BASE 0x30000000
#define SRAM1_BASE 0x30010000
#define USER_SPACE_ADDR 0x30000000

#define USER_writeByte(data, offset) \
    *(((volatile char *) USER_SPACE_ADDR) + offset) = data;

#define USER_readByte(offset) \
    *(((volatile char *) USER_SPACE_ADDR) + offset)

#define USER_writeHalfWord(data, offset) \
    *(((volatile short *) USER_SPACE_ADDR) + offset) = data;

#define USER_readHalfWord(offset) \
    *(((volatile short *) USER_SPACE_ADDR) + offset)

void main(void)
{
    ManagmentGpio_write(0);
    ManagmentGpio_outputEnable();
    
    uint32_t sram0_offset_base = (SRAM0_BASE - USER_SPACE_ADDR) >> 2;
    uint32_t sram1_offset_base = (SRAM1_BASE - USER_SPACE_ADDR) >> 2;
    
    uint32_t errors = 0;
    
    ManagmentGpio_write(1);
    
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t test_data = 0x12345678 + i;
        uint32_t offset = sram0_offset_base + i;
        USER_writeWord(test_data, offset);
    }
    
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t expected = 0x12345678 + i;
        uint32_t offset = sram0_offset_base + i;
        uint32_t read_data = USER_readWord(offset);
        
        if (read_data != expected) {
            errors++;
        }
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t word_addr = i * 4;
        uint32_t word_offset = sram0_offset_base + i;
        
        USER_writeWord(0x00000000, word_offset);
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t byte_offset = (SRAM0_BASE - USER_SPACE_ADDR) + (i * 4);
        
        USER_writeByte(0xAA, byte_offset + 0);
        USER_writeByte(0xBB, byte_offset + 1);
        USER_writeByte(0xCC, byte_offset + 2);
        USER_writeByte(0xDD, byte_offset + 3);
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t byte_offset = (SRAM0_BASE - USER_SPACE_ADDR) + (i * 4);
        
        uint8_t b0 = USER_readByte(byte_offset + 0);
        uint8_t b1 = USER_readByte(byte_offset + 1);
        uint8_t b2 = USER_readByte(byte_offset + 2);
        uint8_t b3 = USER_readByte(byte_offset + 3);
        
        if (b0 != 0xAA || b1 != 0xBB || b2 != 0xCC || b3 != 0xDD) {
            errors++;
        }
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t word_offset = sram0_offset_base + (i + 20);
        USER_writeWord(0x00000000, word_offset);
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t hw_offset = (sram0_offset_base * 2) + ((i + 20) * 2);
        
        USER_writeHalfWord(0x1234, hw_offset + 0);
        USER_writeHalfWord(0x5678, hw_offset + 1);
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t hw_offset = (sram0_offset_base * 2) + ((i + 20) * 2);
        
        uint16_t hw0 = USER_readHalfWord(hw_offset + 0);
        uint16_t hw1 = USER_readHalfWord(hw_offset + 1);
        
        if (hw0 != 0x1234 || hw1 != 0x5678) {
            errors++;
        }
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t word_offset = sram0_offset_base + (i + 40);
        USER_writeWord(0xFFFFFFFF, word_offset);
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t byte_offset = (SRAM0_BASE - USER_SPACE_ADDR) + ((i + 40) * 4);
        
        USER_writeByte(0x00, byte_offset + 1);
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t word_offset = sram0_offset_base + (i + 40);
        uint32_t expected = 0xFFFF00FF;
        uint32_t read_data = USER_readWord(word_offset);
        
        if (read_data != expected) {
            errors++;
        }
    }
    
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t test_data = 0xABCD0000 | i;
        uint32_t offset = sram1_offset_base + i;
        USER_writeWord(test_data, offset);
    }
    
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t expected = 0xABCD0000 | i;
        uint32_t offset = sram1_offset_base + i;
        uint32_t read_data = USER_readWord(offset);
        
        if (read_data != expected) {
            errors++;
        }
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t byte_offset = (SRAM1_BASE - USER_SPACE_ADDR) + (i * 4);
        
        USER_writeByte(0x11, byte_offset + 0);
        USER_writeByte(0x22, byte_offset + 1);
        USER_writeByte(0x33, byte_offset + 2);
        USER_writeByte(0x44, byte_offset + 3);
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t word_offset = sram1_offset_base + i;
        uint32_t expected = 0x44332211;
        uint32_t read_data = USER_readWord(word_offset);
        
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
