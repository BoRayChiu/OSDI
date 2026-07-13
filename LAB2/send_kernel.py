import serial
import os
import time

#SERIAL_PORT = '/dev/pts/11'
SERIAL_PORT = 'COM10'
BAUD_RATE = 115200
KERNEL_PATH = './Kernel/kernel8.img'

def send_kernel():
    kernel_size = os.path.getsize(KERNEL_PATH)
    
    with open(KERNEL_PATH, "rb") as f:
        kernel_data = f.read()

    print(f"Kernel size: {kernel_size} bytes")
    print(f"Opening {SERIAL_PORT} at {BAUD_RATE} baud...")

    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            ser.write(kernel_size.to_bytes(4, byteorder='little'))
            print("Size sent! Waiting a bit...")
            time.sleep(0.5)
            
            print("Sending kernel data...")
            ser.write(kernel_data)
            print("Transfer complete! OS should be booting now.")
                    
    except Exception as e:
        print(f"Error: {e}")

if __name__ == '__main__':
    send_kernel()