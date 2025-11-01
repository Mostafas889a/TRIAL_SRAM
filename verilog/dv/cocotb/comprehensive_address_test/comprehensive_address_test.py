import cocotb
from caravel_cocotb.caravel_interfaces import test_configure
from caravel_cocotb.caravel_interfaces import report_test
import cocotb.log

@cocotb.test()
@report_test
async def comprehensive_address_test(dut):
    caravelEnv = await test_configure(dut, timeout_cycles=3000000)
    cocotb.log.info("[TEST] Start comprehensive address coverage test")
    
    await caravelEnv.release_csb()
    
    cocotb.log.info("[TEST] Wait for GPIO pulse indicating test start")
    await caravelEnv.wait_mgmt_gpio(1)
    cocotb.log.info("[TEST] Test started - writing corner addresses")
    
    cocotb.log.info("[TEST] Wait for test completion")
    await caravelEnv.wait_mgmt_gpio(0)
    
    cocotb.log.info("[TEST] Wait for final result pulse")
    await caravelEnv.wait_mgmt_gpio(1)
    cocotb.log.info("[TEST] Test passed - all address tests successful")
    
    cocotb.log.info("[TEST] Comprehensive address test PASSED")
