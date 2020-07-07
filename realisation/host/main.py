
import os, time, math
import font
import cv2
import numpy as np

STRIP_N = 8
LED_N = 256
SIZE = round(STRIP_N * LED_N * 3)

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

def set_at_index(buf, index, value):
    
    i = math.floor(index/2)
    val = value & 0xf0
    if (index % 2) == 0:
        buf[i] &= 0x0f
        buf[i] |= val
    else:
        buf[i] &= 0xf0
        buf[i] |= val >> 4
    '''
    buf[int(index)] = value
    '''

def set_pixel(buf, strips, pixel, color):
    r, g, b = color
    for strip in strips:
        if pixel >= LED_N or strip >= STRIP_N:
            print("wrong strip (" + str(strip) + ") or pixel (" + str(pixel) + ")")
            exit()
        set_at_index(buf, (strip * LED_N + pixel) * 3 + 0, g)
        set_at_index(buf, (strip * LED_N + pixel) * 3 + 1, r)
        set_at_index(buf, (strip * LED_N + pixel) * 3 + 2, b)

def set_pixel_xy(buf, strips, x, y, color):
    set_pixel(buf, strips, y * 8 + (x if y % 2 == 0 else 7 - x), color)


test = 0
pos = -1
count = 0
max_count = 100
STRIP = [0, 1, 2, 3, 4, 5, 6, 7] # 5 ??
BRIGHTNESS = 16
STRIP_2 = [0]
t0 = time.time()

colors = [
    (BRIGHTNESS, 0, 0),
    (0, BRIGHTNESS, 0),
    (0, 0, BRIGHTNESS),
    (BRIGHTNESS, BRIGHTNESS, 0),
    (BRIGHTNESS, 0, BRIGHTNESS),
    (0, BRIGHTNESS, BRIGHTNESS),
    (BRIGHTNESS, BRIGHTNESS, BRIGHTNESS),
    (BRIGHTNESS, BRIGHTNESS, BRIGHTNESS),
]
'''
gamma8 = [
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 ]
def set_pixel_image(buffer, x, y, color):
    strip = int(x / 8)
    s_x = int(x % 8)
    s_y = y
    if strip % 2 == 1:
        s_x = 7-s_x
        s_y = 31-s_y
    set_pixel_xy(buffer, [strip], s_x, s_y, color)

vid = cv2.VideoCapture('test_2.mp4')
while True:
    ret, img = vid.read()
    if not ret:
        break
    img = cv2.resize(img, None, fx=0.5, fy=0.5, interpolation=cv2.INTER_LINEAR)
    for x in range(32):
        for y in range(32):
            b, g, r = img[x, y]
            m = 1
            b = gamma8[int(b*m)]
            g = gamma8[int(g*m)]
            r = gamma8[int(r*m)]
            set_pixel_image(buffer, y, 31-x, (r, g, b))
    update(buffer)
vid.release()


ascend = True
while True:
    for i in STRIP:
        for x in range(8):
            for y in range(8):
                c = colors[i] if font.font_is_on(str(i), x, y) else (0, 0, 0)
                set_pixel_xy(buffer, [i], x, y+pos, c)
    update(buffer)
    for i in STRIP:
        for x in range(8):
            for y in range(8):
                set_pixel_xy(buffer, [i], x, y+pos, (0, 0, 0))
    pos += 1 if ascend else -1
    if pos > LED_N/8 - 8:
        pos -= 1
        ascend = False
    if pos < 0:
        pos += 1
        ascend = True
    time.sleep(0.02)
'''
'''
buffer_r = bytearray(SIZE)
buffer_g = bytearray(SIZE)
for i in range(LED_N):
    set_pixel(buffer_r, STRIP, i, 16, 0, 0)
    set_pixel(buffer_g, STRIP, i, 0, 16, 0)
while True:
    update(buffer_r)
    update(buffer_g)
'''
'''
while True:
    set_pixel(buffer, STRIP, 5, 0, pos, 0)
    pos += 1
    time.sleep(0.02)
    print(pos)
    update(buffer)
'''
'''
while True:
    if pos >= 0:
        set_pixel(buffer, STRIP, pos, 0, 0, 0)
    pos = (pos+1) % LED_N
    set_pixel(buffer, STRIP, pos, 0, 16, 0)
    update(buffer)
    time.sleep(1)
    set_pixel(buffer, STRIP, pos, 16, 0, 0)
    update(buffer)
    time.sleep(1)
    set_pixel(buffer, STRIP, pos, 0, 0, 16)
    update(buffer)
    time.sleep(1)
'''

while True:
    if test % 1 == 0:
        if pos >= 0:
            for i in range(8):
                set_pixel(buffer, STRIP, pos*8+i, (0, 0, 0))
        pos = (pos+1) % (LED_N/8)
        for i in range(8):
            set_pixel(buffer, STRIP, pos*8+i, (0, BRIGHTNESS, BRIGHTNESS))

    if test % 2 == 0:
        set_pixel(buffer, STRIP_2, 10, (BRIGHTNESS, 0, 0))
    else:
        set_pixel(buffer, STRIP_2, 10, (0, 0, 0))
    test += 1

    update(buffer)
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
