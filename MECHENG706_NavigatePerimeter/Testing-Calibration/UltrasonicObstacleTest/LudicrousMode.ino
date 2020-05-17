#include <Servo.h>

const byte left_front = 46;
const byte left_rear = 47;
const byte right_rear = 50;
const byte right_front = 51;

// defines pins numbers
const int trigPin = 48;
const int echoPin = 49;

HardwareSerial *serialCom;

// defines variables
long duration;
int distance;

Servo LF;
Servo LB;
Servo RF;
Servo RB;

void setup() {
  LF.attach(46);
  LB.attach(47);
  RF.attach(51);
  RB.attach(50);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance= duration*0.034/2;

  if (distance <= 15) {
    LF.writeMicroseconds(1500);
    LB.writeMicroseconds(1500);
    RF.writeMicroseconds(1500);
    RB.writeMicroseconds(1500);
  } else {
    LF.writeMicroseconds(2000);
    LB.writeMicroseconds(2000);
    RF.writeMicroseconds(1000);
    RB.writeMicroseconds(1000);
  }
}






