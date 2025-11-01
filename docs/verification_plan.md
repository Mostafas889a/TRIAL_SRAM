# Verification Plan for TRIAL_SRAM

## Overview

This document outlines the verification strategy for the TRIAL_SRAM Caravel user project, which integrates two CF_SRAM_1024x32 memory peripherals via Wishbone B4 bus interface.

## Verification Objectives

1. **Functional Correctness**: Verify that both SRAM peripherals operate correctly
2. **Bus Protocol Compliance**: Ensure Wishbone B4 protocol is properly implemented
3. **Address Decoding**: Validate correct peripheral selection based on address
4. **Data Integrity**: Confirm data written to SRAMs can be read back correctly
5. **Integration**: Verify proper integration with Caravel SoC

## Test Environment

### Framework and Tools
- **Verification Framework**: cocotb (Python-based testbenches)
- **Simulator**: Icarus Verilog (iverilog)
- **Firmware**: C code compiled for Caravel management SoC
- **Waveform Viewer**: GTKWave
- **Clock Frequency**: 40 MHz (25 ns period)

### Test Configuration
- **Simulation Type**: RTL (behavioral models for SRAMs)
- **Timeout**: 1,000,000 cycles per test
- **Results Location**: `sim/<tag>/RTL-<test_name>/`

## Test Suite Architecture

### Test Categories

#### 1. Individual SRAM Tests
Test each SRAM peripheral independently to verify basic functionality.

#### 2. Dual SRAM Tests
Test both SRAMs together to verify address decoding and no cross-talk.

#### 3. System Integration Test
Comprehensive test that exercises the complete system.

## Detailed Test Descriptions

### Test 1: sram0_basic_test

**Purpose**: Verify SRAM0 basic read/write operations

**Test Sequence**:
1. Firmware configures Caravel and enables Wishbone interface
2. Firmware signals configuration complete via management GPIO pulse
3. Firmware writes test pattern to SRAM0 (base address 0x30000000)
4. Firmware reads back data from SRAM0
5. Firmware verifies data integrity
6. Firmware signals pass (2 pulses) or fail (1 pulse)
7. Python test monitors management GPIO and confirms test result

**Success Criteria**:
- Firmware completes without timeout
- All write operations acknowledged within 1 cycle
- Read data matches written data
- Final GPIO signal indicates pass (2 pulses)

**Test Patterns**:
- Walking ones: 0x00000001, 0x00000002, 0x00000004, ...
- Walking zeros: 0xFFFFFFFE, 0xFFFFFFFD, 0xFFFFFFFB, ...
- Random patterns
- Address = Data patterns

**Coverage**:
- All 1024 SRAM0 addresses
- Single-word writes (wbs_sel_i = 0xF)
- Single-word reads
- Back-to-back transactions

---

### Test 2: sram1_basic_test

**Purpose**: Verify SRAM1 basic read/write operations

**Test Sequence**: Same as sram0_basic_test but targeting SRAM1 (base 0x30010000)

**Success Criteria**: Same as sram0_basic_test

**Test Patterns**: Same patterns as SRAM0 test

**Coverage**:
- All 1024 SRAM1 addresses
- Single-word writes
- Single-word reads
- Back-to-back transactions

---

### Test 3: dual_sram_test

**Purpose**: Verify both SRAMs can be accessed independently without interference

**Test Sequence**:
1. Firmware configuration complete signal
2. Write unique pattern to SRAM0[0] (e.g., 0xAAAAAAAA)
3. Write different pattern to SRAM1[0] (e.g., 0x55555555)
4. Read back SRAM0[0] and verify
5. Read back SRAM1[0] and verify
6. Write to multiple addresses in both SRAMs
7. Read back all addresses and verify
8. Signal pass (2 pulses) or fail (1 pulse)

**Success Criteria**:
- Both SRAMs maintain independent data
- No cross-talk between peripherals
- Address decoding correctly routes to intended SRAM
- All operations complete successfully

**Test Patterns**:
- Complementary patterns (0xAAAAAAAA vs 0x55555555)
- Sequential addresses in both SRAMs
- Interleaved accesses (SRAM0, SRAM1, SRAM0, SRAM1, ...)

**Coverage**:
- Address decoder logic (bits [19:16])
- Strobe gating per peripheral
- Independent ACK generation
- Data multiplexing

---

### Test 4: byte_lane_test

**Purpose**: Verify byte-lane write functionality using wbs_sel_i

**Test Sequence**:
1. Write full word 0x00000000 to SRAM0[0]
2. Write byte 0 only (wbs_sel_i = 0x1): data = 0xAA
3. Read back and verify 0x000000AA
4. Write byte 1 only (wbs_sel_i = 0x2): data = 0xBB00
5. Read back and verify 0x0000BBAA
6. Write byte 2 only (wbs_sel_i = 0x4): data = 0xCC0000
7. Read back and verify 0x00CCBBAA
8. Write byte 3 only (wbs_sel_i = 0x8): data = 0xDD000000
9. Read back and verify 0xDDCCBBAA
10. Signal test result

**Success Criteria**:
- Each byte can be written independently
- Unselected bytes remain unchanged
- Half-word writes work correctly
- Full-word writes work correctly

**Test Patterns**:
- Individual byte writes
- Half-word writes (wbs_sel_i = 0x3, 0xC)
- Overlapping byte updates

**Coverage**:
- All 4 byte-lane selects
- All combinations of wbs_sel_i
- Partial word writes

---

### Test 5: address_decode_test

**Purpose**: Verify address decoder correctly handles valid and invalid addresses

**Test Sequence**:
1. Access valid SRAM0 addresses (0x30000000 - 0x30000FFC)
2. Access valid SRAM1 addresses (0x30010000 - 0x30010FFC)
3. Access invalid address in peripheral space (0x30020000)
4. Python test monitors for 0xDEADBEEF on invalid read
5. Verify all valid accesses work correctly
6. Verify invalid accesses return error code

**Success Criteria**:
- Valid addresses route to correct SRAM
- Invalid addresses return 0xDEADBEEF
- Invalid writes are ACKed but discarded
- No bus hangs on any address

**Test Patterns**:
- Boundary addresses (first and last word of each SRAM)
- Out-of-range addresses within 64KB windows
- Completely invalid peripheral select values

**Coverage**:
- All 4 bits of peripheral_select [19:16]
- Edge cases at peripheral boundaries
- Error handling paths

---

### Test 6: stress_test

**Purpose**: Stress test the system with continuous back-to-back transactions

**Test Sequence**:
1. Perform 1000 random write operations to random addresses in both SRAMs
2. Read back all written locations
3. Verify data integrity for all locations
4. Repeat for different random seeds
5. Signal test result

**Success Criteria**:
- No lost transactions
- No data corruption
- All ACKs received within 1 cycle
- System remains stable under continuous load

**Test Patterns**:
- Pseudo-random addresses
- Pseudo-random data
- Mix of SRAM0 and SRAM1 accesses
- Back-to-back transactions with no idle cycles

**Coverage**:
- Sustained bus utilization
- Random access patterns
- Worst-case switching scenarios

---

### Test 7: system_integration_test

**Purpose**: Comprehensive end-to-end system test

**Test Sequence**:
1. Run abbreviated version of all previous tests
2. Basic read/write to both SRAMs
3. Byte-lane test on one address
4. Address decode verification
5. Moderate stress test (100 operations)
6. Final verification and reporting

**Success Criteria**:
- All sub-tests pass
- System demonstrates stability
- Firmware execution completes without errors

**Coverage**:
- Integration of all test scenarios
- End-to-end workflow
- Real-world usage patterns

## Test Implementation Strategy

### Firmware (C Code)

Each test will include firmware that:
1. Initializes Caravel management SoC
2. Configures GPIOs (if needed - none for SRAM project)
3. Enables Wishbone user interface (`User_enableIF()`)
4. Signals configuration complete via management GPIO
5. Executes test-specific operations using `USER_writeWord()` and `USER_readWord()`
6. Verifies results
7. Signals test completion with pass/fail indication

**Synchronization Method**: Management GPIO pulses
- 1 pulse: Configuration complete
- 2 pulses: Test passed
- 1 pulse (after initial): Test failed

### Python Testbench

Each Python test will:
1. Configure Caravel environment (`test_configure()`)
2. Release CSB
3. Wait for firmware configuration complete (1 pulse)
4. Monitor test progression
5. Wait for test result signal (2 pulses = pass, 1 pulse = fail)
6. Assert pass/fail based on firmware result
7. Log detailed results

### Test Organization

```
verilog/dv/cocotb/
├── sram0_basic_test/
│   ├── sram0_basic_test.c
│   └── sram0_basic_test.py
├── sram1_basic_test/
│   ├── sram1_basic_test.c
│   └── sram1_basic_test.py
├── dual_sram_test/
│   ├── dual_sram_test.c
│   └── dual_sram_test.py
├── byte_lane_test/
│   ├── byte_lane_test.c
│   └── byte_lane_test.py
├── address_decode_test/
│   ├── address_decode_test.c
│   └── address_decode_test.py
├── stress_test/
│   ├── stress_test.c
│   └── stress_test.py
├── system_integration_test/
│   ├── system_integration_test.c
│   └── system_integration_test.py
├── cocotb_tests.py  (imports all tests)
└── design_info.yaml
```

## Coverage Goals

### Functional Coverage

| Feature | Target | Measurement Method |
|---------|--------|-------------------|
| SRAM0 word addresses | 100% | Test 1 covers all 1024 words |
| SRAM1 word addresses | 100% | Test 2 covers all 1024 words |
| Byte-lane combinations | 100% | Test 4 covers all wbs_sel_i values |
| Address decoder paths | 100% | Tests 3 & 5 cover valid/invalid paths |
| Back-to-back transactions | Yes | Tests 1, 2, 6 include consecutive ops |
| Bus protocol compliance | Yes | All tests verify ACK timing |

### Protocol Coverage

| Protocol Feature | Coverage | Test |
|-----------------|----------|------|
| wbs_cyc_i routing (never gated) | ✓ | All tests |
| wbs_stb_i gating (peripheral select) | ✓ | Tests 3, 5 |
| One-cycle ACK | ✓ | All tests |
| wbs_sel_i byte lanes | ✓ | Test 4 |
| Invalid address handling | ✓ | Test 5 |
| wbs_we_i (read vs write) | ✓ | All tests |

### Corner Cases

| Corner Case | Covered By |
|------------|-----------|
| First word of each SRAM | Tests 1, 2, 5 |
| Last word of each SRAM | Tests 1, 2, 5 |
| Boundary between peripherals | Test 5 |
| Invalid peripheral select | Test 5 |
| All zeros data pattern | Tests 1, 2 |
| All ones data pattern | Tests 1, 2 |
| Alternating bit patterns | Test 3 |

## Expected Results

### Per-Test Success Criteria

- **All tests must pass** for the design to be considered ready for synthesis
- Each test must complete within timeout period (1M cycles)
- Firmware must signal pass condition (2 GPIO pulses)
- No simulation errors or warnings (except benign timescale warnings from IP)

### Overall Success Criteria

- **7/7 tests passing (100% pass rate)**
- No bus protocol violations
- No data integrity issues
- Clean simulation logs
- Waveforms available for debug (if needed)

## Simulation Performance

### Expected Simulation Times

| Test | Expected Duration | Timeout |
|------|------------------|---------|
| sram0_basic_test | ~30 seconds | 600s |
| sram1_basic_test | ~30 seconds | 600s |
| dual_sram_test | ~40 seconds | 600s |
| byte_lane_test | ~20 seconds | 600s |
| address_decode_test | ~25 seconds | 600s |
| stress_test | ~60 seconds | 600s |
| system_integration_test | ~50 seconds | 600s |

**Total regression time**: ~5 minutes

## Debug Strategy

### If Test Fails

1. **Check firmware.log**: Look for compilation errors or firmware execution issues
2. **Check test.log**: Review cocotb test output for Python-side errors
3. **Review waves.vcd**: Open in GTKWave to inspect signal timing
4. **Key signals to monitor**:
   - `wb_clk_i`: Clock
   - `wb_rst_i`: Reset
   - `wbs_cyc_i`, `wbs_stb_i`: Bus cycle and strobe
   - `wbs_ack_o`: Acknowledge from user project
   - `wbs_adr_i`: Address bus
   - `wbs_dat_i`, `wbs_dat_o`: Data buses
   - `wbs_we_i`, `wbs_sel_i`: Write enable and byte select
   - `mprj.sram0_stb`, `mprj.sram1_stb`: Peripheral strobes

### Common Issues and Solutions

| Issue | Likely Cause | Solution |
|-------|-------------|----------|
| Test timeout | Firmware not starting | Check reset, clock, power sequence |
| No management GPIO pulse | Firmware stuck | Check firmware compilation, review firmware.log |
| Wrong data read back | Address decode error | Verify peripheral_select logic |
| Bus hang | Missing ACK | Check ACK generation logic |
| Intermittent failures | Timing issue | Review bus timing, check setup/hold |

## Regression Testing

### Test Execution Order

Run tests in this order for efficient debug:
1. `sram0_basic_test` - Simplest test, validates SRAM0
2. `sram1_basic_test` - Validates SRAM1
3. `dual_sram_test` - Validates address decode
4. `byte_lane_test` - Validates byte operations
5. `address_decode_test` - Validates error handling
6. `stress_test` - Validates robustness
7. `system_integration_test` - Final comprehensive test

### Regression Command

```bash
cd /workspace/TRIAL_SRAM/verilog/dv/cocotb
timeout 4200s caravel_cocotb -t sram0_basic_test sram1_basic_test dual_sram_test byte_lane_test address_decode_test stress_test system_integration_test -tag regression
```

## Post-Verification Activities

### Upon Successful Verification

1. Generate verification report (`verification_report.md`)
2. Create verification handoff document (`verification_handoff.md`)
3. Archive all test logs and waveforms
4. Update README with verification status
5. Proceed to Verification Evaluation stage
6. If evaluation passes, proceed to synthesis

### Upon Verification Failure

1. Analyze failure logs and waveforms
2. Identify root cause
3. Fix RTL or test as appropriate
4. Re-run failing test
5. Run full regression to ensure no regressions
6. Document issue and resolution in verification report

## Metrics and Reporting

### Key Metrics

- **Test Pass Rate**: 100% required
- **Code Coverage**: Not measured (design is simple)
- **Assertion Coverage**: Not applicable (no SystemVerilog assertions)
- **Bug Discovery Rate**: Track bugs found during verification
- **Test Development Time**: Track for future projects

### Report Contents

The final verification report will include:
- Summary of all tests run
- Pass/fail status for each test
- Simulation logs (excerpts)
- Key waveform screenshots (if issues found)
- List of bugs found and fixed
- Coverage analysis
- Lessons learned
- Recommendations for future work

## Risks and Mitigation

### Identified Risks

| Risk | Impact | Probability | Mitigation |
|------|--------|------------|-----------|
| Behavioral SRAM model differs from hard macro | High | Low | Use official IP behavioral model |
| Test timeout due to slow simulation | Medium | Medium | Set generous timeout (600s per test) |
| Firmware compilation issues | Medium | Low | Use proven firmware_apis.h |
| Caravel-cocotb framework issues | High | Low | Follow official templates exactly |

## Conclusion

This verification plan provides comprehensive coverage of the TRIAL_SRAM design. With 7 targeted tests covering functional correctness, protocol compliance, and system integration, we will achieve high confidence in the design before proceeding to synthesis.

**Next Steps**:
1. Implement all tests as described
2. Run individual tests and debug as needed
3. Run full regression
4. Generate verification report
5. Proceed to evaluation

---

**Document Version**: 1.0  
**Date**: 2025-11-01  
**Author**: NativeChips Agent  
**Status**: Active
