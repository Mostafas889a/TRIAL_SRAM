# Integration Notes

## Overview

This document provides detailed integration notes for the TRIAL_SRAM Caravel user project, including clock/reset architecture, bus timing, simulation setup, and verification methodology.

## Clock and Reset Architecture

### Clock Domains

The design uses a **single clock domain** for simplicity and reliability:

| Clock Signal | Source | Frequency | Usage |
|-------------|--------|-----------|-------|
| wb_clk_i | Caravel SoC | Up to 100 MHz | Wishbone bus and SRAM controller |

**Design Decision**: Single clock domain chosen to:
- Eliminate CDC (Clock Domain Crossing) complexity
- Simplify verification and timing closure
- Reduce metastability risks
- Comply with SRAM IP operating specifications (100 MHz max)

### Reset Strategy

| Reset Signal | Type | Polarity | Domain |
|-------------|------|----------|--------|
| wb_rst_i | Synchronous | Active-High | Wishbone clock domain |

**Reset Characteristics**:
- **Synchronous reset**: Aligned with `wb_clk_i` rising edge
- **Active-high**: Reset asserted when `wb_rst_i = 1`
- **Duration**: Minimum 2 clock cycles recommended
- **Release**: Synchronous deassertion

**Reset Sequence**:
1. Caravel management SoC asserts `wb_rst_i`
2. User project resets all internal registers
3. SRAM controller state machines return to idle
4. Wishbone outputs driven to safe states (`wbs_ack_o = 0`, `wbs_dat_o = 0`)
5. SRAM contents **not cleared** (SRAM is non-volatile within power cycle)

## Bus Architecture

### Wishbone B4 (Classic) Implementation

**Bus Configuration**:
- **Protocol**: Wishbone B4 (Classic), registered feedback
- **Data Width**: 32 bits
- **Address Width**: 32 bits (only [19:2] used)
- **Granularity**: 8 bits (byte-level access)
- **Endianness**: Little-endian

### Address Decoder Implementation

The user_project module implements a simple combinational address decoder:

```verilog
wire [3:0] peripheral_select = wbs_adr_i[19:16];
wire sram0_stb = (peripheral_select == 4'h0) & wbs_stb_i;
wire sram1_stb = (peripheral_select == 4'h1) & wbs_stb_i;
```

**Key Implementation Rules**:
1. `wbs_cyc_i` is **never gated** - routed directly to all peripherals
2. Only `wbs_stb_i` is gated for peripheral selection
3. One peripheral active at a time (one-hot strobe selection)
4. Invalid addresses handled by default case (returns 0xDEADBEEF)

### Bus Timing

#### Read Cycle Timing

```
Cycle:        T0      T1      T2
            ___/‾‾\___/‾‾\___/‾‾\___
wb_clk_i

wbs_cyc_i   ___/‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\___

wbs_stb_i   ___/‾‾‾‾‾‾‾‾‾‾\___________

wbs_we_i    _______________________

wbs_adr_i   ---<ADDR>---------------

wbs_ack_o   _______/‾‾‾‾‾‾\___________

wbs_dat_o   -----------<DATA>-------
```

**Timing Parameters**:
- Address setup: Available before T0 rising edge
- Strobe asserted: T0
- Acknowledge asserted: T1 (one cycle later)
- Data valid: T1 (same cycle as ACK)
- Strobe deasserted: T2

#### Write Cycle Timing

```
Cycle:        T0      T1      T2
            ___/‾‾\___/‾‾\___/‾‾\___
wb_clk_i

wbs_cyc_i   ___/‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\___

wbs_stb_i   ___/‾‾‾‾‾‾‾‾‾‾\___________

wbs_we_i    ___/‾‾‾‾‾‾‾‾‾‾\___________

wbs_adr_i   ---<ADDR>---------------

wbs_dat_i   ---<DATA>---------------

wbs_sel_i   ---<SEL>----------------

wbs_ack_o   _______/‾‾‾‾‾‾\___________
```

**Timing Parameters**:
- Address/data setup: Available before T0 rising edge
- Write enable asserted: T0
- Acknowledge asserted: T1 (one cycle later)
- Data written to SRAM: T1 rising edge

### Back-to-Back Transactions

The bus supports back-to-back transactions with no dead cycles:

```
Cycle:        T0   T1   T2   T3   T4
            ___/‾\__/‾\__/‾\__/‾\__/‾\___
wb_clk_i

wbs_cyc_i   ___/‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\___

wbs_stb_i   ___/‾‾‾‾‾‾\___/‾‾‾‾‾‾\______

wbs_adr_i   ---<ADR0>--<ADR1>----------

wbs_ack_o   _______/‾‾\______/‾‾\______
```

**Maximum throughput**: One transaction per clock cycle

## Interrupt Architecture

This design **does not implement interrupts** because:
1. SRAMs are synchronous memory devices (no async events)
2. All accesses complete in one cycle (no completion notification needed)
3. No error conditions that require interrupt handling

If interrupts were needed in future versions:
- Caravel provides `user_irq[2:0]` (3 interrupt lines)
- Interrupts should be level-triggered (active-high)
- Status registers should use W1C (write-one-to-clear) for flags

## Power Domains

### Caravel Power Domains

| Domain | Voltage | Usage |
|--------|---------|-------|
| VCCD1 | 1.8V | Management SoC core |
| VCCD2 | 1.8V | User project area |
| VDDA1 | 3.3V | Management SoC analog |
| VDDA2 | 3.3V | User project analog |

### SRAM Power Configuration

The CF_SRAM_1024x32 IP uses multiple power pins:

```verilog
`ifdef USE_POWER_PINS
    .VPWR(vccd2),  // User area power
    .VGND(vssd2),  // User area ground
`endif
```

**Power Switch Configuration**: Disabled (default recommended mode)
- `vpwrm` → `vccd2` (main power)
- `vpwra` → `vccd2` (array power)
- `vpwrp` → `vccd2` (periphery power)
- `vpwrac` → `1'b1` (power switch control - keep on)
- `vpwrpc` → `1'b1` (power switch control - keep on)
- `vgnd` → `vssd2` (ground)
- `vpb` → `vccd2` (P-well bias)
- `vnb` → `vssd2` (N-well bias)

## Physical Design Considerations

### Area Estimation

| Component | Area (um²) | Notes |
|-----------|-----------|-------|
| CF_SRAM_1024x32 (each) | 0.118 mm² | Hard macro (387.87 × 303.32 um) |
| Total SRAM area | 0.236 mm² | Two instances |
| Wishbone decoder | ~0.005 mm² | Small combinational logic |
| **Total estimate** | ~0.241 mm² | Excluding routing |

**Caravel user area**: ~3000 × 3600 um = 10.8 mm²  
**Utilization**: ~2.2% (very low, plenty of room for expansion)

### Macro Placement

Default macro placement in `user_project_wrapper`:

| Instance | Location (um) | Orientation |
|----------|--------------|-------------|
| sram0_inst | (500, 500) | N |
| sram1_inst | (1200, 500) | N |

**Placement Strategy**:
- Place near Wishbone bus connection point
- Allow for routing channels between macros
- Leave space for future peripherals
- Avoid placement near power pads (if applicable)

### Routing Considerations

- Use automatic routing (OpenLane handles this)
- SRAM macros have fixed pin locations (LEF defined)
- Wishbone signals are low-speed (no special routing needed)
- No high-speed or analog signals to shield

## Simulation and Verification

### Simulation Environments

#### 1. RTL Simulation (cocotb)

**Purpose**: Functional verification of Wishbone protocol and SRAM access

**Tools**:
- **Simulator**: Icarus Verilog (iverilog)
- **Framework**: cocotb (Python-based testbenches)
- **Waveform viewer**: GTKWave

**Test Coverage**:
- Single SRAM read/write operations
- Back-to-back transactions
- Byte-lane writes (partial word writes)
- Invalid address handling
- Bus protocol compliance
- Concurrent access arbitration (should not occur)

**Run Command**:
```bash
cd verilog/dv/cocotb
python cocotb_tests.py --test=sram0_test
```

#### 2. Gate-Level Simulation

**Purpose**: Verify synthesized netlist matches RTL behavior

**Additional checks**:
- Timing violations (setup/hold)
- X-propagation
- Scan chain integrity (if DFT enabled)

**Run Command**:
```bash
cd verilog/dv/cocotb
python cocotb_tests.py --test=sram0_test --gl
```

#### 3. SDF Simulation (Post-Layout)

**Purpose**: Verify with back-annotated timing from layout

**Run Command**:
```bash
cd verilog/dv/cocotb
python cocotb_tests.py --test=sram0_test --sdf
```

### Verification Methodology

#### Test Plan Structure

Each peripheral has dedicated tests:
1. **sram0_test**: Test SRAM0 functionality
2. **sram1_test**: Test SRAM1 functionality  
3. **dual_sram_test**: Test both SRAMs (address decode verification)
4. **system_integration_test**: Full system test with firmware

#### Coverage Goals

| Category | Target | Measurement |
|----------|--------|-------------|
| Functional | 100% | Manual checklist |
| Code | 95%+ | Not measured (simple design) |
| Toggle | 90%+ | Not measured |
| Protocol | 100% | Wishbone compliance checker |

#### Self-Checking Tests

All tests are self-checking using Python assertions:

```python
def test_sram_write_read():
    # Write test pattern
    await write_sram(0x30000000, 0x12345678)
    
    # Read back and verify
    data = await read_sram(0x30000000)
    assert data == 0x12345678, f"Mismatch: got {data:08x}"
```

### Firmware Testing

#### Firmware Structure

```
fw/
├── sram_test.c          # C test code
├── sram_test.h          # Register definitions
└── firmware_apis.h      # Caravel utility functions
```

#### Test Firmware Features

1. **Basic R/W test**: Write patterns, read back, verify
2. **Walking ones/zeros**: Test all address bits
3. **Byte-lane test**: Partial word writes
4. **Performance test**: Measure access latency
5. **Stress test**: Continuous R/W for reliability

#### Running Firmware Tests

```bash
cd verilog/dv/cocotb/sram_test
make clean
make
python sram_test.py
```

## OpenLane Synthesis and Hardening

### Synthesis Strategy

**Two-stage hardening**:
1. **Stage 1**: Harden `user_project` (the Wishbone bus logic and SRAM instances)
2. **Stage 2**: Harden `user_project_wrapper` (includes hardened user_project macro)

**Note**: Per the SRAM PnR guidelines, the CF_SRAM_1024x32 instances are treated as hard macros and should be integrated at the wrapper level, not synthesized.

### OpenLane Configuration

**For user_project_wrapper** (see `openlane/user_project_wrapper/config.json`):

Key settings:
```json
{
    "DESIGN_NAME": "user_project_wrapper",
    "CLOCK_PORT": "wb_clk_i",
    "CLOCK_PERIOD": 25,
    "FP_PDN_MULTILAYER": false,
    "MACROS": {
        "sram0_inst": {
            "gds": ["dir::../../ip/CF_SRAM_1024x32/gds/..."],
            "lef": ["dir::../../ip/CF_SRAM_1024x32/lef/..."],
            ...
        }
    }
}
```

### Hardening Flow

```bash
# Harden user_project_wrapper
openlane /workspace/TRIAL_SRAM/openlane/user_project_wrapper/config.json \
    --ef-save-views-to /workspace/TRIAL_SRAM

# Check results
python -c "from rtl_agent_skills import get_latest_run_dir, view_openlane_metrics; \
    run_dir = get_latest_run_dir('openlane/user_project_wrapper/runs'); \
    print(f'Latest run: {run_dir}'); \
    view_openlane_metrics('openlane/user_project_wrapper')"
```

### Success Criteria

- ✅ No synthesis errors
- ✅ No inferred latches
- ✅ No timing violations at target frequency (40 MHz typical)
- ✅ No DRC violations
- ✅ No LVS violations
- ✅ Routing convergence (no unrouted nets)

## Timing Constraints

### Clock Constraints

```sdc
create_clock [get_ports wb_clk_i] -name wb_clk -period 25.0
```

**Target frequency**: 40 MHz (25 ns period)  
**SRAM max frequency**: 100 MHz (10 ns period)  
**Margin**: 2.5× safety factor

### Input/Output Delays

```sdc
set_input_delay -clock wb_clk -max 5.0 [all_inputs]
set_input_delay -clock wb_clk -min 1.0 [all_inputs]
set_output_delay -clock wb_clk -max 5.0 [all_outputs]
set_output_delay -clock wb_clk -min 1.0 [all_outputs]
```

### False Paths

No false paths in this design (single clock domain, all paths are real).

## Known Issues and Limitations

### Current Limitations

1. **Fixed SRAM size**: Both SRAMs are 4 KB (not different sizes as originally requested)
   - **Reason**: Only CF_SRAM_1024x32 is available as hard macro with WB wrapper
   - **Future**: Could integrate different SRAM sizes if IPs become available

2. **No write protection**: SRAMs are always writable when addressed
   - **Mitigation**: Implement software-based access control if needed

3. **No ECC**: Memory does not have error correction
   - **Acceptable for**: Non-critical applications, prototyping
   - **Future**: Add ECC wrapper if reliability critical

4. **No power management**: SRAMs always powered on
   - **Impact**: Continuous leakage power (~42 nW per SRAM)
   - **Future**: Could implement power switching (IP supports it)

### Debugging Tips

**Common issues**:

1. **Bus hangs**: Usually caused by gating `wbs_cyc_i` (don't do this!)
   - **Fix**: Route `wbs_cyc_i` directly to all peripherals

2. **Read returns 0xDEADBEEF**: Invalid address
   - **Fix**: Check address calculation, verify peripheral selection logic

3. **Ack never asserted**: Strobe not reaching peripheral
   - **Fix**: Verify address decoder logic, check `wbs_stb_i` gating

4. **Simulation X's**: Uninitialized signals or timing violations
   - **Fix**: Check reset initialization, verify all signals driven

## Performance Analysis

### Access Latency

| Operation | Latency (cycles) | Latency (ns @ 40 MHz) |
|-----------|-----------------|---------------------|
| Read | 1 | 25 ns |
| Write | 1 | 25 ns |

### Throughput

| Metric | Value |
|--------|-------|
| Max read throughput | 160 MB/s (4 bytes × 40 MHz) |
| Max write throughput | 160 MB/s |
| Random access time | 25 ns |

### Power Consumption

**Per SRAM (typical @ 1.8V, 25°C)**:
- Active read: 8.6 mA
- Active write: 7.8 mA  
- Standby (EN=0): 0.019 mA
- Leakage: 23.1 nA

**Total system** (both SRAMs, worst case):
- Active: ~17 mA (assuming one active at a time)
- Idle: ~38 µA

## References

### Documentation

- [Caravel User Project Documentation](https://caravel-user-project.readthedocs.io/)
- [Wishbone B4 Specification](https://opencores.org/howto/wishbone)
- [CF_SRAM_1024x32 Datasheet](../ip/CF_SRAM_1024x32/README.md)

### Related Files

- [Register Map](register_map.md)
- [Pad Map](pad_map.md)
- [Project README](../README.md)

## Revision History

| Version | Date       | Description |
|---------|-----------|-------------|
| 1.0     | 2025-11-01 | Initial integration notes for dual SRAM project |

---

*Last Updated: 2025-11-01*
