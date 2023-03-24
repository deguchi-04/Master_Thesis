void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

void HomeFunction() {
  Serial.println("Finding Home...");
  steppermotor.setSpeed(Spd);
  steppermotor.step(STRT * 3);
  while (digitalRead(HALL_SENSOR) == LOW)
  {
    steppermotor.step(1);
  }
  while (digitalRead(HALL_SENSOR) == HIGH)
  {
    steppermotor.step(1);
  }
  steppermotor.step(STRT * 3); //SET TO 20 FOR A TO BE HOME
  Serial.println("Home Found!");
}
