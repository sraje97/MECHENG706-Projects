#include <UltrasonicSensor706.h>

UltrasonicSensor706::UltrasonicSensor706(uint8_t trigPin, uint8_t echoPin) {
	this->trigPin = trigPin;
	this->echoPin = echoPin;

	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
}

void UltrasonicSensor706::clearTrigPin() {
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
}

double UltrasonicSensor706::getBurstDistance() {
	double sum = 0;
	for (int i = 0; i < 5; i++) {
		sum += getDistance();
		delayMicroseconds(10);
	}
	return sum / 5;
}
	

double UltrasonicSensor706::getDistance() {
	// Sets the trigPin on HIGH state for 10 micro seconds
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);

	// Reads the echoPin, returns the sound wave travel time in microseconds
	pulseDuration = pulseIn(echoPin, HIGH);

	// Calculating the distance
	return pulseDuration*0.034 / 2;
}