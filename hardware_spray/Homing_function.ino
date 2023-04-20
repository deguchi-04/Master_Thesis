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

// Define the steps per revolution for each stepper motor
#define PAN_STEPS_PER_REV 1540
#define TILT_STEPS_PER_REV 200

// Define the maximum degrees of pan and tilt
#define MAX_PAN_DEGREES 180
#define MAX_TILT_DEGREES 180

// Create AccelStepper objects for pan and tilt stepper motors
AccelStepper panStepper(AccelStepper::DRIVER, PAN_STEP_PIN, PAN_DIR_PIN);
AccelStepper tiltStepper(AccelStepper::DRIVER, TILT_STEP_PIN, TILT_DIR_PIN);

// Initialize variables for current pan and tilt positions
int currentPanPos = 0;
int currentTiltPos = 0;

int h = 0;



// Function to home the pan and tilt stepper motors
void homeStepperMotors() {
  // Set the sleep pins to HIGH to enable the stepper motors
  digitalWrite(PAN_SLEEP_PIN, HIGH);
  digitalWrite(TILT_SLEEP_PIN, HIGH);
  if(analogRead(PAN_HALL_SENSOR) > HALL_THRESHOLD) {
    // Home the pan stepper motor
  while (analogRead(PAN_HALL_SENSOR) > HALL_THRESHOLD) {
   
    Serial.println(analogRead(PAN_HALL_SENSOR));
    panStepper.setSpeed(500);
    panStepper.runSpeed();
    
  }
  panStepper.setCurrentPosition(0);
  delay(1000);
     // Apply offset
    panStepper.move(15);
    panStepper.runToPosition();
    
  }
  
  if(analogRead(TILT_HALL_SENSOR) > HALL_THRESHOLD) {
    // Home the tilt stepper motor
    while (analogRead(TILT_HALL_SENSOR) > HALL_THRESHOLD) {
      
      tiltStepper.setSpeed(500);
      tiltStepper.runSpeed();
    }
    tiltStepper.setCurrentPosition(0);
    delay(1000);
     // Apply offset
    tiltStepper.move(8);
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

void setup() {
  // Set the sleep pins to LOW to disable the stepper motors
  digitalWrite(PAN_SLEEP_PIN, LOW);
  digitalWrite(TILT_SLEEP_PIN, LOW);
  
  // Set the initial speed and acceleration for the pan and tilt stepper motors
  panStepper.setMaxSpeed(1500);
  panStepper.setAcceleration(1000);
  tiltStepper.setMaxSpeed(1500);
  tiltStepper.setAcceleration(1000);
  
  // Home the stepper motors
  homeStepperMotors();
  
  // Initialize the serial communication
  Serial.begin(9600);
}

void loop() {
  // Wait for user input from the serial monitor
  if (Serial.available() > 0) {
    // Read the input values for pan and tilt positions
    float panPos = Serial.parseInt();
    float tiltPos = -1*Serial.parseInt();
    h = Serial.parseInt();

    if(h == -1000){
      homeStepperMotors();
    }
  

// Calculate the number of steps needed to move the pan and tilt stepper motors

  // Set the sleep pins to HIGH to enable the stepper motors
  digitalWrite(PAN_SLEEP_PIN, HIGH);
  digitalWrite(TILT_SLEEP_PIN, HIGH);
  
  // Move the pan and tilt stepper motors to the desired positions
  int panSteps = 200*panPos/55;
  //int panSteps= map(panPos, 0, MAX_PAN_DEGREES, 0, PAN_STEPS_PER_REV);
  
  panStepper.move(panSteps);
  Serial.println(panPos);
  Serial.println(panSteps);
  panStepper.runToPosition();
  //int tiltSteps = map(tiltPos, 0, MAX_TILT_DEGREES, 0, TILT_STEPS_PER_REV);
  int tiltSteps = 200*tiltPos/115;
  tiltStepper.move(tiltSteps);
  Serial.println("");
  Serial.println(tiltPos);
  Serial.println(tiltSteps);
  tiltStepper.runToPosition();
  
  // Update the current positions to be the desired positions
  currentPanPos = panPos;
  currentTiltPos = tiltPos;

}
}
