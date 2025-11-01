from caravel_cocotb.caravel_interfaces import test_configure, report_test
import cocotb

async def wait_for_pulses(env, count, label):
    for _ in range(count):
        await env.wait_mgmt_gpio(1)
        await env.wait_mgmt_gpio(0)
    cocotb.log.info(f"[TEST] {label}")

@cocotb.test()
@report_test
async def system_integration_test(dut):
    caravelEnv = await test_configure(dut, timeout_cycles=1000000)
    cocotb.log.info("[TEST] Start system_integration_test")
    await caravelEnv.release_csb()
    
    cocotb.log.info("[TEST] Wait for management GPIO pulse (firmware ready)")
    await wait_for_pulses(caravelEnv, 1, "firmware ready")
    
    cocotb.log.info("[TEST] SRAM0 write/read test")
    await wait_for_pulses(caravelEnv, 1, "SRAM0 test complete")
    
    cocotb.log.info("[TEST] SRAM1 write/read test")
    await wait_for_pulses(caravelEnv, 1, "SRAM1 test complete")
    
    cocotb.log.info("[TEST] Verify SRAM0 data still intact")
    await wait_for_pulses(caravelEnv, 1, "SRAM0 verification complete")
    
    cocotb.log.info("[TEST] Wait for final test result")
    pulses = 0
    try:
        await wait_for_pulses(caravelEnv, 2, "Test passed")
        pulses = 2
    except:
        pulses = 1
    
    if pulses == 2:
        cocotb.log.info("[TEST] System integration test PASSED")
    else:
        cocotb.log.error("[TEST] System integration test FAILED")
        assert False, "System integration test failed"
