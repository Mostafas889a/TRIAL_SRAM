import cocotb
from caravel_cocotb.caravel_interfaces import test_configure
from caravel_cocotb.caravel_interfaces import report_test
import cocotb.log

@cocotb.test()
@report_test
async def access_type_test(dut):
    caravelEnv = await test_configure(dut, timeout_cycles=10000000)
    cocotb.log.info("[TEST] Start access type test (word, half-word, byte)")
    
    await caravelEnv.release_csb()
    
    cocotb.log.info("[TEST] Wait for GPIO pulse indicating test start")
    await caravelEnv.wait_mgmt_gpio(1)
    cocotb.log.info("[TEST] Test started - testing different access types")
    
    cocotb.log.info("[TEST] Wait for test completion")
    await caravelEnv.wait_mgmt_gpio(0)
    
    cocotb.log.info("[TEST] Wait for final result pulse")
    await caravelEnv.wait_mgmt_gpio(1)
    cocotb.log.info("[TEST] Test passed - all access types successful")
    
    cocotb.log.info("[TEST] Access type test PASSED")
