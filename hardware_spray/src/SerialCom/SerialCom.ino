#include <TaskScheduler.h>
#include <AccelStepper.h>
#include <Servo.h>

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
#define HALL_THRESHOLD 485

// Define gear ratios
#define PAN_GEAR_RATIO 8.47
#define TILT_GEAR_RATIO 3.05

// Define stepper motor steps per revolution
#define STEPS_PER_REV 200

// Create AccelStepper objects for pan and tilt stepper motors
AccelStepper panStepper(AccelStepper::DRIVER, PAN_STEP_PIN, PAN_DIR_PIN);
AccelStepper tiltStepper(AccelStepper::DRIVER, TILT_STEP_PIN, TILT_DIR_PIN);

Servo firstESC; // Create a Servo object for the first ESC

// Initialize variables for current pan and tilt positions
int currentPanPos = 0;
int currentTiltPos = 0;

const int BUFFER_SIZE = 600; // Adjust the size based on your requirements
int processedIDs[BUFFER_SIZE] = {0};
int bufferIndex = 0;

class Grape {
public:
  int id;
  float centerX;
  float centerY;
  int comLaserX;
  int comLaserY;

  // Default constructor
  Grape() : id(0), centerX(0), centerY(0), comLaserX(0), comLaserY(0) {}

  // Parameterized constructor
  Grape(int id, float centerX, float centerY, int comLaserX, int comLaserY)
    : id(id), centerX(centerX), centerY(centerY), comLaserX(comLaserX), comLaserY(comLaserY) {}
};

void homeStepperMotors() {
  Serial.println("Homing...");

  // Set the sleep pins to HIGH to enable the stepper motors
  digitalWrite(PAN_SLEEP_PIN, HIGH);
  digitalWrite(TILT_SLEEP_PIN, HIGH);
  int steps = 0;
  int dir = -1;
  panStepper.move(200);
  panStepper.runToPosition();
  
  if (digitalRead(PAN_HALL_SENSOR) != LOW) {
     while (digitalRead(PAN_HALL_SENSOR) != LOW && steps < 20000) 
     {
        panStepper.setSpeed(dir*500);
        panStepper.runSpeed();
        steps++;
     }
     if(digitalRead(PAN_HALL_SENSOR) != LOW && steps >= 20000)
     {
      dir = dir*dir;
     }
     while (digitalRead(PAN_HALL_SENSOR) != LOW) {
        panStepper.setSpeed(dir*500);
        panStepper.runSpeed();
     }
     panStepper.setCurrentPosition(0);
    delay(1000);
    panStepper.move(dir*35);
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
    tiltStepper.move(-25);
    tiltStepper.runToPosition();
  }

  delay(1000);

  float tiltSteps = 90 * STEPS_PER_REV * TILT_GEAR_RATIO / 360;
  tiltStepper.move(tiltSteps);
  tiltStepper.runToPosition();

  currentPanPos = 0;
  currentTiltPos = 0;
}

void spray() {
  Serial.println("Spraying Spraying");
  int receivedValue = 1200;
  firstESC.writeMicroseconds(receivedValue);
  Serial.print("Sending value ");
  Serial.println(receivedValue);
  delay(2500);
}

void processGrape(const Grape& grape) {
  Serial.println("PROC GRAPE");

  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (processedIDs[i] == grape.id) {
      Serial.print("Grape with ID ");
      Serial.print(grape.id);
      Serial.println(" already processed");
      firstESC.writeMicroseconds(0);
      return;
    }
  }

  Serial.print("Processing grape with ID ");
  Serial.println(grape.id);

  digitalWrite(PAN_SLEEP_PIN, HIGH);
  digitalWrite(TILT_SLEEP_PIN, HIGH);

  float movePan = -(grape.centerX - currentPanPos);
  float moveTilt = grape.centerY - currentTiltPos;

  float panSteps = movePan * STEPS_PER_REV * PAN_GEAR_RATIO / 360;
  panStepper.move(panSteps);
  panStepper.runToPosition();

  float tiltSteps = moveTilt * STEPS_PER_REV * TILT_GEAR_RATIO / 360;
  tiltStepper.move(tiltSteps);
  tiltStepper.runToPosition();

  currentPanPos = grape.centerX;
  currentTiltPos = grape.centerY;

  spray();

  processedIDs[bufferIndex] = grape.id;
  bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
  if (bufferIndex >= 590) {
    int remainingDataSize = bufferIndex - 580;
    memset(processedIDs, 0, 580 * sizeof(*processedIDs)); // Set data before position 590 to zero
  
    // Move data after position 590 to the beginning of the buffer
    memmove(processedIDs, &processedIDs[580], remainingDataSize * sizeof(*processedIDs));
  
    bufferIndex = remainingDataSize+10; // Update the bufferIndex
  }

}

Scheduler scheduler;
volatile bool stopExecution = false;
volatile int flag = 1;
Task stopTask(100, TASK_FOREVER, &stopExecutionCheck);
Task mainTask(500, TASK_FOREVER, &mainTaskFunction);

void stopExecutionCheck() {
  Serial.println("AAAA");
  // Check if stop signal is received
    if (flag == -1) {
      stopExecution = true;
      mainTask.disable(); // Stop the mainTask immediately
      stopTask.disable();
      
    }
  
}

void mainTaskFunction() {
  Serial.println("B");
  
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      if(input == 'stop')
      {
        flag = -1;
      }

        int commaIndex1 = input.indexOf(',');
        int commaIndex2 = input.indexOf(',', commaIndex1 + 1);
        int commaIndex3 = input.indexOf(',', commaIndex2 + 1);
        int commaIndex4 = input.indexOf(',', commaIndex3 + 1);
        int commaIndex5 = input.indexOf(',', commaIndex4 + 1);

        if (commaIndex1 == -1 || commaIndex2 == -1 || commaIndex3 == -1 || commaIndex4 == -1 || commaIndex5 == -1) {
          Serial.println("Invalid input format");
        } else {
          float centerX = round(input.substring(0, commaIndex1).toFloat());
          float centerY = round(input.substring(commaIndex1 + 1, commaIndex2).toFloat());
          int comLaserX = input.substring(commaIndex2 + 1, commaIndex3).toInt();
          int comLaserY = input.substring(commaIndex3 + 1, commaIndex4).toInt();
          int idStr = input.substring(commaIndex4 + 1).toInt();
          flag = input.substring(commaIndex5 +1).toInt();
          
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

          Serial.print("ON: ");
          Serial.println(flag);

          Grape grape(idStr, centerX, centerY, comLaserX, comLaserY);
          processGrape(grape);
        }
      
    }
  
}

void setup() {
  Serial.begin(9600);

  firstESC.attach(A3); // attached to pin 9 I just do this with 1 Servo  
  firstESC.writeMicroseconds(0); 
  
  // Set the sleep pins to LOW to disable the stepper motors
  digitalWrite(PAN_SLEEP_PIN, LOW);
  digitalWrite(TILT_SLEEP_PIN, LOW);

  // Set the initial speed and acceleration for the pan and tilt stepper motors
  panStepper.setMaxSpeed(300);
  panStepper.setAcceleration(700);
  tiltStepper.setMaxSpeed(300);
  tiltStepper.setAcceleration(700);

  // Home the stepper motors
  homeStepperMotors();
  
  
  scheduler.addTask(stopTask);
  stopTask.enable();
  scheduler.addTask(mainTask);
  mainTask.enable();
  scheduler.startNow();


}

void loop() {
  if (!stopExecution) {
    
    scheduler.execute();
  }
  if(stopExecution){
    Serial.println("STOPED");
    if (Serial.available() > 0) {
      
      String input = Serial.readStringUntil('\n');
      input.trim();
      if (input == "start") {
        firstESC.writeMicroseconds(0);
        homeStepperMotors();
        memset(processedIDs, 0, sizeof(processedIDs));
        bufferIndex = 0;
        mainTask.enable();
        stopTask.enable();
        flag = 1;
        stopExecution= false;
      }
  }
  }
  
}
