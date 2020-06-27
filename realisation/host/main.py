
import os, time

SIZE = 6144

buffer = bytearray(SIZE)

for i in range(SIZE):
    buffer[i] = 255

buffer_g = bytearray(SIZE)
for i in range(SIZE):
    if i % 12 == 3:
        buffer_g[i] = 0
    else:
        buffer_g[i] = 255

buffer_r = bytearray(SIZE)
for i in range(SIZE):
    if i % 12 == 7:
        buffer_r[i] = 0
    else:
        buffer_r[i] = 255

buffer_b = bytearray(SIZE)
for i in range(SIZE):
    if i % 12 == 11:
        buffer_b[i] = 0
    else:
        buffer_b[i] = 255

write_indicator = bytearray(1)
write_indicator[0] = 0
def update(buf):
    with open('../test/testfile.txt', 'r+b') as f:
        f.seek(1024)
        f.write(buf)
        os.sync()
        #f.seek(0)
        #write_indicator[0] = (write_indicator[0] + 1) % 256
        #f.write(write_indicator)
        #os.sync()

pos = 5
count = 0
max_count = 100
t0 = time.time()

'''
while True:
    update(buffer_g)
    time.sleep(0.5)
    update(buffer_r)
    time.sleep(0.5)
    update(buffer_b)
    time.sleep(0.5)
    update(buffer)
    time.sleep(0.5)
    count += 1
    if count % max_count == 0:
        print(count)
        t1 = time.time()
        print(4*max_count/(t1 - t0))
        t0 = t1
'''

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

test = 0
while True:
    if test % 10 == 0:
        buffer[pos] = 255
        pos = (pos+8) % (SIZE)
        buffer[pos] = 0
    if test % 2 == 0:
        buffer[5] = 0
    else:
        buffer[5] = 255
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
    #time.sleep(0.016)
