/***********************************
* Filename: SharpIR706.cpp
* Purpose: Take readings from the infrared sensors used for the MECHENG 706 project
* Author: Michael Jessup, MJES247
* Date created: 12/3/18
***********************************/

#include <SharpIR706.h>

SharpIR706::SharpIR706(int setSensorType, uint8_t setSensorPin, double setOffset) {
  sensorType = setSensorType;
  sensorPin = setSensorPin;
  offset = setOffset;
  pinMode(sensorPin, INPUT);
}

double SharpIR706::convertToDistance(double reading) {
  double distance;

  switch (sensorType) {
    case IR_TYPE_SHORT:
      distance = 2076 / (reading - 11);
      distance = clip(distance, 30, 4);
      return distance + offset;
      break;
    case IR_TYPE_LONG:
      distance = 4800 / (reading - 20);
      distance = clip(distance, 80, 10);
      return distance + offset;
      break;
    case IR_TYPE_LONG_NO_OFFSET:
      distance = 4800 / (reading - 20);
      distance = clip(distance, 80, 10);
      return distance;
      break;
    default:
      return -1;
      break;
  }
}

double::SharpIR706::getRawReading() {
  return analogRead(sensorPin);
}

double SharpIR706::getSingleDistance() {
  int ADCreading = analogRead(sensorPin);
  return convertToDistance((double)analogRead(sensorPin));
}

double SharpIR706::getBurstDistance() {
  return getBurstDistance(5, 5);
}

double SharpIR706::getBurstDistance(int numReadings, int delayTime) {
  int cumulativeReadings = 0;
  for (int i = 0; i < numReadings; i++) {
    cumulativeReadings += analogRead(sensorPin);
    delay(delayTime);
  }
  double averageReading = (double)cumulativeReadings / numReadings;
  return convertToDistance(averageReading);
}

double SharpIR706::clip(double valueToClip, double maxVal, double minVal) {
  if (valueToClip > maxVal) { return maxVal; }
  //else if (valueToClip < minVal) { return minVal; }
  else if (valueToClip < minVal) { return maxVal; }
  else { return valueToClip; }
}

bool SharpIR706::validReading() {
  double reading = getBurstDistance();
  switch (sensorType) {
    case IR_TYPE_SHORT:
      return !(reading == 4 + offset || reading == 30 + offset);
      break;
    case IR_TYPE_LONG:
      return !(reading == 10 + offset || reading == 80 + offset);
      break;
    case IR_TYPE_LONG_NO_OFFSET:
      return !(reading == 10 || reading == 80);
      break;
    }
}
