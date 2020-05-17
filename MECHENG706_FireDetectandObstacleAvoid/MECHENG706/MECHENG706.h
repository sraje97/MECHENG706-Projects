/***********************************
* Filename: MECHENG706.h
* Purpose: Include pin definitions for our MECHENG 706 robot
* Author: Michael Jessup, MJES247
* Date created: 19/3/18
***********************************/

// Phototransistor pins
#define PHOTOTRANSISTOR1 A12
#define PHOTOTRANSISTOR2 A13
#define PHOTOTRANSISTOR3 A14
#define PHOTOTRANSISTOR4 A15

// Servo motor definitions
#define SERVO_PIN 45
#define SERVO_MAX 160
#define SERVO_MIN 95
#define SERVO_MIDDLE 135

// Infrared rangefinder pins
#define IR_FL_PIN A10
#define IR_FR_PIN A4
#define IR_BL_PIN A8
#define IR_BR_PIN A6

// Motor pins
#define FL_MOTOR_PIN 46
#define FR_MOTOR_PIN 51
#define BL_MOTOR_PIN 47
#define BR_MOTOR_PIN 50

// Ultrasonic pins
#define ULTRASONIC_OUT 48
#define ULTRASONIC_IN 49

// Definitions used for WallDetected() function
#define SIDE_WALL true
#define FRONT_WALL false

// Fan pin
#define FAN_PIN 22
