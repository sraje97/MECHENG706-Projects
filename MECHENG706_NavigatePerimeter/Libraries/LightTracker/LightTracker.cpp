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

	previousMillis = 0;
}

bool LightTracker::FireDetected() {
	int sum = 0;
	for (int i = 0; i < 4; i++) {
		sum += analogRead(PTPins[i]);
	}
	return sum > 60;
}

int LightTracker::GetAngle() {
	return SERVO_MIDDLE - servoPosition;
}

void LightTracker::Track() {
	// Read phototransistors
	for (int i = 0; i < 4; i++) {
		PTReadings[i] = analogRead(PTPins[i]);
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

	if (millis() - previousMillis > 20) {
		servoPosition -= round(fuzzyAction * 10);

		if (servoPosition > 150) {
			servoPosition = 150;
		}
		if (servoPosition < 50) {
			servoPosition = 50;
		}

		servo.write(servoPosition);
		previousMillis = millis();
	}
}