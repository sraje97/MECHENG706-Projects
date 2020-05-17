/***********************************
* Filename: MecanumWheel706.h
* Purpose: Control the driving base of the mecanum wheel robot
* Author: Michael Jessup, MJES247
* Date created: 26/3/18
***********************************/

#include <MecanumWheel706.h>

MecanumWheel706::MecanumWheel706() {
  FLcurrent = 1500;
  FRcurrent = 1500;
  BLcurrent = 1500;
  BRcurrent = 1500;
}

void MecanumWheel706::Init(int FLpin, int FRpin, int BLpin, int BRpin) {
  FL.attach(FLpin);
  FR.attach(FRpin);
  BL.attach(BLpin);
  BR.attach(BRpin);
}

void MecanumWheel706::Stop() {
  FL.writeMicroseconds(1500);
  FLcurrent = 1500;
  FR.writeMicroseconds(1500);
  FRcurrent = 1500;
  BL.writeMicroseconds(1500);
  BLcurrent = 1500;
  BR.writeMicroseconds(1500);
  BRcurrent = 1500;
}

// Angle from 0-360, speed from 0-100
// This ignores any current robot movement
void MecanumWheel706::Move(int angle, float speed) {
  double FLpulse, FRpulse, BLpulse, BRpulse;

  // Convert angle to radians
  double angleRadians = (angle * PI) / 180;
  FLpulse = sin(angleRadians + (PI / 4));
  FRpulse = sin(angleRadians - (PI / 4));
  BLpulse = sin(angleRadians + (3 * PI / 4));
  BRpulse = sin(angleRadians - (3 * PI / 4));

  // Scale the wheel speeds based on the speed value
  double maxVal = max(max(abs(FLpulse), abs(FRpulse)), max(abs(BLpulse), abs(BRpulse)));
  FLpulse *= 5 * speed / maxVal;
  FRpulse *= 5 * speed / maxVal;
  BLpulse *= 5 * speed / maxVal;
  BRpulse *= 5 * speed / maxVal;

  FLpulse += 1500;
  FRpulse += 1500;
  BLpulse += 1500;
  BRpulse += 1500;

  // Set the wheel speeds
  FL.writeMicroseconds(FLpulse);
  FLcurrent = FLpulse;
  FR.writeMicroseconds(FRpulse);
  FRcurrent = FRpulse;
  BL.writeMicroseconds(BLpulse);
  BLcurrent = BLpulse;
  BR.writeMicroseconds(BRpulse);
  BRcurrent = BRpulse;
}

// Direction = true if left, speed from 0-100
// This takes into account any current robot movement
// The faster you turn, the less you move (speed = 100 means turning on the spot)
void MecanumWheel706::Turn(bool direction, float speed) {
  /*FLcurrent -= 1500;
  FRcurrent -= 1500;
  BLcurrent -= 1500;
  BRcurrent -= 1500;*/
  if (direction) {  // Turn left
    /*FLcurrent *= speed / 100;
    BLcurrent *= speed / 100;
    FRcurrent += (500 - FRcurrent) * (speed / 100);
    BRcurrent += (500 - BRcurrent) * (speed / 100);*/
    FLcurrent = FLcurrent - (FLcurrent - 1000) * (speed / 100);
    FRcurrent = FRcurrent - (FRcurrent - 1000) * (speed / 100);
    BLcurrent = BLcurrent - (BLcurrent - 1000) * (speed / 100);
    BRcurrent = BRcurrent - (BRcurrent - 1000) * (speed / 100);
  } else {  // Turn right
    /*FRcurrent *= speed / 100;
    BRcurrent *= speed / 100;
    FLcurrent += (500 - FLcurrent) * (speed / 100);
    BLcurrent += (500 - BLcurrent) * (speed / 100);*/
    FLcurrent = FLcurrent + (2000 - FLcurrent) * (speed / 100);
    FRcurrent = FRcurrent + (2000 - FRcurrent) * (speed / 100);
    BLcurrent = BLcurrent + (2000 - BLcurrent) * (speed / 100);
    BRcurrent = BRcurrent + (2000 - BRcurrent) * (speed / 100);
  }
  /*FLcurrent += 1500;
  FRcurrent += 1500;
  BLcurrent += 1500;
  BRcurrent += 1500;*/

  FL.writeMicroseconds(FLcurrent);
  FR.writeMicroseconds(FRcurrent);
  BL.writeMicroseconds(BLcurrent);
  BR.writeMicroseconds(BRcurrent);
}

void MecanumWheel706::Print() {
  Serial.print(FLcurrent);
  Serial.print(", ");
  Serial.print(FRcurrent);
  Serial.print(", ");
  Serial.print(BLcurrent);
  Serial.print(", ");
  Serial.println(BRcurrent);
}
