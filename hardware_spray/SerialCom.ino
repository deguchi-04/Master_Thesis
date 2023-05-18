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

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////7

// Function to home the pan and tilt stepper motors
void homeStepperMotors() {
  // Set the sleep pins to HIGH to enable the stepper motors
  digitalWrite(PAN_SLEEP_PIN, HIGH);
  digitalWrite(TILT_SLEEP_PIN, HIGH);
  
  if(analogRead(PAN_HALL_SENSOR) > HALL_THRESHOLD) {
    // Home the pan stepper motor
    while (analogRead(PAN_HALL_SENSOR) > HALL_THRESHOLD) {
      panStepper.setSpeed(-500);
      panStepper.runSpeed();
      
    }
    panStepper.setCurrentPosition(0);
    delay(1000);
    // Apply offset
    panStepper.move(-60);
    panStepper.runToPosition();  
  }
  
  if(analogRead(TILT_HALL_SENSOR) > HALL_THRESHOLD) {
    // Home the tilt stepper motor
    while (analogRead(TILT_HALL_SENSOR) > HALL_THRESHOLD) {
      tiltStepper.setSpeed(-500);
      tiltStepper.runSpeed();
    }
    tiltStepper.setCurrentPosition(0);
    delay(1000);
    // Apply offset
    tiltStepper.move(-8);
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

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////7

void setup() {
  // Initialize the serial communication
  Serial.begin(9600);
  // Set the sleep pins to LOW to disable the stepper motors
  digitalWrite(PAN_SLEEP_PIN, LOW);
  digitalWrite(TILT_SLEEP_PIN, LOW);
  
  // Set the initial speed and acceleration for the pan and tilt stepper motors
  panStepper.setMaxSpeed(500);
  panStepper.setAcceleration(2000);
  tiltStepper.setMaxSpeed(500);
  tiltStepper.setAcceleration(2000);
  
  // Home the stepper motors
  homeStepperMotors();
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////7


void loop() {
  // Wait for user input from the serial monitor
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "home") {
      // Home the stepper motors
      Serial.println("homing...");
      homeStepperMotors();
    } else {
      // Parse the input values for centerX, centerY, idStr, comLaserX, and comLaserY positions
      int commaIndex1 = input.indexOf(',');
      int commaIndex2 = input.indexOf(',', commaIndex1 + 1);
      int commaIndex3 = input.indexOf(',', commaIndex2 + 1);
      int commaIndex4 = input.indexOf(',', commaIndex3 + 1);

      if (commaIndex1 == -1 || commaIndex2 == -1 || commaIndex3 == -1 || commaIndex4 == -1) {
        // Invalid input format
        Serial.println("Invalid input format");
      } else {
        int centerX = input.substring(0, commaIndex1).toInt();
        int centerY = input.substring(commaIndex1 + 1, commaIndex2).toInt();
        int idStr = input.substring(commaIndex2 + 1, commaIndex3).toInt();
        int comLaserX = input.substring(commaIndex3 + 1, commaIndex4).toInt();
        int comLaserY = input.substring(commaIndex4 + 1).toInt();
        Serial.print("Point: ");
        Serial.print(centerX);
        Serial.print(",");
        Serial.println(centerY);
        
        Serial.print("Point Laser: ");
        Serial.print(comLaserX);
        Serial.print(",");
        Serial.println(comLaserY);

        Serial.print("Id: ");
        Serial.println(idStr);

        // Set the sleep pins to HIGH to enable the stepper motors
        digitalWrite(PAN_SLEEP_PIN, HIGH);
        digitalWrite(TILT_SLEEP_PIN, HIGH);

        // Calculate the pan and tilt angles based on the (centerX, centerY) coordinates
        int panAngle = -25 - atan((centerX - 320) / 600.0) * 180.0 / PI; // 320 is the center of the image, 600 is the focal length
        int tiltAngle = 85 + atan((320 - centerY) / 600.0) * 180.0 / PI;

        Serial.println("Angle");
        Serial.print(panAngle);
        Serial.print(",");
        Serial.println(tiltAngle);

        int movePan = panAngle - currentPanPos;
        int moveTilt = tiltAngle - currentTiltPos;
        Serial.println(" ");
        Serial.println("pos");
        Serial.print(movePan);
        Serial.print(",");
        Serial.println(moveTilt);
        
        // Move the pan and tilt stepper motors to the desired positions
        int panSteps = movePan * stepsPerRevolution * panGearRatio / 360;
        panStepper.move(panSteps);
        panStepper.runToPosition();

        int tiltSteps = moveTilt * stepsPerRevolution * tiltGearRatio / 360;
        tiltStepper.move(tiltSteps);
        tiltStepper.runToPosition();

        // Update the current positions to be the desired positions
        currentPanPos = panAngle;
        currentTiltPos = tiltAngle;
      }
    }
  }
}
