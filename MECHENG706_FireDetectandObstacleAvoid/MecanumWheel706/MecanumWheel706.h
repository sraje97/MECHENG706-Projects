/***********************************
* Filename: MecanumWheel706.h
* Purpose: Control the driving base of the mecanum wheel robot
* Author: Michael Jessup, MJES247
* Date created: 26/3/19
***********************************/

#include <Arduino.h>
#include <Servo.h>
#define LEFT true
#define RIGHT false

class MecanumWheel706 {
  public:
    // Constructor
    MecanumWheel706();

    // Methods that control robot movement
    void Init(int FLpin, int FRpin, int BLpin, int BRpin);
    void Stop();
    void Move(int angle, float speed);
    void Turn(bool direction, float speed);
    void Print();

  private:
    // Private members
    Servo FL, FR, BL, BR;
    int FLcurrent, FRcurrent, BLcurrent, BRcurrent;
};
