#include <Gyroscope2.h>

// Constructor
Gyroscope2::Gyroscope2() {
	currentAngle = 0;
}


void Gyroscope2::Init() {
	pinMode(gyroPin, INPUT);
	int readings[100], reading;
	double average = 0;
	int numSamples = 100;
	double stdDev = 0;
	
	// Record multiple samples
	Serial1.println("Readings:");
	for (int i = 0; i < numSamples; i++) {
		reading = analogRead(gyroPin);
		readings[i] = reading;
		average += reading;
		delay(2);
		Serial1.println(reading);
	}

	// Find the average value
	average /= numSamples;

	// Find the standard deviation
	for (int i = 0; i < numSamples; i++) {
		stdDev += pow((readings[i] - average), 2);
	}
	stdDev /= numSamples;
	stdDev = sqrt(stdDev);

	// Set the gyro parameters
	zeroPoint = average;
	noiseThreshold = stdDev * 2;

	Serial1.print("zeroPoint: ");
	Serial1.println(zeroPoint);
	Serial1.print("noiseThreshold");
	Serial1.println(noiseThreshold);

	previousMillis = millis();
}

void Gyroscope2::UpdateAngle() {

	// Read the gyro voltage
	int readingRaw = analogRead(gyroPin);
	Serial1.print("Reading: ");
	Serial1.println(readingRaw);

	// If the reading is within the noise threshold, ignore it
	if (readingRaw > zeroPoint - noiseThreshold && readingRaw < zeroPoint + noiseThreshold) {
		//Serial1.println("FORCING EARLY RETURN THRESHOLD");
		return;
	}

	// Check that some time has passed since the last reading (checking for divide by zero error)
	if (previousMillis == millis()) {
		//Serial1.println("FORCING EARLY RETURN MILLIS");
		return;
	}

	// Convert the ADC reading to a voltage
	double readingVoltage = (((double)readingRaw - zeroPoint) * 5) / 1023;

	// Calculate the angular velocity
	double angularVelocity = readingVoltage / sensitivity;

	// Approximate the integration of the angular velocity
	double angularDisplacement = angularVelocity * ((millis() - previousMillis) / 1000);

	currentAngle += angularDisplacement;

	previousMillis = millis();
}

double Gyroscope2::GetAngle() {
	UpdateAngle();
	return currentAngle;
}