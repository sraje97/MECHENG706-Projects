#include <Gyroscope.h>

Gyroscope::Gyroscope() {
  prevTime = 0;
  gyroValue = 0; // Read out value of gyroscope
  gyroSupplyVoltage = 5; // Supply voltage for gyro
  gyroZeroVoltage = 0; // The value of voltage when gyro is zero
  gyroSensitivity = 0.007; // Gyro sensitivity unit is (mv/degree/second) get from datasheet
  rotationThreshold = 2; // Because of gyro drifting, defining rotation angular velocity less
  // than this value will be ignored
  gyroRate = 0; // Read out value of sensor in voltage
  currentAngle = 0; // Current angle calculated by angular velocity integral on
  angleChange = 0;
  angularVelocity = 0;
}

void Gyroscope::gyro_Init() {
  int i;
  float sum = 0;
  pinMode(gyroPin,INPUT);
  currentAngle = 0;

  // Read 100 values of voltage when gyro is at still, to calculate the zero-drift.
  for (i = 0; i < 100; i++) {
    gyroValue = analogRead(gyroPin);
    sum += gyroValue;
    delay(5);
  }
  gyroZeroVoltage = sum/100; // Average the sum as the zero drifting
}

void Gyroscope::updateAngle() {

  // Gyro Rate Calculation
  // Convert the 0-1023 signal to 0-5V
  gyroRate = (analogRead(gyroPin)*gyroSupplyVoltage)/1023;
  //Serial1.println("---");
  //Serial1.println(gyroRate);
  // Find the voltage offset the value of voltage when gyro is zero (still)
  gyroRate -= ((gyroZeroVoltage/1023)*gyroSupplyVoltage);
  //Serial1.println(gyroRate);

  // Angular velocity & angle Calculation
  // Read out voltage divided the gyro sensitivity to calculate the angular velocity
  angularVelocity = gyroRate/gyroSensitivity; // from Data Sheet, gyroSensitivity is 0.007 V/dps
  //Serial1.println(angularVelocity);

  // Check for divide by zero error
  if (millis() == prevTime) {
	  Serial1.println("FORCING EARLY RETURN");
	  return;
  }

  // If the angular velocity is less than the threshold, ignore it
  if (angularVelocity >= rotationThreshold || angularVelocity <= -rotationThreshold) {	// THIS CONDITIONAL ISN'T EXECUTING FOR SOME REASON
    angleChange = angularVelocity/(1000/(millis() - prevTime));
	//Serial.println(millis());
	//Serial.println(prevTime);
	//Serial.println(angleChange);
    currentAngle += angleChange;
  }

  //Serial1.println(currentAngle);

  // keep the angle between 0-360
  /*if (currentAngle < 0) {
    currentAngle += 360;
  } else if (currentAngle > 359) {
    currentAngle -= 360;
  }*/

  //Serial1.println(currentAngle);

  prevTime = millis();

 //Serial1.println(prevTime);
}

float Gyroscope::getAngle() {
  updateAngle();

  return currentAngle;
}
