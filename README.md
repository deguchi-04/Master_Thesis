# M.EEC Master Thesis
### Vision-based Smart Sprayer for Precision Farming

###### Version 1.0

Precision Sprayer Software Development 

## :dart: Objective

The main objective was to develop a perception algorithm to identify and segment grapes in a vineyard. After the grape localization, the code must command the hardware to point the spray in the grape direction. The hardware will also point a laser to the fruit. The code must segment the laser color from the image in order to validate the grape position. After all these steps, the hardware must perform the spray action only in the grapes. Development in partnership with INESC-TEC.

## :gear: Hardware
- OAK-1 Camera
- INESC TEC Pixel Driver 3
- NEMA 17 Stepper Motors
- Designed Structure 3D Printed

## :book: Requirements
- Python (3.10.6 minimum)
- OpenCV (4.x minimum)
- CMake (3.5 minimum)
- [Depthai C++](https://github.com/luxonis/depthai-core)

## :computer: Code
 #### [Sprayer Hardware](https://gitlab.inesctec.pt/agrob/UltraPrecisionSprayer/-/tree/main/hardware_spray)

The **Homing_function. in** file was developed to test the motors and driver sensors. In the serial monitor it is expected an angle value to the pan movement and another to the tilt movement, e.g

    90,90

In this situation, the pan and tilt motors must perform a 90º movement.

The **SerialCom.ino** file reads the information sent by the python script and use as input to the mechanism. The mechanism must aim in the grape direction

 #### [Sprayer Software](https://gitlab.inesctec.pt/agrob/UltraPrecisionSprayer/-/tree/main/software_spray)
The Inference and Segmentation code is present in this folder.
It is necessary to execute the **com.py** file in the command line to run the inference:
    
    python3 com.py

This script executes the grape recognition software and communicates with the hardware through the serial port. The trained model can be found [here](https://gitlab.inesctec.pt/agrob/UltraPrecisionSprayer/-/tree/main/software_spray/src/files/yolo8n_model_best). There is one model to video capture of 640x640 pixels and the same model but to video captures of 512x512.

To modify the **cnn_with_depthai.cpp** file to change the used model segmentation mask values, etc., it is necessary to compile the code (make) inside the **build** folder.

    cmake ..
    make
    ./inf

The **segmentation.cpp** code was used to find the best mask possible to segment the red laser with the aid of trackbars.

    ./seg

## :movie_camera: Video Demo

**por o video aqui

## :pencil2: Authors

- [@deguchi-04](https://github.com/deguchi-04)


## 🚀 About Me
Currently enrolled in the Master in Electrotechnical Engineering and Computers course at Faculty of Engineering of U.Porto. My main specializing area is Computer Vision and Machine Learning. Developing the master thesis at INESC TEC.

