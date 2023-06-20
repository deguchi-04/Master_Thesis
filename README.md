# M.EEC Master Thesis
### Vision-based Smart Sprayer for Precision Farming

###### Version 1.0

Precision Sprayer Software Development 

## :dart: Objective

The main objective was to develop a perception algorithm to identify and segment grapes in a vineyard. After the grape localization, the code must command the hardware to point the spray in the grape direction. The hardware will also point a laser to the fruit. The code must segment the laser color from the image in order to validate the grape position. After all these steps, the hardware must perform the spray action only in the grapes. Development in partnership with INESC-TEC.

![graphic](https://github.com/deguchi-04/Master_Thesis/assets/78708477/61a16b9b-90f2-4a63-ad21-002ee60095e9)


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



https://github.com/deguchi-04/Master_Thesis/assets/78708477/b8a32e26-82f1-4599-a1f2-79b929bb79bc



## :pencil2: Authors

- [@deguchi-04](https://github.com/deguchi-04)

## :memo: Cite this work

     @article{article,
     author = {Deguchi, Thaidy},
     year = {2023},
     month = {06},
     pages = {104},
     title = {Master in Electrotechnical Engineering and Computers course at Faculty of Engineering of U.Porto, EN Vision-based Smart Sprayer for Precision Farming}
     abstract = {Nowadays, it is known that one of the main discussion subjects is sustainability. Climate change, limited resources, and population growth impose us to manage better our agricultural methods to reduce food waste and soil degradation. Since the emergence of agriculture, humankind started to use chemicals to fight plagues and consequently the losses in the farms, which, from the nineteenth century,  evolved into what we know as pesticides today. However, this practice could damage the soil because the product is not directly applied to the target. The precision agriculture method uses AI and image processing to control the number of chemicals used in a given type of plant, reducing the losses and maximizing resource usage. Based on the previous studies analyzed in this master thesis, it is clear that the most recent approaches were functional; however, there is potential to improve the sprayer system's precision, accuracy, and mechanical aspects. This work will compile, study, execute, and test these improvements.To achieve the objective of this dissertation work, that is, developing a functional algorithm to detect and spray pesticide only in grapes in a crop, and also a sprayer with two degrees of freedom, this work aims to do: A systematic review of the already existent algorithms today; Improvement of the previous codes, and consequently, their results and accuracy; Study and design a structure for the precision spray with two degrees of freedom; Test and validation of the work developed.}
     issn = {},
     doi = {},
     }
     
## 🚀 About Me
Currently enrolled in the Master in Electrotechnical Engineering and Computers course at Faculty of Engineering of U.Porto. My main specializing area is Computer Vision and Machine Learning. Developing the master thesis at INESC TEC.

