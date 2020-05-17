#include <Arduino.h>
#include <MECHENG706.h>

class Gyroscope2 {
public:
	// Constructor
	Gyroscope2();

	// Public methods
	void Init();	// Initialises gyro object and sets zero point
	void UpdateAngle();
	double GetAngle();

private:
	int gyroPin = A2;
	double zeroPoint;	// The ADC value that the gyro reads when it is stationary
	double noiseThreshold;	// The ADC value below which readings will be ignored
	const double sensitivity = 0.007;
	double currentAngle;
	unsigned long previousMillis;
};