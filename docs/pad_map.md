# Pad Map and IO Configuration

## Overview

This document describes the IO pad assignments for the TRIAL_SRAM Caravel user project. Since the integrated SRAMs are internal memory peripherals with Wishbone bus interface, they do not require external IO pins.

## Caravel IO Overview

The Caravel SoC provides 38 configurable IO pins (`mprj_io[37:0]`) for user projects. These pins can be configured as inputs, outputs, or bidirectional IOs.

## TRIAL_SRAM IO Requirements

### Internal-Only Design

The TRIAL_SRAM project uses **no external IO pins** because:

1. Both SRAM peripherals are accessed exclusively through the Wishbone bus
2. All communication is internal to the chip (CPU ↔ Wishbone ↔ SRAM)
3. No external memory interface or debug signals are required

### Reserved Caravel IOs

The following Caravel IO pins are reserved by the system and **cannot be used** by user projects:

| Pin Index | Function | Description |
|-----------|----------|-------------|
| mprj_io[0] | JTAG | JTAG interface (reserved) |
| mprj_io[1] | SDO | Housekeeping SPI SDO |
| mprj_io[2] | SDI | Housekeeping SPI SDI |
| mprj_io[3] | CSB | Housekeeping SPI CSB |
| mprj_io[4] | SCK | Housekeeping SPI SCK |

### User Project IO Assignment

Since this project requires no external IOs, all user-accessible pins (`mprj_io[37:5]`) are left **unconnected** or **tristated**.

## IO Configuration in user_project_wrapper

### Default Configuration

All IO pins are configured as inputs with output drivers disabled:

```verilog
assign mprj_io_out[37:0] = 38'b0;  // All outputs driven low
assign mprj_io_oeb[37:0] = 38'h3FFFFFFFFF;  // All OEBs high (tristated)
```

**Note**: `mprj_io_oeb` is active-low:
- `1` = Output driver disabled (input mode)
- `0` = Output driver enabled (output mode)

## Future Expansion Options

If future versions of this project require external IOs (e.g., for debug or monitoring), the following pins are available:

### Available IO Pins

| Pin Range | Count | Suggested Use Cases |
|-----------|-------|-------------------|
| mprj_io[37:5] | 33 pins | Debug signals, status LEDs, external triggers |

### Example Debug Signal Assignment

If debug signals are needed in the future, they could be assigned as follows:

| Pin Index | Direction | Function | Description |
|-----------|-----------|----------|-------------|
| mprj_io[5] | Output | sram0_active | SRAM0 access indicator |
| mprj_io[6] | Output | sram1_active | SRAM1 access indicator |
| mprj_io[7] | Output | bus_error | Invalid address access |
| mprj_io[8] | Output | heartbeat | Clock divider heartbeat |

**Implementation Example**:

```verilog
// Debug outputs (example - not currently implemented)
assign mprj_io_out[5] = sram0_active;
assign mprj_io_out[6] = sram1_active;
assign mprj_io_out[7] = bus_error;
assign mprj_io_out[8] = heartbeat;

// Enable output drivers for debug pins
assign mprj_io_oeb[8:5] = 4'b0000;  // Enable outputs

// Tristate remaining pins
assign mprj_io_oeb[37:9] = 29'h1FFFFFFF;
```

## IO Power Domains

The Caravel IO pads are powered by different domains depending on their location:

| Domain | Pins | Power Supply |
|--------|------|-------------|
| VDDIO  | Most IOs | 3.3V (external IO voltage) |
| VDDA   | Analog IOs | 3.3V (analog supply) |
| VCCD   | Core logic | 1.8V (core digital supply) |

Since this project doesn't use IOs, power domain considerations are not applicable.

## Pin Configuration Registers

Caravel provides configuration registers for each IO pin to set:
- **Pull-up/pull-down**: Enable weak pull resistors
- **Slew rate**: Control output transition speed
- **Drive strength**: Set output drive current
- **Input buffer**: Enable/disable input buffer

These are configured through the Caravel management SoC, typically during firmware initialization. For this project, default settings are sufficient since no IOs are used.

## Firmware Configuration

No special firmware configuration is required for IO pins since this project uses only internal Wishbone-connected SRAMs.

If IOs were needed, the typical initialization sequence would be:

```c
// Example IO configuration (not needed for TRIAL_SRAM)
#include "caravel.h"

void configure_io() {
    // Set GPIO mode for specific pins
    // reg_mprj_io_5 = GPIO_MODE_MGMT_STD_OUTPUT;
    
    // Apply configuration
    // reg_mprj_xfer = 1;
    // while (reg_mprj_xfer == 1);
}
```

## Physical Pad Locations

For reference, the Caravel chip IO pad locations are distributed around the chip periphery:

- **West side**: mprj_io[0:7]
- **North side**: mprj_io[8:18]
- **East side**: mprj_io[19:29]
- **South side**: mprj_io[30:37]

This information is relevant for future board design if external connections are added.

## Layout Considerations

Since no IOs are used:
- No special pad ring considerations
- No ESD protection circuits needed (beyond Caravel defaults)
- No level shifters required
- Reduced power consumption (no IO buffer switching)

## Testing and Debug

### Simulation

During simulation (cocotb), IO pin states can be monitored:

```python
# Monitor IO outputs (all should be 0 or tristated)
io_out = dut.mprj_io_out.value
io_oeb = dut.mprj_io_oeb.value

# Verify all IOs are tristated
assert io_oeb == 0x3FFFFFFFFF, "IOs should be tristated"
```

### Post-Silicon Debug

Since no IOs are exposed, post-silicon debug relies on:
1. **Wishbone bus access** via management SoC
2. **LA (Logic Analyzer) probes** (Caravel provides 128 LA signals)
3. **UART output** from management SoC firmware

## Summary

**TRIAL_SRAM IO Usage:**
- External IOs used: **0**
- External IOs available: **33** (mprj_io[37:5])
- Configuration required: **None** (all IOs tristated)

This design demonstrates a pure Wishbone-based integration with no external IO requirements, simplifying the physical design and reducing pin count constraints.

## Revision History

| Version | Date       | Description |
|---------|-----------|-------------|
| 1.0     | 2025-11-01 | Initial pad map for dual SRAM integration |

---

*Last Updated: 2025-11-01*
