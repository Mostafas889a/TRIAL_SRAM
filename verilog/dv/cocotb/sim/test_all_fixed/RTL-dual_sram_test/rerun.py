
import os
import shutil
import argparse

parser = argparse.ArgumentParser(description="Run cocotb tests")
parser.add_argument("-extend", help="extend the command")
args = parser.parse_args()

os.environ["CARAVEL_ROOT"] = "/nc/templates/caravel"
os.environ["MCW_ROOT"] = "/nc/templates/mgmt_core_wrapper"

os.chdir("/workspace/TRIAL_SRAM/verilog/dv/cocotb")

command = "python3 /usr/local/bin/caravel_cocotb -test dual_sram_test -tag test_all_fixed/RTL-dual_sram_test/rerun   -sim RTL -corner nom-t  -seed 1761992913 "
if args.extend is not None:
    command += f" {args.extend}"
os.system(command)

shutil.copyfile("/workspace/TRIAL_SRAM/verilog/dv/cocotb/sim/test_all_fixed/RTL-dual_sram_test/rerun.py", "/workspace/TRIAL_SRAM/verilog/dv/cocotb/sim/test_all_fixed/RTL-dual_sram_test/rerun/RTL-dual_sram_test/rerun.py")
