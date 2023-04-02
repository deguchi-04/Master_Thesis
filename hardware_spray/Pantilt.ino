#include <AccelStepper.h>

// Define the pins for the stepper motors
#define PAN_STEP_PIN 2
#define PAN_DIR_PIN 3
#define TILT_STEP_PIN 4
#define TILT_DIR_PIN 5
#define HALL_SENSOR_PIN 6

// Define the maximum angle for the tilt movement
#define MAX_TILT_ANGLE 141

// Define the steps per revolution for the stepper motors
#define STEPS_PER_REVOLUTION 200

// Define the acceleration and speed for the stepper motors
#define STEPPER_ACCELERATION 1000
#define STEPPER_SPEED 500

// Define the initial position of the pan and tilt motors
#define PAN_INITIAL_POSITION 0
#define TILT_INITIAL_POSITION 0

// Define the homing state
#define HOMING_STATE 0

// Define the pan and tilt angles
int pan_angle = 0;
int tilt_angle = 0;

// Create the stepper motor objects
AccelStepper pan_motor(AccelStepper::DRIVER, PAN_STEP_PIN, PAN_DIR_PIN);
AccelStepper tilt_motor(AccelStepper::DRIVER, TILT_STEP_PIN, TILT_DIR_PIN);

// Homing function to set the initial position
void homing() {
  // Move the tilt motor until the hall sensor is triggered
  tilt_motor.setAcceleration(STEPPER_ACCELERATION);
  tilt_motor.setSpeed(STEPPER_SPEED);
  tilt_motor.setMaxSpeed(STEPPER_SPEED);
  tilt_motor.moveTo(MAX_TILT_ANGLE * STEPS_PER_REVOLUTION / 360);
  while (!digitalRead(HALL_SENSOR_PIN)) {
    tilt_motor.run();
  }
  tilt_motor.stop();
  tilt_motor.setCurrentPosition(0);

  // Set the initial position of the pan and tilt motors
  pan_motor.setCurrentPosition(PAN_INITIAL_POSITION);
  tilt_motor.setCurrentPosition(TILT_INITIAL_POSITION);
}

void setup() {
  // Initialize the pins
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);

  // Set the acceleration and speed for the motors
  pan_motor.setAcceleration(STEPPER_ACCELERATION);
  pan_motor.setSpeed(STEPPER_SPEED);
  tilt_motor.setAcceleration(STEPPER_ACCELERATION);
  tilt_motor.setSpeed(STEPPER_SPEED);

  // Set the initial position of the motors
  homing();
}

void loop() {
  // Read the (x, y) coordinate from ROS2
  int x = 0; // replace with the actual x coordinate
  int y = 0; // replace with the actual y coordinate

  // Calculate the pan and tilt angles using direct kinematics
  pan_angle = (int)(atan2(x - 3, y) * 180 / PI);
  tilt_angle = (int)(atan2(y, x - 3) * 180 / PI);

  // Move the pan and tilt motors to the desired angles
  pan_motor.moveTo(pan_angle * STEPS_PER_REVOLUTION / 360);
  tilt_motor.moveTo(tilt_angle * STEPS_PER_REVOLUTION / 360);

  // Run the motors until they reach the desired positions
  while (pan_motor.distanceToGo() != 0 || tilt_motor.distanceToGo() != 0) {
    pan_motor.run();
    tilt_motor.run();
  }
}
