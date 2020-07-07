
import os, time, math

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

def get_bit(buffer, bit_index):
    if bit_index < 2:
        return bit_index
    bit_index -= 2
    return 1 if buffer[int(bit_index / 8)] & (1 << int(bit_index % 8)) else 0

def set_bit(buffer, bit_index, set):
    if set:
        buffer[int(bit_index / 8)] |= 1 << int(bit_index % 8)
    else:
        buffer[int(bit_index / 8)] &= ~(1 << int(bit_index % 8))

def compare_bits(buffer, bit_index_1, bit_index_2, length):
    for i in range(length):
        if get_bit(buffer, bit_index_1 + i) != get_bit(buffer, bit_index_2 + i):
            return False
    return True

def copy_bits(src, dst, bit_index_src, bit_index_dst, length):
    for i in range(length):
        set_bit(dst, bit_index_dst, get_bit(src, bit_index_src))

def print_buffer(buffer, size):
    s = ''
    for i in range(size):
        for j in range(8):
            s += str((buffer[i] >> j) & 1) + ' '
        s += '\n'
    print(s)

def print_buffer_test(buffer, size):
    for i in range(size):
        for j in range(8):
            print('set_bit(encoded_data_a, index++, ' + str((buffer[i] >> j) & 1) + ');')

def write_code(code, length, buffer, index):
    for i in range(length):
        set_bit(buffer, index + i, (code >> (length - 1 - i)) & 1)

decoded_data = bytearray(8)
dec_index = 0
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 0); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1
set_bit(decoded_data, dec_index, 1); dec_index += 1

table = {
    0: (0, 1),
    1: (1, 1),
}
table_index = 2
current_variable_length = 2

encoded_index = 0
decoded_length = 32

index = 0
while index < decoded_length:
    max_length = 0
    found_index = -1

    found = True
    while found:
        new_max_length = max_length + 1
        if index + new_max_length > decoded_length:
            break
        found = False
        for key in table:
            if new_max_length == table[key][1] and compare_bits(decoded_data, table[key][0], index+2, new_max_length):
                found = True
                max_length = new_max_length
                found_index = key
                break

    table[table_index] = (index+2, max_length + 1)
    if table_index >= 1+(1 << current_variable_length):
        current_variable_length += 1
    table_index += 1

    write_code(found_index, current_variable_length, buffer, encoded_index)
    encoded_index += current_variable_length
    index += max_length
    print('-- ' + str(current_variable_length))

    print(str(found_index) + ' ' + str(max_length) + ' ' + str(table[found_index]))

print(index)
print(table)
print_buffer_test(buffer, 8)
