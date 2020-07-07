
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

decoded_data = b'ababbabonjouraa'
table = {}
for i in range(256):
    a = bytearray(1)
    a[0] = i
    table[i] = a

decoded_index = 0
encoded_index = 0
table_index = 0

while decoded_index < len(decoded_data):
    symbol_length = 0
    symbol_table_index = -1
    while decoded_index+symbol_length+1 < len(decoded_data) and decoded_data[decoded_index:decoded_index+symbol_length+1] in table:
        symbol_table_index = list(table.keys())[list(table.values()).index(decoded_data[decoded_index:decoded_index+symbol_length+1])]
        symbol_length += 1

    buffer[encoded_index] = symbol_table_index - 256
    encoded_index += 1

    if decoded_index + symbol_length < len(decoded_data):
        table[table_index] = decoded_data[decoded_index:decoded_index+symbol_length+1]
        table_index += 1

        decoded_index += symbol_length

        buffer[encoded_index] = decoded_data[decoded_index]
        decoded_index += 1
    else:
        break

print(decoded_index)
print(encoded_index)
print(table_index)

print(decoded_data)
print(table)
print(buffer[0:20])

#update(buffer)
