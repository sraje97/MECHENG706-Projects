#include <MECHENG706.h>
#include <UltrasonicSensor706.h>
#include <SharpIR706.h>
#include <Gyroscope.h>
#include <Gyroscope2.h>
#include <MecanumWheel706.h>

MecanumWheel706 drive = MecanumWheel706();
UltrasonicSensor706 ultrasonic = UltrasonicSensor706(ULTRASONIC_OUT, ULTRASONIC_IN);
Gyroscope gyro = Gyroscope();

const double IRoffset = 7.3175;
SharpIR706 frontIR = SharpIR706(IR_TYPE_LONG_NO_OFFSET, IR_F_PIN, 0);
SharpIR706 leftFrontIR = SharpIR706(IR_TYPE_SHORT, IR_LF_PIN, IRoffset);
SharpIR706 leftMidIR = SharpIR706(IR_TYPE_LONG, IR_LM_PIN, IRoffset);
SharpIR706 leftBackIR = SharpIR706(IR_TYPE_SHORT, IR_LB_PIN, IRoffset);

float previousAngle, currentAngle;

void setup() {
//  Serial.begin(9600);
  Serial1.begin(115200);
  drive.Init(FL_MOTOR_PIN, FR_MOTOR_PIN, BL_MOTOR_PIN, BR_MOTOR_PIN);
  delay(5000);  // Time for putty to get set up
//  gyro.Init();
  gyro.gyro_Init();
}

void loop() {
  // On/Off control
  previousAngle = gyro.getAngle();
  currentAngle = previousAngle;
  while (currentAngle < previousAngle + 67) {
    currentAngle = gyro.getAngle();
    drive.Move(0, 100);
    drive.Turn(false, 100);
  }
  drive.Stop();
  delay(1000);
}
