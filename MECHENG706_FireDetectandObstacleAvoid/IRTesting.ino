#include <MECHENG706.h>
#include <UltrasonicSensor706.h>
#include <MecanumWheel706.h>
#include <LightTracker.h>
#include <SharpIR706.h>

UltrasonicSensor706 ultrasonic = UltrasonicSensor706(ULTRASONIC_OUT, ULTRASONIC_IN); 
LightTracker lightTracker = LightTracker();
MecanumWheel706 drive = MecanumWheel706();

SharpIR706 frontLeftIR = SharpIR706(IR_TYPE_SHORT, IR_FL_PIN, 0);
SharpIR706 frontRightIR = SharpIR706(IR_TYPE_SHORT, IR_FR_PIN, 0);
SharpIR706 backLeftIR = SharpIR706(IR_TYPE_LONG, IR_BL_PIN, 0);
SharpIR706 backRightIR = SharpIR706(IR_TYPE_LONG, IR_BR_PIN, 0);


void setup() {
  // put your setup code here, to run once:
  //  Serial.begin(9600);
  Serial1.begin(115200);
  delay(2000); // TODO: Remove
}

void loop() {
  // put your main code here, to run repeatedly:
  float frontLeftDistance = frontLeftIR.getBurstDistance();
  float frontRightDistance = frontRightIR.getBurstDistance();
  float backLeftDistance = backLeftIR.getBurstDistance();
  float backRightDistance = backRightIR.getBurstDistance();

  Serial1.println(frontLeftDistance);
  Serial1.println(frontRightDistance);
  Serial1.println(backLeftDistance);
  Serial1.println(backRightDistance);
  Serial1.println();
  delay(1000);
}
