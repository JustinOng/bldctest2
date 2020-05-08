import sys
import serial
import struct
import threading
from collections import deque
import numpy as np
from matplotlib import pyplot as plt

MAX_BUFF_SIZE = 1000
VARS = [(2, 'H'), (2, 'H')]

# how many bytes are there in one packet
PACKET_SIZE = 6
# how many packets to read at a time
PACKETS_TO_READ = 128

chunk_size = PACKET_SIZE * PACKETS_TO_READ

data = [np.zeros(1) for i in range(len(VARS))]

def read():
    global data

    with serial.Serial("COM3", 115200, timeout=None) as ser:
        print("Port open")

        # dump the first line because likely to be cut-off data
        while ser.is_open:
            ser.readline()
            chunk = ser.read(chunk_size)

            offset = 0
            for i, (var_len, var_type) in enumerate(VARS):
                var_data = np.ndarray((PACKETS_TO_READ, ), var_type, chunk, offset, (PACKET_SIZE,))
                offset += var_len

                data[i] = np.append(data[i], var_data)[-MAX_BUFF_SIZE:]
            
            #print(ser.in_waiting)

t = threading.Thread(target=read, daemon=True)
t.start()

f, axes = plt.subplots(len(VARS), 1)

lines = []
for i in range(len(VARS)):
    line, = axes[i].plot(data[i])
    lines.append(line)
    axes[i].autoscale()

plt.ion()
plt.show()

while True:
    for i in range(len(VARS)):
        lines[i].set_data(np.arange(len(data[i])), data[i])
        axes[i].set_xlim(0, len(data[i]))
        axes[i].set_ylim(min(data[i]), max(data[i]))
        #print(f'{i}: {min(data[i])} - {max(data[i])}')
    
    plt.pause(0.05)
