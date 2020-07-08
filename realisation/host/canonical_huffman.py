
import queue

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
    same_code_length_flag = 0
    # for each code size, count number of symbols with this code size
    size_counts = [0] * len(huffman_code)
    for symbol in range(len(huffman_code)):
        size = len(huffman_code[symbol])
        size_counts[size] += 1
        if size_counts[size] > 255:
            same_code_length_flag = 1
    
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

    if same_code_length_flag:
        for i in range(len(size_counts)):
            size_counts[i] = 0
        size_counts[0] = 1
    return canonical_code, size_counts, sorted_symbols

def get_bit(buffer, bit_index):
    return 1 if buffer[int(bit_index / 8)] & (1 << int(bit_index % 8)) else 0

def set_bit(buffer, bit_index, set):
    if set:
        buffer[int(bit_index / 8)] |= 1 << int(bit_index % 8)
    else:
        buffer[int(bit_index / 8)] &= ~(1 << int(bit_index % 8))

def add_to_res(encoded_data, index, code):
    for i in range(code[1]):
        bit = (code[0] >> (code[1] - 1 - i)) & 1
        set_bit(encoded_data, index, bit)
        index += 1

def encode_data(canonical_code, raw_data, offset, encoded_data):
    index = 0
    count = 0
    for i in range(len(raw_data) - offset):
        symbol = raw_data[i + offset]
        code = canonical_code[symbol]
        if code[1] + index >= len(encoded_data)*8:
            break
        count += 1
        for i in range(code[1]):
            bit = (code[0] >> (code[1] - 1 - i)) & 1
            set_bit(encoded_data, index, bit)
            index += 1
    return count

def decode_data(size_counts, sorted_symbols, encoded_data, length):
    res = b''

    index = 0

    while len(res) < length:
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
