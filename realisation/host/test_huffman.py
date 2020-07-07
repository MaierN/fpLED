
import os, time, math, queue

STRIP_N = 8
LED_N = 256
SIZE = 512 # round(STRIP_N * LED_N * 3)

if SIZE > 6*1024:
    print("WRONG SIZE")
print(SIZE)

buffer = bytearray(SIZE)

for i in range(len(buffer)):
    buffer[i] = 0

control = bytearray(4)
control[0] = 0

def update(buf):
    with open('../test/data', 'r+b') as f:
        f.write(buf)
    os.sync()
    with open('../test/control', 'r+b') as f:
        control[0] = (control[0] + 1) % 256
        control[1] = 1
        control[2] = 0
        control[3] = 1
        f.write(control)
    os.sync()



def get_huffman_code(raw_data, dict_size):
    probability = []
    for symbol in range(dict_size):
        probability.append({'count': 0, 'symbol': symbol})

    for symbol in raw_data:
        probability[symbol]['count'] += 1

    priority_queue = queue.PriorityQueue()
    for symbol in probability:
        priority_queue.put((symbol['count'], [symbol['symbol']]))

    huffman_code = [b''] * dict_size
    while priority_queue.qsize() > 1:
        elt_1 = priority_queue.get()
        for elt in elt_1[1]:
            huffman_code[elt] = b'1' + huffman_code[elt]

        elt_0 = priority_queue.get()
        for elt in elt_0[1]:
            huffman_code[elt] = b'0' + huffman_code[elt]

        priority_queue.put((elt_1[0] + elt_0[0], elt_1[1] + elt_0[1]))

    return huffman_code

def get_canonical_huffman_code(huffman_code):
    # for each code size, count number of symbols with this code size
    size_counts = [0] * len(huffman_code)
    for symbol in range(len(huffman_code)):
        size = len(huffman_code[symbol])
        size_counts[size] += 1
    
    # american flag sort: count number of elements in each bucket
    bucket_indexes = []
    acc = 0
    for i in range(len(size_counts)):
        bucket_indexes.append(acc)
        acc += size_counts[i]
    # american flag sort: sort symbols by code size
    sorted_symbols = [None] * len(huffman_code)
    for symbol in range(len(huffman_code)):
        sorted_symbols[bucket_indexes[len(huffman_code[symbol])]] = symbol
        bucket_indexes[len(huffman_code[symbol])] += 1

    # compute each symbols' code
    canonical_code = {}
    index = 0
    first_code_on_row = 0
    for row in range(len(size_counts)):
        first_code_on_row <<= 1
        for i in range(size_counts[row]):
            canonical_code[sorted_symbols[index]] = (first_code_on_row, row)
            index += 1
            first_code_on_row += 1

    return canonical_code, size_counts, sorted_symbols

def get_bit(buffer, bit_index):
    return 1 if buffer[int(bit_index / 8)] & (1 << int(bit_index % 8)) else 0

def set_bit(buffer, bit_index, set):
    if set:
        buffer[int(bit_index / 8)] |= 1 << int(bit_index % 8)
    else:
        buffer[int(bit_index / 8)] &= ~(1 << int(bit_index % 8))

def print_buffer(buffer, size):
    s = ''
    for i in range(size):
        for j in range(8):
            s += str((buffer[i] >> j) & 1) + ' '
        s += '\n'
    print(s)

def add_to_res(res, index, code):
    for i in range(code[1]):
        bit = (code[0] >> (code[1] - 1 - i)) & 1
        set_bit(res, index, bit)
        index += 1

def encode_data(canonical_code, raw_data, res):
    index = 0
    for symbol in raw_data:
        add_to_res(res, index, canonical_code[symbol])
        index += canonical_code[symbol][1]
    return index

def decode_data(size_counts, sorted_symbols, encoded_data, length):
    res = b''

    index = 0

    while index < length:
        row = 1

        first_index_on_row = 0
        first_code_on_row = 0
        accumulator = 0
        while True:

            accumulator = (accumulator << 1) | get_bit(encoded_data, index)
            index += 1
            first_code_on_row <<= 1

            if accumulator - first_code_on_row < size_counts[row]:
                res += bytes([sorted_symbols[accumulator - first_code_on_row + first_index_on_row]])
                break
            
            first_code_on_row += size_counts[row]
            first_index_on_row += size_counts[row]

            row += 1

    return res

raw_data = b'abcdefghijklmndddddddddddddddopqrstuvwxyz'
raw_data = b''
for i in range(256):
    raw_data += bytes([i])
raw_data += bytes([0, 0])

huffman_code = get_huffman_code(raw_data, 256)
#print(huffman_code)
canonical_code, size_counts, sorted_symbols = get_canonical_huffman_code(huffman_code)
#print(canonical_code)
encoded_length = encode_data(canonical_code, raw_data, buffer)
#print_buffer(buffer, 32)

res = decode_data(size_counts, sorted_symbols, buffer, encoded_length)
print(res)
print(len(res) * 8)
print(encoded_length)
print(size_counts)
print(sorted_symbols)

def set_at_index(buf, index, value):
    buf[int(index)] = value

def set_pixel(buf, strips, pixel, color):
    r, g, b = color
    for strip in strips:
        if pixel >= LED_N or strip >= STRIP_N:
            print("wrong strip (" + str(strip) + ") or pixel (" + str(pixel) + ")")
            exit()
        set_at_index(buf, (strip * LED_N + pixel) * 3 + 0, g)
        set_at_index(buf, (strip * LED_N + pixel) * 3 + 1, r)
        set_at_index(buf, (strip * LED_N + pixel) * 3 + 2, b)

raw_data = bytearray(512)
set_pixel(raw_data, [0], 0, (255, 255, 255))
set_pixel(raw_data, [0], 5, (255, 0, 0))
set_pixel(raw_data, [0], 12, (0, 255, 0))
set_pixel(raw_data, [0], 13, (0, 0, 1))

huffman_code = get_huffman_code(raw_data, 256)
canonical_code, size_counts, sorted_symbols = get_canonical_huffman_code(huffman_code)
encoded_length = encode_data(canonical_code, raw_data, buffer)

res = decode_data(size_counts, sorted_symbols, buffer, encoded_length)
print('=========')
print(len(res) * 8)
print(encoded_length)

with open('../test/data', 'r+b') as f:
    f.write(buffer)
with open('../test/coding', 'r+b') as f:
    f.write(bytes(size_counts))
    f.write(bytes(sorted_symbols))
os.sync()
with open('../test/control', 'r+b') as f:
    control[0] = (control[0] + 1) % 256
    control[1] = 0
    control[2] = 0
    control[3] = 1
    f.write(control)
os.sync()
with open('../test/control', 'r+b') as f:
    control[0] = (control[0] + 1) % 256
    control[1] = 0
    control[2] = 0
    control[3] = 1
    f.write(control)
os.sync()
with open('../test/control', 'r+b') as f:
    control[0] = (control[0] + 1) % 256
    control[1] = 0
    control[2] = 0
    control[3] = 1
    f.write(control)
os.sync()
