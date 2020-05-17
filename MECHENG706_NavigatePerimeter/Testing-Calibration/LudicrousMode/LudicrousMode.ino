#include <Servo.h>

const byte left_front = 46;
const byte left_rear = 47;
const byte right_rear = 50;
const byte right_front = 51;

Servo LF;
Servo LB;
Servo RF;
Servo RB;

void setup() {
  LF.attach(46);
  LB.attach(47);
  RF.attach(51);
  RB.attach(50);

  LF.writeMicroseconds(2000);
  LB.writeMicroseconds(2000);
  RF.writeMicroseconds(1000);
  RB.writeMicroseconds(1000);
}

void loop() {

}
