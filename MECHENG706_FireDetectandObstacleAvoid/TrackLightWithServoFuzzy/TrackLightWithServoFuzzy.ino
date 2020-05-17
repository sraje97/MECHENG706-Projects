#include <Servo.h>
#define PHOTOTRANSISTOR1 A12
#define PHOTOTRANSISTOR2 A13
#define PHOTOTRANSISTOR3 A14
#define PHOTOTRANSISTOR4 A15

Servo servo;
int servoPosition = 115;
int servoReset = 115;

// Fuzzy Logic variables
//const int dimUpper = 10;
//const int brightLower = 767;

int dimUpper = 10;
int brightLower = 767;

const int leftLower = -4;
const int leftUpper = -2;
const int rightLower = 2;
const int rightUpper = 4;

int PTPins[4] = {PHOTOTRANSISTOR1, PHOTOTRANSISTOR2, PHOTOTRANSISTOR3, PHOTOTRANSISTOR4};
int PTReadings[4];
float PTFuzzy[4][2];

void setup() {
  Serial.begin(9600);

  pinMode(PHOTOTRANSISTOR1, INPUT);
  pinMode(PHOTOTRANSISTOR2, INPUT);
  pinMode(PHOTOTRANSISTOR3, INPUT);
  pinMode(PHOTOTRANSISTOR4, INPUT);

  servo.attach(45);

  servo.write(servoReset);
}

void loop() {
  // Read phototransistors
  for (int i = 0; i < 4; i++) {
    PTReadings[i] = analogRead(PTPins[i]);
    Serial.print(PTReadings[i]);
    Serial.print(',');
  }

  dimUpper = min(min(PTReadings[0],PTReadings[1]),min(PTReadings[2],PTReadings[3]));
  brightLower = max(max(PTReadings[0],PTReadings[1]),max(PTReadings[2],PTReadings[3]));
  if (dimUpper < 10) { dimUpper = 10;}
  
  // Fuzzify phototransistors
  for (int i = 0; i < 4; i++) {
    if (PTReadings[i] < dimUpper) {
      PTFuzzy[i][0] = 1;
      PTFuzzy[i][1] = 0;
    } else if (PTReadings[i] > brightLower) {
      PTFuzzy[i][0] = 0;
      PTFuzzy[i][1] = 1;
    } else {
      PTFuzzy[i][0] = (float)(brightLower - PTReadings[i]) / (brightLower - dimUpper);
      PTFuzzy[i][1] = (float)(PTReadings[i] - dimUpper) / (brightLower - dimUpper);
    }
  }
  Serial.println();

  // Apply Fuzzy Rules
  float fuzzyRules[3];  // {L, 0, R}
  fuzzyRules[0] = min(min(PTFuzzy[0][1], PTFuzzy[2][0]), PTFuzzy[3][0]); // BXDD -> L
  fuzzyRules[0] = max(fuzzyRules[0], min(min(min(PTFuzzy[0][0], PTFuzzy[1][1]), PTFuzzy[2][0]), PTFuzzy[3][0]));  //DBDD -> L
  fuzzyRules[1] = min(PTFuzzy[1][1], PTFuzzy[2][1]); // XBBX -> 0
  fuzzyRules[2] = min(min(PTFuzzy[0][0], PTFuzzy[1][0]), PTFuzzy[3][1]);  // DDXB -> R
  fuzzyRules[2] = max(fuzzyRules[2], min(min(min(PTFuzzy[0][0], PTFuzzy[1][0]), PTFuzzy[2][1]), PTFuzzy[3][1]));  // DDBD -> R

  for (int i = 0; i < 3; i++) {
    Serial.print(fuzzyRules[i]);
    Serial.print(',');
  }

  Serial.println();

  // Defuzzification
  float intersections[4];
  intersections[0] = -0.25 - 0.25 * fuzzyRules[0];
  intersections[1] = -0.5 + 0.25 * fuzzyRules[1];
  intersections[2] = 0.5 - 0.25 * fuzzyRules[1];
  intersections[3] = 0.25 + 0.25 * fuzzyRules[2];

  // MoM Defuzzification
  //  float fuzzyAction = 0;
  //  float fuzzyRulesMax = max(max(fuzzyRules[0], fuzzyRules[1]), fuzzyRules[2]);



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
    Serial.print("All 0");
  }
  else if ((fuzzyRules[0] == 0) and (fuzzyRules[1] == 0)) { // First two are 0
    if (fuzzyRules[1] > fuzzyRules[2]) {
      centroidNumerator += 0.5 * (intersections[3] - intersections[2]) * (fuzzyRules[1] + fuzzyRules[2]) * (intersections[2] + ((intersections[3] - intersections[2]) * (2 * fuzzyRules[2] + fuzzyRules[1])) / (3 * (fuzzyRules[1] + fuzzyRules[2])));
    } else {
      centroidNumerator += 0.5 * (intersections[3] - intersections[2]) * (fuzzyRules[1] + fuzzyRules[2]) * (intersections[3] - ((intersections[3] - intersections[2]) * (2 * fuzzyRules[1] + fuzzyRules[2])) / (3 * (fuzzyRules[1] + fuzzyRules[2])));
    }
    Serial.print("First two are 0");
  }
  else if ((fuzzyRules[1] == 0) and (fuzzyRules[2] == 0)) { // Last two are 0
    if (fuzzyRules[0] > fuzzyRules[1]) {
      centroidNumerator += 0.5 * (intersections[1] - intersections[0]) * (fuzzyRules[0] + fuzzyRules[1]) * (intersections[0] + ((intersections[1] - intersections[0]) * (2 * fuzzyRules[1] + fuzzyRules[0])) / (3 * (fuzzyRules[0] + fuzzyRules[1])));
    } else {
      centroidNumerator += 0.5 * (intersections[1] - intersections[0]) * (fuzzyRules[0] + fuzzyRules[1]) * (intersections[1] - ((intersections[1] - intersections[0]) * (2 * fuzzyRules[0] + fuzzyRules[1])) / (3 * (fuzzyRules[0] + fuzzyRules[1])));
    }
    Serial.print("Last two are 0");
  }
  else {
    if (fuzzyRules[0] > fuzzyRules[1]) { // All different levels
      centroidNumerator += 0.5 * (intersections[1] - intersections[0]) * (fuzzyRules[0] + fuzzyRules[1]) * (intersections[0] + ((intersections[1] - intersections[0]) * (2 * fuzzyRules[1] + fuzzyRules[0])) / (3 * (fuzzyRules[0] + fuzzyRules[1])));
    } else {
      centroidNumerator += 0.5 * (intersections[1] - intersections[0]) * (fuzzyRules[0] + fuzzyRules[1]) * (intersections[1] - ((intersections[1] - intersections[0]) * (2 * fuzzyRules[0] + fuzzyRules[1])) / (3 * (fuzzyRules[0] + fuzzyRules[1])));
    }
    if (fuzzyRules[1] > fuzzyRules[2]) {
      centroidNumerator += 0.5 * (intersections[3] - intersections[2]) * (fuzzyRules[1] + fuzzyRules[2]) * (intersections[2] + ((intersections[3] - intersections[2]) * (2 * fuzzyRules[2] + fuzzyRules[1])) / (3 * (fuzzyRules[1] + fuzzyRules[2])));
    } else {
      centroidNumerator += 0.5 * (intersections[3] - intersections[2]) * (fuzzyRules[1] + fuzzyRules[2]) * (intersections[3] - ((intersections[3] - intersections[2]) * (2 * fuzzyRules[1] + fuzzyRules[2])) / (3 * (fuzzyRules[1] + fuzzyRules[2])));
    }
    Serial.print("All different levels");
  }

  Serial.println();
  // Calculate the centroid
  float fuzzyAction;
  if ((fuzzyRules[0] == 0) and (fuzzyRules[1] == 0) and (fuzzyRules[2] == 0)) {
    fuzzyAction = 0;
  } else {
    fuzzyAction = centroidNumerator / totalArea;
    // Subtract 1 so that the range is from -1 to 1
    fuzzyAction -= 1;
  }

  Serial.print(centroidNumerator);
  Serial.print(',');
  Serial.print(totalArea);
  Serial.print(',');
  Serial.println(fuzzyAction);

  servoPosition -= round(fuzzyAction * 10);

  if (servoPosition > 150) {
    servoPosition = 150;
  }
  if (servoPosition < 50) {
    servoPosition = 50;
  }
  servo.write(servoPosition);
}
