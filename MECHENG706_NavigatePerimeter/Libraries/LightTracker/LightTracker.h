/***********************************
* Filename: LightTracker.h
* Purpose: Track a detected light using a servo and a phototransistor array
* Author: Michael Jessup, MJES247
* Date created: 30/5/19
***********************************/

#include <Arduino.h>
#include <Servo.h>
#include <MECHENG706.h>

class LightTracker {
	public:
		void Init();

		// Track the light
		void Track();

		// Return the relative angle between the robot and the light
		int GetAngle();

		// Returns true if a fire is detected
		bool FireDetected();

	private:
		Servo servo;
		int servoPosition;
		unsigned long previousMillis;
		
		// Fuzzy input set (set dyanamically during runtime)
		int dimUpper, brightLower;
		
		// Fuzzy output set
		const float leftLower = -0.5;
		const float leftUpper = -0.1;
		const float rightLower = 0.1;
		const float rightUpper = 0.5;

		// Array of phototransistor pins
		int PTPins[4] = { PHOTOTRANSISTOR1, PHOTOTRANSISTOR2, PHOTOTRANSISTOR3, PHOTOTRANSISTOR4 };
		
		// Array of phototransistor readings
		int PTReadings[4];

		// Array of fuzzified inputs
		float PTFuzzy[4][2];

};