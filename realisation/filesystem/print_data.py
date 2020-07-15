'''
 * Copyright (c) 2020 University of Applied Sciences Western Switzerland / Fribourg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 * Project: HEIA-FR / Fast ws281x LED control in parallel via USB
 *
 * Purpose: This script dumps the first 4 blocks of 512 bytes from fs.img into a C array
 *
 * Author:  Nicolas Maier
 * Date:    June 2020
'''

s = 'const uint8_t usb_filesystem_metadata[] = {\n'
cnt = 0

a = 0

with open('fs.img', 'rb') as f:
    for i in range(512 * 4):
        if a == 512:
            s += '\n'
            a = 0
            cnt = 0
        
        byte_s = f.read(1)
        if not byte_s:
            break
        byte = byte_s[0]

        if cnt == 0:
            s += '    '
        s += hex(byte) + ','
        cnt += 1
        if cnt >= 32:
            cnt = 0
            s += '\n'
        else:
            s += ' '

        a += 1

s += '};'

print(s)
