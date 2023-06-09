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
    
    x_m, y_m = convert_to_meters(x,y, -z, hfov,vfov, 640, 640)
    x_c, y_c = convert_to_meters(320,320, -z,hfov,vfov, 640, 640)
    angle_pan =  math.degrees(math.atan((x_c-x_m)/z))
    tilt_angle = -math.degrees(math.atan((y_c-y_m)/z))  
    cos_theta = math.cos(angle_pan)
    sin_theta = math.sin(tilt_angle)
    
    ################ COORDINATE CHANGE FROM IMAGE TO PAN ########################

    rotation_img_to_pan = np.array([[1, 0, 0],
                                    [0, cos_theta, -sin_theta],
                                    [0, sin_theta, cos_theta]])
    
    translation_img_to_pan = np.array([-0.035, 0, -1])

    transformation_matrix_img_to_pan = np.eye(4)
    transformation_matrix_img_to_pan[:3, :3] = rotation_img_to_pan
    transformation_matrix_img_to_pan[:3, 3] = translation_img_to_pan

    ################ COORDINATE CHANGE FROM PAN TO TILT ########################

    rotation_pan_to_tilt = np.array([[cos_theta, 0, sin_theta],
                                     [0, 1, 0],
                                     [-sin_theta, 0, cos_theta]])
    
    translation_pan_to_tilt = np.array([+0.20, 0, 0])

    transformation_matrix_pan_to_tilt = np.eye(4)
    transformation_matrix_pan_to_tilt[:3, :3] = rotation_pan_to_tilt
    transformation_matrix_pan_to_tilt[:3, 3] = translation_pan_to_tilt

    ################ COORDINATE CHANGE FROM TILT TO LASER ########################

    rotation_tilt_to_laser = np.eye(3)
    translation_tilt_to_laser = np.array([+0.035, 0, 0.12])

    transformation_matrix_tilt_to_laser = np.eye(4)
    transformation_matrix_tilt_to_laser[:3, :3] = rotation_tilt_to_laser
    transformation_matrix_tilt_to_laser[:3, 3] = translation_tilt_to_laser

    ################ COORDINATE CHANGE FROM IMG TO LASER ########################

    transformation_matrix_img_to_laser = np.dot(np.dot(transformation_matrix_img_to_pan,transformation_matrix_pan_to_tilt),transformation_matrix_tilt_to_laser)
    point = np.dot(transformation_matrix_img_to_laser,[x,y,0,1])
    x3 = point[0]
    y3 = point[1]
    #z =z+1
    x_m, y_m = convert_to_meters(x,y, -z, hfov, vfov, 640, 640)
    x_c, y_c = convert_to_meters(320,320, -z, hfov, vfov, 640, 640)
    angle_pan =  math.degrees(math.atan2((x_c-x_m),z))
    tilt_angle = -math.degrees(math.atan2((y_c-y_m)+0.22,z))  
    return angle_pan, tilt_angle


def main():
    # Open a serial connection to the Arduino
    ser = serial.Serial('/dev/ttyACM0', 9600)

    try:
        # Add a delay to allow Arduino to initialize
        time.sleep(2)  # Adjust the delay time as needed

        ser.write(b"start\n")
        print("Start")
        flag = 1

        time.sleep(4)  # Adjust the delay time as needed

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
                    x3, y3 = coordinate_system(x, y, 1, 34, 36)

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
    main()
