# Copyright (c) 2020 University of Applied Sciences Western Switzerland / Fribourg
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#
# Project: HEIA-FR / Fast ws281x LED control in parallel via USB
#
# Purpose: This script compiles the demo programs (which use the C++ driver)
#
# Author:  Nicolas Maier
# Date:    June 2020

g++ -pthread demo_benchmark.cpp -Ofast -Wall -Wextra -o demo_benchmark
g++ -pthread demo_telecom_tower_line.cpp -Ofast -Wall -Wextra -o demo_telecom_tower_line
g++ -pthread demo_telecom_tower_text.cpp -Ofast -Wall -Wextra -o demo_telecom_tower_text
g++ -pthread demo_tests.cpp -Ofast -Wall -Wextra -o demo_tests
