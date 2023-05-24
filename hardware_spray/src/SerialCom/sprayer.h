#ifndef SPRAYER_H
#define SPRAYER_H

#include <AccelStepper.h>

// Define the pin connections for the pan stepper motor
#define PAN_DIR_PIN 9
#define PAN_STEP_PIN 10
#define PAN_SLEEP_PIN 11

// Define the pin connections for the tilt stepper motor
#define TILT_DIR_PIN A0
#define TILT_STEP_PIN A1
#define TILT_SLEEP_PIN A2

// Define the pin connections for the analog hall sensors
#define PAN_HALL_SENSOR A4
#define TILT_HALL_SENSOR A5
#define HALL_THRESHOLD 465

// Define the number of steps per revolution for the stepper motors
#define STEPS_PER_REV 200

// Define gear ratios
#define panGearRatio 8.47
#define tiltGearRatio 3.05

// Define stepper motor steps per revolution
#define stepsPerRevolution 200

// Define the maximum degrees of pan and tilt
#define MAX_PAN_DEGREES 180
#define MAX_TILT_DEGREES 180

#define pi 3.1415

// Create AccelStepper objects for pan and tilt stepper motors
AccelStepper panStepper(AccelStepper::DRIVER, PAN_STEP_PIN, PAN_DIR_PIN);
AccelStepper tiltStepper(AccelStepper::DRIVER, TILT_STEP_PIN, TILT_DIR_PIN);

// Initialize variables for current pan and tilt positions
int currentPanPos = 0;
int currentTiltPos = 0;
int h = 0;

class Grape {
public:
  int id;
  int centerX;
  int centerY;
  int comLaserX;
  int comLaserY;

  // Default constructor
  Grape() {
    id = 0;
    centerX = 0;
    centerY = 0;
    comLaserX = 0;
    comLaserY = 0;
  }

  // Parameterized constructor
  Grape(int id, int centerX, int centerY, int comLaserX, int comLaserY) {
    this->id = id;
    this->centerX = centerX;
    this->centerY = centerY;
    this->comLaserX = comLaserX;
    this->comLaserY = comLaserY;
  }
};

// Create a buffer to store processed object IDs
const int BUFFER_SIZE = 50; // Adjust the size based on your requirements
int processedIDs[BUFFER_SIZE] = {0};
int bufferIndex = 0;

// Function to home the pan and tilt stepper motors
void homeStepperMotors() {
  Serial.println("homing...");
  // Set the sleep pins to HIGH to enable the stepper motors
  digitalWrite(PAN_SLEEP_PIN, HIGH);
  digitalWrite(TILT_SLEEP_PIN, HIGH);

  if (analogRead(PAN_HALL_SENSOR) > HALL_THRESHOLD) {
    // Home the pan stepper motor
    while (analogRead(PAN_HALL_SENSOR) > HALL_THRESHOLD) {
      panStepper.setSpeed(-500);
      panStepper.runSpeed();
    }
    panStepper.setCurrentPosition(0);
    delay(1000);
    // Apply offset
    panStepper.move(-55);
    panStepper.runToPosition();
  }

  if (analogRead(TILT_HALL_SENSOR) > HALL_THRESHOLD) {
    // Home the tilt stepper motor
    while (analogRead(TILT_HALL_SENSOR) > HALL_THRESHOLD) {
      tiltStepper.setSpeed(-500);
      tiltStepper.runSpeed();
    }
    tiltStepper.setCurrentPosition(0);
    delay(1000);
    // Apply offset
    tiltStepper.move(-10);
    tiltStepper.runToPosition();
  }

  delay(1000);
  // Set the sleep pins to LOW to disable the stepper motors
  digitalWrite(PAN_SLEEP_PIN, LOW);
  digitalWrite(TILT_SLEEP_PIN, LOW);

  // Update the current positions to be 0
  currentPanPos = 0;
  currentTiltPos = 0;
}

void calibration(int centerX, int centerY, int comLaserX, int comLaserY) {
  // Move the pan stepper motor based on the sign of the difference between centerX and comLaserX
  while (centerX - comLaserX < -10) {
    // Move the pan stepper motor to the left
    panStepper.move(-10);
    panStepper.runToPosition();

    // Update the current pan position
    currentPanPos -= 10;
  }

  while (centerX - comLaserX > 10) {
    // Move the pan stepper motor to the right
    panStepper.move(10);
    panStepper.runToPosition();

    // Update the current pan position
    currentPanPos += 10;
  }

  // Move the tilt stepper motor based on the sign of the difference between centerY and comLaserY
  while (centerY - comLaserY < -10) {
    // Move the tilt stepper motor to the left
    tiltStepper.move(-10);
    tiltStepper.runToPosition();

    // Update the current tilt position
    currentTiltPos -= 10;
  }

  while (centerY - comLaserY > 10) {
    // Move the tilt stepper motor to the right
    tiltStepper.move(10);
    tiltStepper.runToPosition();

    // Update the current tilt position
    currentTiltPos += 10;
  }
}


void spray(){
  Serial.println("Spraying Spraying");
  delay(6000);
}

void processGrape(const Grape& grape) {
  Serial.println("PROC GRAPE ");
  // Check if the ID of the grape has already been processed
  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (processedIDs[i] == grape.id) {
      Serial.print("Grape with ID ");
      Serial.print(grape.id);
      Serial.println(" already processed");
      return; // Exit the function if the ID has already been processed
    }
  }

  Serial.print("Processing grape with ID ");
  Serial.println(grape.id);

  // Set the sleep pins to HIGH to enable the stepper motors
  digitalWrite(PAN_SLEEP_PIN, HIGH);
  digitalWrite(TILT_SLEEP_PIN, HIGH);

  // Calculate the pan and tilt angles based on the (centerX, centerY) coordinates
  int panAngle = -10 - atan((grape.centerX - 320) / 600.0) * 180.0 / PI; // 320 is the center of the image, 600 is the focal length
  int tiltAngle = 90 + atan((320 - grape.centerY) / 600.0) * 180.0 / PI;

  int movePan = panAngle - currentPanPos;
  int moveTilt = tiltAngle - currentTiltPos;

  // Move the pan and tilt stepper motors to the desired positions
  int panSteps = movePan * stepsPerRevolution * panGearRatio / 360;
  panStepper.move(panSteps);
  panStepper.runToPosition();

  int tiltSteps = moveTilt * stepsPerRevolution * tiltGearRatio / 360;
  tiltStepper.move(tiltSteps);
  tiltStepper.runToPosition();

  //if (grape.comLaserX != 0 && grape.comLaserY != 0) {
    //calibration(grape.centerX, grape.centerY, grape.comLaserX, grape.comLaserY);
  //}
  
  // Update the current positions to be the desired positions
  currentPanPos = panAngle;
  currentTiltPos = tiltAngle;

  spray();
  
  // Add the ID to the processed IDs buffer
  processedIDs[bufferIndex] = grape.id;
  Serial.print("Actual ID");
  Serial.println(processedIDs[bufferIndex]);
  bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
}

#endif
