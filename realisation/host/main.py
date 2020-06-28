
import os, time

SIZE = 6*1024

buffer = bytearray(SIZE)

for i in range(SIZE):
    buffer[i] = 255

buffer_g = bytearray(SIZE)
for i in range(SIZE):
    if i % 24 == 4:
        buffer_g[i] = 0
    else:
        buffer_g[i] = 255

buffer_r = bytearray(SIZE)
for i in range(SIZE):
    if i % 24 == 12:
        buffer_r[i] = 0
    else:
        buffer_r[i] = 255

buffer_b = bytearray(SIZE)
for i in range(SIZE):
    if i % 24 == 20:
        buffer_b[i] = 0
    else:
        buffer_b[i] = 255

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

test = 0
pos = 20
count = 0
max_count = 100
t0 = time.time()


while True:
    print('g')
    update(buffer_g)
    time.sleep(.5)
    print('r')
    update(buffer_r)
    time.sleep(.5)
    print('b')
    update(buffer_b)
    time.sleep(.5)
    print('none')
    update(buffer)
    time.sleep(.5)
    count += 1
    if count % max_count == 0:
        print(count)
        t1 = time.time()
        print(4*max_count/(t1 - t0))
        t0 = t1


'''
while True:
    buffer[pos] = 255
    pos = (pos+1) % SIZE
    buffer[pos] = 0
    update(buffer)
    count += 1
    if count % max_count == 0:
        print(count)
        t1 = time.time()
        print(max_count/(t1 - t0))
        t0 = t1
    #time.sleep(0.01)
'''

while True:
    if test % 30 == 0:
        buffer[pos] = 255
        pos = (pos+8) % (SIZE)
        buffer[pos] = 0
    if test % 2 == 0:
        buffer[13] = 0
    else:
        buffer[13] = 255
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
