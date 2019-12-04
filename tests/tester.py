import os
from subprocess import call

def get_return_code(file):
    """Get return code from filename xx_filename"""
    return int(file.split("_")[0])


def run_test(executable, file):
    """Run test and return exit code"""
    return call("./{} < {} > output".format(executable, file), shell=True)

def __main__():
    tests = os.path.realpath("tests/input/")  	 # directory with tests
    arr = os.listdir(tests)   			 # file with tests
    src = os.path.realpath("ifj19")		 # source file
    executable = "ifj19"                   	 # executable file
    interpret = "tests/ic19int"
    
    for file in arr:
        return_code = get_return_code(file)  	# return code from test file
        error_code = run_test(executable, tests + '/' + file) 
        if error_code != return_code:
            print("{} \texpected {} but returns {}".format(file, return_code, error_code))
        else:
            if return_code == 0:
                ret = call("./{} output".format(interpret), shell=True)
                if ret:
                    print("{} \texpected {} but returns {}".format(file, error_code, ret))

__main__()
