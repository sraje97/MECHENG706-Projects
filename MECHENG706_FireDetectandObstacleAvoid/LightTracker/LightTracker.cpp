/***********************************
* Filename: LightTracker.h
* Purpose: Track a detected light using a servo and a phototransistor array
* Author: Michael Jessup, MJES247
* Date created: 30/5/19
***********************************/

#include <LightTracker.h>

void LightTracker::Init() {
	pinMode(PHOTOTRANSISTOR1, INPUT);
	pinMode(PHOTOTRANSISTOR2, INPUT);
	pinMode(PHOTOTRANSISTOR3, INPUT);
	pinMode(PHOTOTRANSISTOR4, INPUT);

	servo.attach(SERVO_PIN);

	servo.write(SERVO_MIDDLE);

	previousMillisServo = 0;
	previousMillisSweep = 0;
	stageOfSweep = 0;
	tracking = true;
	derivativeCount = 0;
	fireExtinguished = false;
	for (int i = 0; i < 4; i++) {
		prevPTReadings[i] = 0;
	}
}

bool LightTracker::FireDetected() {
	int sum = 0;
	for (int i = 0; i < 4; i++) {
		sum += analogRead(PTPins[i]);
	}
	return sum > 60;
}

bool LightTracker::FireIsClose() {
	return (analogRead(PTPins[1]) + analogRead(PTPins[2])) > 1500;
}

int LightTracker::GetAngle() {
	if (tracking) {
		return SERVO_MIDDLE - servoPosition;
	} else {
		return 0;
	}
}

bool LightTracker::FireExtinguished() {
	return fireExtinguished;
}

void LightTracker::ResetFireExtinguished() {
	fireExtinguished = false;
}

void LightTracker::Debug() {
	Serial.print('[');
	for (int i = 0; i < 4; i++) {
		Serial.print(PTReadings[i]);
		if (i < 3) {
			Serial.print(',');
		}
	}
	Serial.println(']');
}

void LightTracker::TrackAndSweep() {
	if (tracking) {
		Track();
		if (millis() - previousMillisSweep > 1000) {
			tracking = false;
			previousMillisSweep = millis();
		}
	} else {
		if (millis() - previousMillisSweep < 250) {
			stageOfSweep = 1;
			servo.write(SERVO_MIN);
		} else if (millis() - previousMillisSweep < 500) {
			stageOfSweep = 2;
			servo.write(SERVO_MAX);
		} else {
			stageOfSweep = 0;
			servo.write(SERVO_MIDDLE);
			previousMillisSweep = millis();
			tracking = true;
		}
	}
}

// -1 = not sweping
// 0 = left
// 1 = middle
// 2 = right
int LightTracker::SweepPos() {
	if (tracking || stageOfSweep == 0) {
		return -1;
	} else if (stageOfSweep == 1) {
		return millis() - previousMillisSweep < 125 ? 1 : 2;
	} else if (stageOfSweep == 2) {
		if (millis() - previousMillisSweep < 350) {
			return 2;
		} else if (millis() - previousMillisSweep < 400) {
			return 1;
		} else {
			return 0;
		}
	}
}

void LightTracker::StartSweep() {
	previousMillisSweep = millis();
	tracking = false;
	TrackAndSweep();
}

bool LightTracker::IsTracking() {
	return tracking;
}

void LightTracker::Track() {
	// Read phototransistors
	derivativeCount++;
	for (int i = 0; i < 4; i++) {
		if (derivativeCount >= 50) {
			prevPTReadings[i] = PTReadings[i];
		}
		PTReadings[i] = analogRead(PTPins[i]) + offset[i];
	}
	if (derivativeCount >= 50) {
		derivativeCount = 0;
	}

	dimUpper = min(min(PTReadings[0], PTReadings[1]), min(PTReadings[2], PTReadings[3]));
	brightLower = max(max(PTReadings[0], PTReadings[1]), max(PTReadings[2], PTReadings[3]));
	if (dimUpper < 10) { dimUpper = 10; }

	// Fuzzify phototransistors
	for (int i = 0; i < 4; i++) {
		if (PTReadings[i] < dimUpper) {
			PTFuzzy[i][0] = 1;
			PTFuzzy[i][1] = 0;
		}
		else if (PTReadings[i] > brightLower) {
			PTFuzzy[i][0] = 0;
			PTFuzzy[i][1] = 1;
		}
		else {
			PTFuzzy[i][0] = (float)(brightLower - PTReadings[i]) / (brightLower - dimUpper);
			PTFuzzy[i][1] = (float)(PTReadings[i] - dimUpper) / (brightLower - dimUpper);
		}
	}

	// Apply Fuzzy Rules
	float fuzzyRules[3];  // {L, 0, R}
	fuzzyRules[0] = min(min(PTFuzzy[0][1], PTFuzzy[2][0]), PTFuzzy[3][0]); // BXDD -> L
	fuzzyRules[0] = max(fuzzyRules[0], min(min(min(PTFuzzy[0][0], PTFuzzy[1][1]), PTFuzzy[2][0]), PTFuzzy[3][0]));  //DBDD -> L
	fuzzyRules[1] = min(PTFuzzy[1][1], PTFuzzy[2][1]); // XBBX -> 0
	fuzzyRules[2] = min(min(PTFuzzy[0][0], PTFuzzy[1][0]), PTFuzzy[3][1]);  // DDXB -> R
	fuzzyRules[2] = max(fuzzyRules[2], min(min(min(PTFuzzy[0][0], PTFuzzy[1][0]), PTFuzzy[2][1]), PTFuzzy[3][1]));  // DDBD -> R

	// Defuzzification
	float intersections[4];
	intersections[0] = leftUpper + (leftLower - leftUpper) * fuzzyRules[0];
	intersections[1] = leftLower + (leftUpper - leftLower) * fuzzyRules[1];
	intersections[2] = rightUpper + (rightLower - rightUpper) * fuzzyRules[1];
	intersections[3] = rightLower + (rightUpper - rightLower) * fuzzyRules[2];

	// Centroid Defuzzification
	// Add 1 to everything so that everything is positive (makes the maths easier)
	for (int i = 0; i < 4; i++) {
		intersections[i]++;
	}

	float totalArea = fuzzyRules[0] * intersections[0] + 0.5 * (intersections[1] - intersections[0]) * (fuzzyRules[0] + fuzzyRules[1])
		+ fuzzyRules[1] * (intersections[2] - intersections[1]) + 0.5 * (intersections[3] - intersections[2]) * (fuzzyRules[1] + fuzzyRules[2])
		+ fuzzyRules[2] * (2 - intersections[3]);
	float centroidNumerator = (intersections[0] / 2) * (fuzzyRules[0] * intersections[0])
		+ ((intersections[2] - intersections[1]) / 2 + intersections[1]) * (intersections[2] - intersections[1]) * fuzzyRules[1]
		+ ((2 - intersections[3]) / 2 + intersections[3]) * (2 - intersections[3]) * fuzzyRules[2];

	if ((fuzzyRules[0] == 0) and (fuzzyRules[1] == 0) and (fuzzyRules[2] == 0)) { // All 0
		centroidNumerator = 0;
	}
	else if ((fuzzyRules[0] == 0) and (fuzzyRules[1] == 0)) { // First two are 0
		if (fuzzyRules[1] > fuzzyRules[2]) {
			centroidNumerator += 0.5 * (intersections[3] - intersections[2]) * (fuzzyRules[1] + fuzzyRules[2]) * (intersections[2] + ((intersections[3] - intersections[2]) * (2 * fuzzyRules[2] + fuzzyRules[1])) / (3 * (fuzzyRules[1] + fuzzyRules[2])));
		}
		else {
			centroidNumerator += 0.5 * (intersections[3] - intersections[2]) * (fuzzyRules[1] + fuzzyRules[2]) * (intersections[3] - ((intersections[3] - intersections[2]) * (2 * fuzzyRules[1] + fuzzyRules[2])) / (3 * (fuzzyRules[1] + fuzzyRules[2])));
		}
	}
	else if ((fuzzyRules[1] == 0) and (fuzzyRules[2] == 0)) { // Last two are 0
		if (fuzzyRules[0] > fuzzyRules[1]) {
			centroidNumerator += 0.5 * (intersections[1] - intersections[0]) * (fuzzyRules[0] + fuzzyRules[1]) * (intersections[0] + ((intersections[1] - intersections[0]) * (2 * fuzzyRules[1] + fuzzyRules[0])) / (3 * (fuzzyRules[0] + fuzzyRules[1])));
		}
		else {
			centroidNumerator += 0.5 * (intersections[1] - intersections[0]) * (fuzzyRules[0] + fuzzyRules[1]) * (intersections[1] - ((intersections[1] - intersections[0]) * (2 * fuzzyRules[0] + fuzzyRules[1])) / (3 * (fuzzyRules[0] + fuzzyRules[1])));
		}
	}
	else {
		if (fuzzyRules[0] > fuzzyRules[1]) { // All different levels
			centroidNumerator += 0.5 * (intersections[1] - intersections[0]) * (fuzzyRules[0] + fuzzyRules[1]) * (intersections[0] + ((intersections[1] - intersections[0]) * (2 * fuzzyRules[1] + fuzzyRules[0])) / (3 * (fuzzyRules[0] + fuzzyRules[1])));
		}
		else {
			centroidNumerator += 0.5 * (intersections[1] - intersections[0]) * (fuzzyRules[0] + fuzzyRules[1]) * (intersections[1] - ((intersections[1] - intersections[0]) * (2 * fuzzyRules[0] + fuzzyRules[1])) / (3 * (fuzzyRules[0] + fuzzyRules[1])));
		}
		if (fuzzyRules[1] > fuzzyRules[2]) {
			centroidNumerator += 0.5 * (intersections[3] - intersections[2]) * (fuzzyRules[1] + fuzzyRules[2]) * (intersections[2] + ((intersections[3] - intersections[2]) * (2 * fuzzyRules[2] + fuzzyRules[1])) / (3 * (fuzzyRules[1] + fuzzyRules[2])));
		}
		else {
			centroidNumerator += 0.5 * (intersections[3] - intersections[2]) * (fuzzyRules[1] + fuzzyRules[2]) * (intersections[3] - ((intersections[3] - intersections[2]) * (2 * fuzzyRules[1] + fuzzyRules[2])) / (3 * (fuzzyRules[1] + fuzzyRules[2])));
		}
	}

	// Calculate the centroid
	float fuzzyAction;
	if ((fuzzyRules[0] == 0) and (fuzzyRules[1] == 0) and (fuzzyRules[2] == 0)) {
		fuzzyAction = 0;
	}
	else {
		fuzzyAction = centroidNumerator / totalArea;
		// Subtract 1 so that the range is from -1 to 1
		fuzzyAction -= 1;
	}

	if (millis() - previousMillisServo > 20) {
		servoPosition -= round(fuzzyAction * 10);

		if (servoPosition > SERVO_MAX) {
			servoPosition = SERVO_MAX;
		}
		if (servoPosition < SERVO_MIN) {
			servoPosition = SERVO_MIN;
		}

		servo.write(servoPosition);
		previousMillisServo = millis();
	}

	// Determine whether the fire has been extinguished by differentiating the phototransistor readings
	int sum = 0;
	if (derivativeCount == 49) {
		//Serial.print("{");
		for (int i = 0; i < 4; i++) {
			//Serial.print(prevPTReadings[i]);
			//Serial.print(',');
			sum += prevPTReadings[i] - PTReadings[i];
		}
		if (sum > 700) {
			fireExtinguished = true;
		}
		//Serial.println('}');
		//Serial.println(sum);
	}
}
