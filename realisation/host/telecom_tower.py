
import time
import os
import struct
import canonical_huffman

class TelecomTower():
    def __init__(self, path):
        self._path = path
        self._encoded_buffer = bytearray(512 - 5)

    def send(self, data):
        test_fff = bytearray(256)
        for i in range(256):
            test_fff[i] = i
        huffman_code = canonical_huffman.get_huffman_code(test_fff, 256)
        canonical_code, size_counts, sorted_symbols = canonical_huffman.get_canonical_huffman_code(huffman_code)
        
        with open(self._path + '/coding', 'r+b') as f:
            f.write(bytes(size_counts))
            f.write(bytes(sorted_symbols))
        os.sync()

        test = 0
        test_data = []

        offset = 0
        while offset < len(data):
            encoded_size = canonical_huffman.encode_data(canonical_code, data, offset, self._encoded_buffer)
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
