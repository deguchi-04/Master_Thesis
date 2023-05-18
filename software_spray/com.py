import subprocess
import re
import serial
import time

# Open a serial connection to the Arduino
ser = serial.Serial('/dev/ttyACM0', 9600)

# Execute the test.cpp executable and capture its output
proc = subprocess.Popen('build/./inf', stdout=subprocess.PIPE)
pattern = r"Detection: (\d+),(\d+),(\d+),(\d+),(\d+)"

# Set the time interval between sending coordinates
interval = 0.2 # in seconds
last_sent_time = time.monotonic() 

# Process the output line by line
for line in proc.stdout:
    # Check if enough time has passed since the last send
   
    if time.monotonic() - last_sent_time >= interval:
        # Apply the regular expression to the line and extract the x and y coordinates
        match = re.search(pattern, line.decode())
        print(match)
        if match:
            x, y, Lx, Ly, id_number = map(float, match.groups())
            
            # Send the coordinates to the Arduino as a string in the format "x,y\n"
            ser.write(f"{x},{y},{Lx},{Ly},{id_number}\n".encode())

            # Update the last sent time
            last_sent_time = time.monotonic()


# Close the serial connection and the process
ser.close()
proc.kill()
