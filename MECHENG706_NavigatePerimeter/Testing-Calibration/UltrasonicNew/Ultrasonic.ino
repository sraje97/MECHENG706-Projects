#include <UltrasonicSensor.h>

UltrasonicSensor ultrasonic = UltrasonicSensor(48,49);

void setup() {
  Serial.begin(9600);
  ultrasonic.clearTrigPin();
}

void loop() {
  Serial.println(ultrasonic.getDistance());
}
