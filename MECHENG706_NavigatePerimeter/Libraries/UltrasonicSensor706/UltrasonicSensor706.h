#include <Arduino.h>

class UltrasonicSensor706 {
public:
	// Constructor
	UltrasonicSensor706(uint8_t trigPin, uint8_t echoPin);

	// Public Members
	unsigned long pulseDuration;
	double distance;

	// Public Methods
	void clearTrigPin();
	double getDistance();


private:
	// Private Members
	uint8_t trigPin;
	uint8_t echoPin;

};
