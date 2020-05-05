import os
import re
import subprocess
import sys
import tempfile

print("""
##############################################
## Checking code in the slides will compile ##
##############################################
""")

assert len(sys.argv) == 2, "Error: expected C++ compiler as argument to this script"
compiler = sys.argv[1]
print('Using {} as C++ compiler, with version:'.format(compiler))
subprocess.call([compiler, '--version'])

script_dir = os.path.realpath(os.path.dirname(__file__))
slides_md = os.path.join(script_dir, 'ModernCppCourse.md')
assert os.path.isfile(slides_md), 'Expected slides at {}'.format(slides_md)

code_pre = """
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <execution>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

namespace fs = std::filesystem;

int main()
{
"""

code_post = """
return 0;
}
"""

gcc_flags = '-std=c++2a -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-but-set-variable -Wno-sign-compare'


def compile_fragment(fragment, comp=compiler, pre=code_pre, post=code_post, flags=gcc_flags):
    """
    Compile a fragment of code and return False if there were warnings/errors
    :param fragment: the fragment of code to compile
    :param comp: path to the compiler
    :param pre: the code before the fragment
    :param post: the code after the fragment
    :param flags: compiler flags to use
    :return: False if compiler warnings/errors, else True
    """

    with tempfile.NamedTemporaryFile(mode='w+') as temp_file:
        temp_file.write(f'{pre}\n{fragment}\n{post}')
        temp_file.flush()
        # print(subprocess.check_output(f'cat {temp_file.name}', shell=True, stderr=subprocess.STDOUT))

        try:
            output = subprocess.check_output(
                f'cat {temp_file.name} | {comp} -c -xc++ {flags} -', shell=True, stderr=subprocess.STDOUT
            ).decode().strip()
        except subprocess.CalledProcessError as e:
            output = e.output.decode().strip()
            pass

        print(f'\n##### Compiling {"#" * 40}\n~~~\n{fragment}\n~~~')

        if output.strip() == '':
            print(f'##### Fine! {"#" * 44}\n')
            return True
        else:
            print(f'##### Errors! {"#" * 72}')
            print(output)
            print(f'{"#" * 56}\n')
            return False


print('Found slides at {}'.format(slides_md))

with open(slides_md, 'r') as slides:

    slides_str = slides.read()

    num_success = 0
    num_errors = 0
    num_unchecked = 0

    re_checked = re.compile(r'\n\s*~~~cpp\s+([\s\S]+?)\s+~~~\s*\n')
    for m in re.finditer(pattern=re_checked, string=slides_str):
        success = compile_fragment(m.group(1))

        if success:
            num_success += 1
        else:
            num_errors += 1

    re_unchecked = re.compile(r'~~~\s+cpp')
    for m in re.finditer(pattern=re_unchecked, string=slides_str):
        num_unchecked += 1

    print(f'\nThere were {num_errors} bad code blocks and {num_success} good code blocks.')
    print(f'There were {num_unchecked} unchecked code blocks.\n')
