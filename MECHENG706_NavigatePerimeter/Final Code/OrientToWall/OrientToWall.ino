#include <MECHENG706.h>
#include <MecanumWheel706.h>
#include <UltrasonicSensor.h>
#include <SharpIR706.h>
#include <MECHENG706.h>

MecanumWheel706 drive = MecanumWheel706();
UltrasonicSensor ultrasonic = UltrasonicSensor(ULTRASONIC_OUT, ULTRASONIC_IN);

const double IRoffset = 7.3175;
SharpIR706 frontIR = SharpIR706(IR_TYPE_LONG_NO_OFFSET, IR_F_PIN, 0);
SharpIR706 leftFrontIR = SharpIR706(IR_TYPE_SHORT, IR_LF_PIN, IRoffset);
SharpIR706 leftMidIR = SharpIR706(IR_TYPE_LONG, IR_LM_PIN, IRoffset);
SharpIR706 leftBackIR = SharpIR706(IR_TYPE_SHORT, IR_LB_PIN, IRoffset);

void setup() {
  Serial1.begin(115200);
  drive.Init(FL_MOTOR_PIN, FR_MOTOR_PIN, BL_MOTOR_PIN, BR_MOTOR_PIN);
  ultrasonic.clearTrigPin();

  delay(5000);
  OrientToWall();
}

void loop() {
//  OrientToWall();
}

void OrientToWall() {
  unsigned long previousMillis;
  
  // Move until the side wall is detected
  bool frontWallDetected = WallDetected(FRONT_WALL);  // Storing this here so that the robot picks a direction of rotation and sticks to it
  previousMillis = millis();
  while (!(WallDetected(SIDE_WALL) && !WallDetected(FRONT_WALL))) {
    if (frontWallDetected) {
      drive.Move(0, 1);
      drive.Turn(RIGHT, 100);
    } else {
      drive.Move(0, 1);
      drive.Turn(LEFT, 100);
    }

    // Timeout
    if (millis() - previousMillis > 2000) {
      break;
    }
  }
  drive.Stop();

  // Move so that we are close enough to the side wall to get good readings
  previousMillis = millis();
  while (min(leftFrontIR.getBurstDistance(), leftBackIR.getBurstDistance()) > 20) {
    drive.Move(-90, 50);
    
    // Timeout
    if (millis() - previousMillis > 2000) {
      break;
    }
  }
  drive.Stop();

  // Align to the side wall
  double angleError, displacementError;
  /*while (abs(angleError) > 0.2 || abs(displacementError) > 0.2) {
    ComputeError(&angleError, &displacementError);
  
    // Move closer or further away from the wall as necessary
    drive.Move(displacementError > 0 ? -90 : 90, (displacementError * 5) + 10);
  
    // Turn so that the robot is parallel with the wall
    drive.Turn(angleError > 0, (angleError * 5) + 10);
  }
  drive.Stop();*/

  delay(100);
  ComputeError(&angleError, &displacementError);
  
//  Serial1.println("Correcting angle");
  // Correct the angle
  previousMillis = millis();
  while (abs(angleError) > 0.02) {
    ComputeError(&angleError, &displacementError);
    drive.Move(0, 1);
    drive.Turn(angleError > 0, abs(angleError * 60) + 10);

    // Timeout
    if (millis() - previousMillis > 2000) {
      break;
    }
  }
  drive.Stop();
  delay(100);
//  Serial1.println("Correcting displacement");
  // Correct the displacement
  previousMillis = millis();
  while (abs(displacementError) > 0.3) {
    ComputeError(&angleError, &displacementError);
    drive.Move(displacementError > 0 ? -90 : 90, abs(displacementError * 5) + 10 > 100 ? 100 : abs(displacementError * 5) + 10);

    // Timeout
    if (millis() - previousMillis > 2000) {
      break;
    }
  }
  drive.Stop();
//  Serial1.println("Done");
  
}

// sideWall = true tests for a wall on the left side, otherwise tests for a wall at the front of the robot
bool WallDetected(bool sideWall) {
  bool wallDetected = false;
  if (sideWall) {
    /*if (leftFrontIR.validReading()) {
      wallDetected = true;
    } else if (leftMidIR.validReading()) {
      wallDetected = true;
    } else if (leftBackIR.validReading()) {
      wallDetected = true;
    }*/
    wallDetected = leftFrontIR.validReading() && leftBackIR.validReading();
  } else {
    double ultrasonicDistance;
    ultrasonicDistance = ultrasonic.getDistance();
    if (ultrasonicDistance < 50 && ultrasonicDistance > 10) {
      wallDetected = true;
    } else if (frontIR.validReading() && frontIR.getBurstDistance() < 50) {
      wallDetected = true;
    }
  }

  return wallDetected;
}

void ComputeError(double* angleError, double* displacementError) {
  double leftFrontDistance, leftMidDistance, leftBackDistance;
  leftFrontDistance = leftFrontIR.getBurstDistance();
  leftMidDistance = leftMidIR.getBurstDistance();
  leftBackDistance = leftBackIR.getBurstDistance();
  /*leftFrontDistance = leftFrontIR.getSingleDistance();
  leftMidDistance = leftMidIR.getSingleDistance();
  leftBackDistance = leftBackIR.getSingleDistance();*/
  /*if (!leftFrontIR.validReading() && !leftBackIR.validReading()) {  // Move closer to wall so that the wall is in range (hopefully)
    *angleError = 0;
    *displacementError = 10;
  } else */if (!leftFrontIR.validReading()) {  // Use mid and back sensors
    *angleError = atan((leftMidDistance - leftBackDistance) / 6.5);
    *displacementError = ((leftMidDistance + leftBackDistance) / 2) - 15;
  } else if (!leftBackIR.validReading()) {  // Use mid and front sensors
    *angleError = atan((leftFrontDistance - leftMidDistance) / 7.5);
    *displacementError = ((leftFrontDistance + leftMidDistance) / 2) - 15;
  } else {  // Use front and back sensors
    *angleError = atan((leftFrontDistance - leftBackDistance) / 14);
    *displacementError = ((leftFrontDistance + leftBackDistance) / 2) - 15;
  }

  /*Serial1.print("[");
  Serial1.print(leftFrontDistance);
  Serial1.print(",");
  Serial1.print(leftMidDistance);
  Serial1.print(",");
  Serial1.print(leftBackDistance);
  Serial1.print("]{");
  Serial1.print(*angleError);
  Serial1.print(",");
  Serial1.print(*displacementError);
  Serial1.println("}");*/
}
