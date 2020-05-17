#include <Arduino.h>
#include <MECHENG706.h>

class Gyroscope {
public:
  // Constructor
  Gyroscope();

  // Public Methods
  void gyro_Init();
  void updateAngle();
  float getAngle();

private:
  // Private Members
  int gyroPin = A2;

  int gyroValue;
  unsigned long prevTime;
  float gyroSupplyVoltage;
  float gyroZeroVoltage;
  float gyroSensitivity;
  float rotationThreshold;
  float gyroRate;
  float currentAngle;
  float angleChange;
  float angularVelocity;

};
