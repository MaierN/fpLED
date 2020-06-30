
import os, time, math

STRIP_N = 8
LED_N = 256
SIZE = round(STRIP_N * LED_N * 3 / 2)

if SIZE > 3*1024:
    print("WRONG SIZE")
print(SIZE)

buffer = bytearray(SIZE)

for i in range(len(buffer)):
    buffer[i] = 0

write_indicator = bytearray(1)
write_indicator[0] = 0

def update(buf):
    with open('../test/testfile.txt', 'r+b') as f:
        f.seek(512)
        f.write(buf)
    os.sync()
    with open('../test/testfile.txt', 'r+b') as f:
        f.seek(0)
        write_indicator[0] = (write_indicator[0] + 1) % 256
        f.write(write_indicator)
    os.sync()

def set_at_index(buf, index, value):
    i = math.floor(index/2)
    val = value & 0xf0
    if (index % 2) == 0:
        buf[i] &= 0x0f
        buf[i] |= val
    else:
        buf[i] &= 0xf0
        buf[i] |= val >> 4

def set_pixel(buf, strip, pixel, r, g, b):
    if pixel >= LED_N or strip >= STRIP_N:
        print("wrong strip or pixel")
        exit()
    set_at_index(buf, (strip * LED_N + pixel) * 3 + 0, g)
    set_at_index(buf, (strip * LED_N + pixel) * 3 + 1, r)
    set_at_index(buf, (strip * LED_N + pixel) * 3 + 2, b)

test = 0
pos = -1
count = 0
max_count = 100
STRIP = 0 # 5 ??
STRIP_2 = 7
t0 = time.time()

'''
buffer_r = bytearray(SIZE)
buffer_g = bytearray(SIZE)
for i in range(LED_N):
    set_pixel(buffer_r, STRIP, i, 16, 0, 0)
    set_pixel(buffer_g, STRIP, i, 0, 16, 0)
while True:
    update(buffer_r)
    update(buffer_g)
'''
'''
while True:
    set_pixel(buffer, STRIP, 5, 0, pos, 0)
    pos += 1
    time.sleep(0.02)
    print(pos)
    update(buffer)
'''
'''
while True:
    if pos >= 0:
        set_pixel(buffer, STRIP, pos, 0, 0, 0)
    pos = (pos+1) % LED_N
    set_pixel(buffer, STRIP, pos, 0, 16, 0)
    update(buffer)
    time.sleep(1)
    set_pixel(buffer, STRIP, pos, 16, 0, 0)
    update(buffer)
    time.sleep(1)
    set_pixel(buffer, STRIP, pos, 0, 0, 16)
    update(buffer)
    time.sleep(1)
'''

while True:
    if test % 10 == 0:
        if pos >= 0:
            for i in range(8):
                set_pixel(buffer, STRIP, pos*8+i, 0, 0, 0)
        pos = (pos+1) % (LED_N/8)
        for i in range(8):
            set_pixel(buffer, STRIP, pos*8+i, 0, 16, 16)

    if test % 2 == 0:
        set_pixel(buffer, STRIP_2, 10, 16, 0, 0)
    else:
        set_pixel(buffer, STRIP_2, 10, 0, 0, 0)
    test += 1

    update(buffer)
    #time.sleep(0.2)

    count += 1

    if count % max_count == 0:
        t1 = time.time()
        fps = max_count/(t1 - t0)
        if fps < 10:
            print('-------------------')
            print('-------------------')
        print(fps)
        t0 = t1
