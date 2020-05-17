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

		void TrackAndSweep();

		void StartSweep();

		bool IsTracking();

		int SweepPos();

		// Return the relative angle between the robot and the light
		int GetAngle();

		// Returns true if a fire is detected
		bool FireDetected();

		bool FireIsClose();

		bool FireExtinguished();
		void ResetFireExtinguished();

		void Debug();

	private:
		Servo servo;
		int servoPosition;
		int stageOfSweep;
		unsigned long previousMillisServo;	// Ensures that the servo isn't written to too often
		unsigned long previousMillisSweep;	// Used to switch between sweeping and tracking
		bool tracking;
		int derivativeCount;
		bool fireExtinguished;

		// Fuzzy input set (set dyanamically during runtime)
		int dimUpper, brightLower;

		// Fuzzy output set
		const float leftLower = -0.5;
		const float leftUpper = -0.05;
		const float rightLower = 0.05;
		const float rightUpper = 0.5;

		const int offset[4] = { 0, 0, 0, 0 };

		// Array of phototransistor pins
		int PTPins[4] = { PHOTOTRANSISTOR1, PHOTOTRANSISTOR2, PHOTOTRANSISTOR3, PHOTOTRANSISTOR4 };

		// Array of phototransistor readings
		int PTReadings[4];
		int prevPTReadings[4];

		// Array of fuzzified inputs
		float PTFuzzy[4][2];

};
