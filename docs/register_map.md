# Register Map and Address Space

## Overview

This document describes the memory map and register layout for the TRIAL_SRAM Caravel user project, which integrates two SRAM peripherals with Wishbone B4 (classical) bus interface.

## Memory Map

The user project occupies the Caravel user project address space starting at `0x3000_0000`. Each SRAM peripheral is allocated a 64 KB (0x10000) address window.

| Peripheral | Base Address | End Address | Size | Description |
|-----------|-------------|-------------|------|-------------|
| SRAM0     | 0x30000000  | 0x3000FFFF  | 4 KB (1024 words) | First SRAM instance |
| SRAM1     | 0x30010000  | 0x3001FFFF  | 4 KB (1024 words) | Second SRAM instance |

## Address Decoding

### Peripheral Selection

The address decoder uses bits `[19:16]` of the Wishbone address to select between peripherals:

```
wbs_adr_i[19:16] == 4'h0  →  SRAM0 selected (0x30000000 - 0x3000FFFF)
wbs_adr_i[19:16] == 4'h1  →  SRAM1 selected (0x30010000 - 0x3001FFFF)
wbs_adr_i[19:16] >= 4'h2  →  Invalid address (returns 0xDEADBEEF)
```

### Word Address Calculation

Each SRAM is organized as 1024 words × 32 bits. The word address is derived from bits `[11:2]` of the Wishbone address:

```
Word Address = wbs_adr_i[11:2]  (10 bits = 1024 addressable words)
```

- Bits `[1:0]` are ignored (word-aligned access only)
- Bits `[15:12]` provide additional address space within the 64 KB window (unused by the 4 KB SRAM)

## SRAM Memory Organization

### CF_SRAM_1024x32 Configuration

Both SRAM instances use the CF_SRAM_1024x32 IP core with the following characteristics:

- **Memory Size**: 1024 words × 32 bits = 4 KB
- **Data Width**: 32 bits
- **Address Width**: 10 bits (words 0-1023)
- **Byte Enable**: 4 bits (one per byte lane)
- **Access Type**: Synchronous read/write

### Memory Access

#### Read Operation

1. Master asserts `wbs_cyc_i` and `wbs_stb_i`
2. Master provides address on `wbs_adr_i`
3. Master sets `wbs_we_i = 0` (read)
4. Slave (SRAM wrapper) asserts `wbs_ack_o` after one cycle
5. Slave provides data on `wbs_dat_o`

**Timing**: 1-cycle read latency (data available on the cycle following the request)

#### Write Operation

1. Master asserts `wbs_cyc_i` and `wbs_stb_i`
2. Master provides address on `wbs_adr_i`
3. Master provides data on `wbs_dat_i`
4. Master sets `wbs_we_i = 1` (write)
5. Master sets `wbs_sel_i` to indicate which byte lanes to write
6. Slave (SRAM wrapper) asserts `wbs_ack_o` after one cycle

**Timing**: 1-cycle write latency

### Byte Lane Selection

The `wbs_sel_i[3:0]` signal controls which bytes are written during write operations:

| wbs_sel_i | Bytes Written | Description |
|-----------|---------------|-------------|
| 4'b0001   | [7:0]         | Write byte 0 only |
| 4'b0010   | [15:8]        | Write byte 1 only |
| 4'b0100   | [23:16]       | Write byte 2 only |
| 4'b1000   | [31:24]       | Write byte 3 only |
| 4'b0011   | [15:0]        | Write half-word (bytes 0-1) |
| 4'b1100   | [31:16]       | Write half-word (bytes 2-3) |
| 4'b1111   | [31:0]        | Write full word (all bytes) |

During read operations, `wbs_sel_i` is ignored and all 32 bits are returned.

## Address Examples

### SRAM0 Access Examples

| Address      | Word Index | Description |
|-------------|-----------|-------------|
| 0x30000000  | 0         | First word of SRAM0 |
| 0x30000004  | 1         | Second word of SRAM0 |
| 0x30000008  | 2         | Third word of SRAM0 |
| ...         | ...       | ... |
| 0x30000FFC  | 1023      | Last word of SRAM0 (word 1023) |

### SRAM1 Access Examples

| Address      | Word Index | Description |
|-------------|-----------|-------------|
| 0x30010000  | 0         | First word of SRAM1 |
| 0x30010004  | 1         | Second word of SRAM1 |
| 0x30010008  | 2         | Third word of SRAM1 |
| ...         | ...       | ... |
| 0x30010FFC  | 1023      | Last word of SRAM1 (word 1023) |

## Error Handling

### Invalid Address Access

Accesses to addresses outside the allocated SRAM ranges will:
- **Read**: Return `0xDEADBEEF`
- **Write**: Assert `wbs_ack_o` but discard the data

### Out-of-Bounds Within Allocated Space

Accesses within the 64 KB peripheral window but beyond the 4 KB SRAM boundary (e.g., 0x30001000-0x3000FFFF for SRAM0):
- Are handled by the SRAM wrapper
- Only addresses [11:2] are used; higher bits are ignored
- Effectively wraps within the 4 KB space

## Bus Protocol Details

### Wishbone B4 Classic Protocol

The implementation follows Wishbone B4 (classic) specification:

- **Bus Width**: 32 bits
- **Granularity**: 8 bits (byte-addressable)
- **Endianness**: Little-endian
- **Max Bus Cycles**: No limit (standard handshake)
- **Signal Synchronization**: All signals synchronous to `wb_clk_i`

### Handshake Timing

```
         _   _   _   _   _   _
wb_clk  | |_| |_| |_| |_| |_| |

cyc     ___/‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\___
stb     ___/‾‾‾‾‾‾‾\___________
we      ___/‾‾‾‾‾‾‾\___________
ack     _______/‾‾‾\___________
adr     ---<ADDR>--------------
dat_i   ---<DATA>--------------  (write)
dat_o   -------<DATA>----------  (read)
```

### Critical Rules

1. **Never gate `wbs_cyc_i`**: Route `wbs_cyc_i` unmodified to all SRAM peripherals
2. **Gate only `wbs_stb_i`**: Peripheral selection done by gating the strobe signal
3. **One-cycle ACK**: All transactions acknowledged in exactly one cycle
4. **No bus hangs**: Invalid addresses still return ACK (with dummy data for reads)

## Reset Behavior

All SRAM contents are **undefined** after reset. The Wishbone controller registers are reset to safe states:
- `wbs_ack_o = 0`
- `wbs_dat_o = 0x00000000`

Software must initialize SRAM contents before use.

## Performance Characteristics

- **Read Latency**: 1 cycle
- **Write Latency**: 1 cycle
- **Maximum Frequency**: 100 MHz (typical, per SRAM datasheet)
- **Bus Utilization**: One transaction per cycle (no wait states)

## Software Access Example (C)

```c
#define SRAM0_BASE 0x30000000
#define SRAM1_BASE 0x30010000

// Write to SRAM0
volatile uint32_t *sram0 = (volatile uint32_t *)SRAM0_BASE;
sram0[0] = 0x12345678;   // Write to word 0
sram0[100] = 0xABCDEF00; // Write to word 100

// Read from SRAM1
volatile uint32_t *sram1 = (volatile uint32_t *)SRAM1_BASE;
uint32_t value = sram1[0];  // Read from word 0
```

## Revision History

| Version | Date       | Description |
|---------|-----------|-------------|
| 1.0     | 2025-11-01 | Initial register map for dual SRAM integration |

---

*Last Updated: 2025-11-01*
