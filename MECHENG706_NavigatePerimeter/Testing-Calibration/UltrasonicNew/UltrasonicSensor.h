#include <Arduino.h>

class UltrasonicSensor {
public:
	// Constructor 
	UltrasonicSensor(uint8_t trigPin, uint8_t echoPin);

	// Public Members
	uint8_t pulseDuration;
	double distance;

	// Public Methods
	void clearTrigPin();
	double getDistance();


private:
	// Private Members
	uint8_t trigPin;
	uint8_t echoPin;

};