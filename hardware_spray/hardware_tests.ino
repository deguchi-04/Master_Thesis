#define STEP 3
#define DIR 2 
#define HALL_SENSOR      5
#define LED      13

boolean setdir = LOW;

void homefunction() {
  // Set motor speed pulse duration
  int pd = 4000;
 
  // Move motor until home position reached
  while (digitalRead(HALL_SENSOR) == 1) {
 
    digitalWrite(DIR, setdir);
    digitalWrite(STEP, HIGH);
    delayMicroseconds(pd);
    digitalWrite(STEP, LOW);
    delayMicroseconds(pd);
  }
  digitalWrite(LED, !digitalRead(HALL_SENSOR));
 
}
 
void setup() {
  // Sets the two pins as Outputs
  pinMode(STEP,OUTPUT); 
  pinMode(DIR,OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(HALL_SENSOR, INPUT);
  digitalWrite(LED, LOW);
}
void loop() {
  homefunction();
}