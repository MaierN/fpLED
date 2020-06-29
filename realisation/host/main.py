
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

def set_at_index(index, value):
    i = math.floor(index/2)
    val = value & 0xf0
    if (index % 2) == 0:
        buffer[i] &= 0x0f
        buffer[i] |= val
    else:
        buffer[i] &= 0xf0
        buffer[i] |= val >> 4

def set_pixel(strip, pixel, r, g, b):
    if pixel >= LED_N or strip >= STRIP_N:
        print("wrong strip or pixel")
        exit()
    set_at_index((strip * LED_N + pixel) * 3 + 0, g)
    set_at_index((strip * LED_N + pixel) * 3 + 1, r)
    set_at_index((strip * LED_N + pixel) * 3 + 2, b)

test = 0
pos = -1
count = 0
max_count = 100
t0 = time.time()

while True:
    if test % 30 == 0:
        if pos >= 0:
            for i in range(8):
                set_pixel(0, pos*8+i, 0, 0, 0)
        pos = (pos+1) % (LED_N/8)
        for i in range(8):
            set_pixel(0, pos*8+i, 0, 16, 0)

    if test % 2 == 0:
        set_pixel(3, 10, 16, 0, 0)
    else:
        set_pixel(3, 10, 0, 0, 0)
    test += 1

    update(buffer)

    count += 1

    if count % max_count == 0:
        t1 = time.time()
        fps = max_count/(t1 - t0)
        if fps < 10:
            print('-------------------')
            print('-------------------')
        print(fps)
        t0 = t1
