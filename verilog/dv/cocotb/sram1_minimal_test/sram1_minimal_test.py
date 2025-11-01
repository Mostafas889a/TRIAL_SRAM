from caravel_cocotb.caravel_interfaces import test_configure
from caravel_cocotb.caravel_interfaces import report_test
import cocotb

@cocotb.test()
@report_test
async def sram1_minimal_test(dut):
    caravelEnv = await test_configure(dut, timeout_cycles=1000000)
    
    cocotb.log.info(f"[TEST] Start sram1_minimal_test")
    
    await caravelEnv.release_csb()
    cocotb.log.info(f"[TEST] Wait for management GPIO pulse (firmware ready)")
    await caravelEnv.wait_mgmt_gpio(1)
    cocotb.log.info(f"[TEST] firmware ready")
    
    cocotb.log.info(f"[TEST] Wait for test result")
    passed = await caravelEnv.wait_mgmt_gpio(2)
    
    if passed:
        cocotb.log.info(f"[TEST] SRAM1 minimal test passed!")
    else:
        cocotb.log.error(f"[TEST] SRAM1 minimal test failed!")
        assert False
