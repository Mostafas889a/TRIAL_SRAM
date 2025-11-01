from caravel_cocotb.caravel_interfaces import test_configure
from caravel_cocotb.caravel_interfaces import report_test
import cocotb

@cocotb.test()
@report_test
async def sram1_debug_test(dut):
    caravelEnv = await test_configure(dut, timeout_cycles=1000000)
    cocotb.log.info(f"[TEST] Start sram1_debug_test")
    
    await caravelEnv.release_csb()
    await caravelEnv.wait_mgmt_gpio(1)
    cocotb.log.info(f"[TEST] firmware ready")
    
    cocotb.log.info(f"[TEST] Waiting for test result pulses...")
    # Wait for multiple pulses to track progress
    for pulse_num in range(1, 6):
        await caravelEnv.wait_mgmt_gpio(1)
        cocotb.log.info(f"[TEST] Got pulse #{pulse_num}")
    
    # Get final result (should be 2 pulses for PASS)
    pulses = await caravelEnv.monitor_gpio(37, 2)
    if pulses == 2:
        cocotb.log.info(f"[TEST] PASS - SRAM1 test successful")
    else:
        cocotb.log.error(f"[TEST] FAIL - SRAM1 test failed")
