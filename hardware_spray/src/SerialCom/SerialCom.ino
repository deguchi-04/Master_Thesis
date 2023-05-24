#include "sprayer.h"

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
      homeStepperMotors();
      // Clear the processed IDs array
      memset(processedIDs, 0, sizeof(processedIDs)); // Clear the array by setting all elements to 0
      bufferIndex = 0; // Reset the buffer index
      
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
        int comLaserX = input.substring(commaIndex2 + 1, commaIndex3).toInt();
        int comLaserY = input.substring(commaIndex3 + 1, commaIndex4).toInt();
        int idStr = input.substring(commaIndex4 + 1).toInt();
        Serial.println("LOOP ");
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

        Grape grape(idStr, centerX, centerY, comLaserX, comLaserY);
        processGrape(grape);
      }
    }
  }
  
  
}
