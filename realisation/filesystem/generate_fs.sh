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
# Project: HEIA-FR / Accélérateur de tour télécom
#
# Purpose: This script generates a FAT12 filesystem and prints a C array containing the filsystem's metadata
#
# Author:  Nicolas Maier
# Date:    June 2020

mformat -v "TELE TOWER" -t 1 -h 1 -s 45 -S 2 -C -i fs.img -c 1 -r 1 -L 1

dd if=/dev/zero of=control bs=512 count=1
dd if=/dev/zero of=coding bs=512 count=1
dd if=/dev/zero of=data bs=19968 count=1

mcopy -i fs.img control ::/
mcopy -i fs.img coding ::/
mcopy -i fs.img data ::/

python3 print_data.py

rm fs.img
rm control
rm coding
rm data
