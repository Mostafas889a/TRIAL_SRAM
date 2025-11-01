from caravel_cocotb.tests.common_functions.test_functions import *
from caravel_cocotb.tests.housekeeping.housekeeping import *

def register_tests(dut):
    tests = []
    
    tests.append(('comprehensive_address_test', 'comprehensive_address_test/comprehensive_address_test.py', 'comprehensive_address_test/comprehensive_address_test.c', {'sim': ['RTL', 'GL', 'GL_SDF']}))
    tests.append(('access_type_test', 'access_type_test/access_type_test.py', 'access_type_test/access_type_test.c', {'sim': ['RTL', 'GL', 'GL_SDF']}))
    tests.append(('multi_bank_test', 'multi_bank_test/multi_bank_test.py', 'multi_bank_test/multi_bank_test.c', {'sim': ['RTL', 'GL', 'GL_SDF']}))
    
    return tests
