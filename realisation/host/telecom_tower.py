
import time
import os
import struct
import canonical_huffman

class TelecomTower():
    def __init__(self, path):
        self._path = path
        self._encoded_buffer = bytearray(512 - 5)

    def send(self, data):
        print('data:')
        print(data)
        huffman_code = canonical_huffman.get_huffman_code(data, 256)
        canonical_code, size_counts, sorted_symbols = canonical_huffman.get_canonical_huffman_code(huffman_code)

        print('canonical_code:')
        print(canonical_code)
        print('size_counts:')
        print(size_counts)
        print('sorted_symbols:')
        print(sorted_symbols)
        
        with open(self._path + '/coding', 'r+b') as f:
            f.write(bytes(size_counts))
            f.write(bytes(sorted_symbols))
        #os.sync()

        test = 0
        test_data = []

        offset = 0
        while offset < len(data):
            encoded_size = canonical_huffman.encode_data(canonical_code, data, offset, self._encoded_buffer)
            print('encoded_buffer:')
            print(self._encoded_buffer)
            test_data.append((struct.pack(b'HHB', offset, encoded_size, 0 if offset + encoded_size < len(data) else 1), bytearray(self._encoded_buffer)))
            with open(self._path + '/data', 'r+b') as f:
                f.write(struct.pack(b'HHB', offset, encoded_size, 0 if offset + encoded_size < len(data) else 1))
                f.write(self._encoded_buffer)
            os.sync()
            offset += encoded_size

        print(len(test_data))
        for i in range(500):
            for data in test_data:
                with open(self._path + '/data', 'r+b') as f:
                    f.write(data[0])
                    f.write(data[1])
                os.sync()

tt = TelecomTower('../test')

buffer = bytearray(256*3*5)
buffer[1] = 255

tt.send(buffer)


'''
data = b'\x00\x01\x01\x01\x01\x02\x00\x03'
test = canonical_huffman.get_huffman_code(data, 256)
print(test)
s = ''
for i in test:
    s += str(len(i)) + ' '
print(s)

print('---')
can = canonical_huffman.get_canonical_huffman_code(test)
print(can)
'''
'''
buffer = bytearray(256*3*5)

test = 0
pos = 0
count = 0
max_count = 1
t0 = time.time()
while True:
    if test % 1 == 0:
        buffer[pos] = 0x00
        pos = (pos+1) % len(buffer)
        buffer[pos] = 0xff

    if test % 2 == 0:
        buffer[10] = 0x00
    else:
        buffer[10] = 0xff
    test += 1

    tt.send(buffer)
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
'''