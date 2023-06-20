import subprocess
import re
import serial
import time
import math
import numpy as np

def convert_to_meters(x,y, dist_to_capture, hfov, vfov, img_widht, img_height):
    amp_hor_meters = 2*dist_to_capture*math.tan(math.radians(hfov/2))
    amp_vert_meters = 2*dist_to_capture*math.tan(math.radians(vfov/2))
   
    x_meters = x*amp_hor_meters/img_widht
    y_meters = y*amp_vert_meters/img_height

    return x_meters, y_meters

def coordinate_system(x,y,z,hfov, vfov):
    x_m, y_m = convert_to_meters(x,y, -z, hfov, vfov, 640, 640)
    x_c, y_c = convert_to_meters(320,320, -z, hfov, vfov, 640, 640)
    angle_pan =  math.degrees(math.atan2((x_c-x_m)-0.03,z))
    tilt_angle = -math.degrees(math.atan2((y_c-y_m)+0.18,z))  
    return angle_pan, tilt_angle


def main(flag):
    try:
        proc = subprocess.Popen('build/./inf', stdout=subprocess.PIPE)
        pattern = r"Detection: ([\d\.]+),([\d\.]+),([\d\.]+),([\d\.]+),([\d\.]+)"

        # Set the time interval between sending coordinates
        interval = 1  # in milliseconds
        last_sent_time = time.monotonic() * 1000  # convert to milliseconds

        # Keep track of sent IDs
        sent_ids = set()

        # Process the output line by line
        for line in proc.stdout:
            # Apply the regular expression to the line and extract the x and y coordinates
            match = re.search(pattern, line.decode())

            if match:
                x, y, Lx, Ly, id_number = map(float, match.groups())

                # Check if the ID has already been sent
                if id_number not in sent_ids:
                    x3, y3 = coordinate_system(x, y, 1, 34, 34)

                    # Send the coordinates to the Arduino as a string in the format "x,y\n"
                    ser.write(f"{x3:.5f},{y3:.5f},{Lx},{Ly},{id_number},{flag}\n".encode())

                    print(f"{x3:.5f},{y3:.5f},{Lx},{Ly},{id_number},{flag}\n".encode())

                    # Update the last sent time and add the ID to the set of sent IDs
                    last_sent_time = time.monotonic() * 1000
                    sent_ids.add(id_number)

            # Check if enough time has passed since the last send
            if (time.monotonic() * 1000) - last_sent_time >= interval:
                # Update the last sent time without sending any coordinates
                last_sent_time = time.monotonic() * 1000

        # Close the serial connection and the process
        ser.write(b"stop\n")
        flag = -1
        print("Sent stop command to Arduino")

        ser.close()
        proc.kill()

    except KeyboardInterrupt:
        # Catch keyboard interrupt and gracefully stop the process
        print("Keyboard interrupt detected. Stopping the script.")

        # Send stop command to the Arduino
        ser.write(b"stop\n")
        flag = -1
        print("Sent stop command to Arduino")

        # Close the serial connection and the process
        ser.close()
        proc.kill()

if __name__ == "__main__":
    # Open a serial connection to the Arduino
    ser = serial.Serial('/dev/ttyACM0', 9600)
    # Add a delay to allow Arduino to initialize
    time.sleep(2)  # Adjust the delay time as needed

    ser.write(b"stop\n")
    time.sleep(1)
    ser.write(b"start\n")
    print("Start")
    flag = 1

    time.sleep(4)  # Adjust the delay time as needed
    main(flag)
