
import os
import shutil
import argparse

parser = argparse.ArgumentParser(description="Run cocotb tests")
parser.add_argument("-extend", help="extend the command")
args = parser.parse_args()

os.environ["CARAVEL_ROOT"] = "/nc/templates/caravel"
os.environ["MCW_ROOT"] = "/nc/templates/mgmt_core_wrapper"

os.chdir("/workspace/TRIAL_SRAM/verilog/dv/cocotb")

command = "python3 /usr/local/bin/caravel_cocotb -test sram0_basic_test -tag test_fixed_sram0/RTL-sram0_basic_test/rerun   -sim RTL -corner nom-t  -seed 1761992852 "
if args.extend is not None:
    command += f" {args.extend}"
os.system(command)

shutil.copyfile("/workspace/TRIAL_SRAM/verilog/dv/cocotb/sim/test_fixed_sram0/RTL-sram0_basic_test/rerun.py", "/workspace/TRIAL_SRAM/verilog/dv/cocotb/sim/test_fixed_sram0/RTL-sram0_basic_test/rerun/RTL-sram0_basic_test/rerun.py")
