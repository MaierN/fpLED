
import os, time

STRIP_N = 8
LED_N = 256
SIZE = STRIP_N * LED_N * 3

if SIZE > 6*1024:
    print("WONG SIZE")

buffer = bytearray(SIZE)

for i in range(SIZE):
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

def set_pixel(strip, pixel, r, g, b):
    buffer[(strip * LED_N + pixel) * 3 + 0] = g
    buffer[(strip * LED_N + pixel) * 3 + 1] = r
    buffer[(strip * LED_N + pixel) * 3 + 2] = b

test = 0
pos = -1
count = 0
max_count = 100
t0 = time.time()

while True:
    if test % 5 == 0:
        if pos >= 0:
            set_pixel(0, pos, 0, 0, 0)
        pos = (pos+1) % LED_N
        set_pixel(0, pos, 0, 255, 0)

    if test % 2 == 0:
        set_pixel(0, 0, 1, 0, 0)
    else:
        set_pixel(0, 0, 0, 0, 0)
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
