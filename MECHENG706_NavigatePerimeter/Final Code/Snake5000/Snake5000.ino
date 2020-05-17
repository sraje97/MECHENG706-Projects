#include <MECHENG706.h>
#include <UltrasonicSensor706.h>
#include <SharpIR706.h>
#include <Gyroscope.h>
#include <MecanumWheel706.h>

MecanumWheel706 drive = MecanumWheel706();
UltrasonicSensor706 ultrasonic = UltrasonicSensor706(ULTRASONIC_OUT, ULTRASONIC_IN);

const double IRoffset = 8.6; //7.3175
SharpIR706 frontIR = SharpIR706(IR_TYPE_LONG_NO_OFFSET, IR_F_PIN, 0);
SharpIR706 leftFrontIR = SharpIR706(IR_TYPE_SHORT, IR_LF_PIN, IRoffset);
SharpIR706 leftMidIR = SharpIR706(IR_TYPE_LONG, IR_LM_PIN, IRoffset);
SharpIR706 leftBackIR = SharpIR706(IR_TYPE_SHORT, IR_LB_PIN, IRoffset);

double angleError, displacementError;
double ultrasonicDistance;
int turnRight = 0;
 
void setup() {
  Serial1.begin(115200);  // TODO: Remove
  drive.Init(FL_MOTOR_PIN, FR_MOTOR_PIN, BL_MOTOR_PIN, BR_MOTOR_PIN);
  ultrasonic.clearTrigPin();

  OrientToWall();
  for (int i = 0; i < 3; i++) {
    DriveForward();
    TurnRight();
  }
  DriveForward();
  drive.Stop();
}

void loop() {}

void TurnRight() {
  unsigned long turnTime = millis();
  while((millis() - turnTime) < 675) {
    drive.Turn(RIGHT, 100);
  }
  drive.Stop();

  unsigned long previousMillis;
  
  ComputeError(&angleError, &displacementError);

  int counter = 0;
  int timeoutCounter = 0;
  while (angleError > 0.005 || displacementError > 0.1) {
    timeoutCounter = 0;
    // Correct the angle
    bool timeout = false;
    for (int i = 0; i < 3; i++) {
      previousMillis = millis();
      while (abs(angleError) > 0.005) {
        ComputeError(&angleError, &displacementError);
        drive.Move(0, 1);
        drive.Turn(angleError > 0, abs(angleError * 60) + 10);
    
        // Timeout
        if (millis() - previousMillis > 500 || timeout) {
          timeout = true;
          timeoutCounter++;
          break;
        }
      }
      drive.Stop();
      delay(5);
    }
    
    delay(50);
    
    // Correct the displacement
    timeout = false;
    for (int i = 0; i < 3; i++) {
      previousMillis = millis();
      while (abs(displacementError) > 0.1) {
        ComputeError(&angleError, &displacementError);
        drive.Move(displacementError > 0 ? -90 : 90, abs(displacementError * 5) + 10 > 100 ? 100 : abs(displacementError * 5) + 10);
    
        // Timeout
        if (millis() - previousMillis > 500 || timeout) {
          timeout = true;
          timeoutCounter++;
          break;
        }
      }
      drive.Stop();
      delay(5);
    }
    counter++;
    if (counter > 2 || timeoutCounter == 2) {
      break;
    }
  }
}

void DriveForward() {
  ultrasonicDistance = ultrasonic.getDistance();
  while (ultrasonicDistance > 12) {
    ComputeError(&angleError, &displacementError);
    drive.Move(displacementError * -10, 100);
    drive.Turn(angleError > 0, abs(angleError) * 150);
    ultrasonicDistance = ultrasonic.getDistance();
  }
  drive.Stop();
  delay(250);
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
    if (millis() - previousMillis > 1000) {
      break;
    }
  }
  drive.Stop();
  delay(50);
  
  ComputeError(&angleError, &displacementError);
  
  // Align to the side wall
  int counter = 0;
  int timeoutCounter = 0;
  while (angleError > 0.005 || displacementError > 0.1) {
    timeoutCounter = 0;
    
    // Correct the angle
    bool timeout = false;
    for (int i = 0; i < 3; i++) {
      previousMillis = millis();
      while (abs(angleError) > 0.005) {
        ComputeError(&angleError, &displacementError);
        drive.Move(0, 1);
        drive.Turn(angleError > 0, abs(angleError * 60) + 10);
    
        // Timeout
        if (millis() - previousMillis > 500 || timeout) {
          timeout = true;
          timeoutCounter++;
          break;
        }
      }
      drive.Stop();
      delay(5);
    }
    delay(50);
    // Correct the displacement
    timeout = false;
    previousMillis = millis();
    for (int i = 0; i < 3; i++) {
      while (abs(displacementError) > 0.1) {
        ComputeError(&angleError, &displacementError);
        drive.Move(displacementError > 0 ? -90 : 90, abs(displacementError * 5) + 10 > 100 ? 100 : abs(displacementError * 5) + 10);
    
        // Timeout
        if (millis() - previousMillis > 500 || timeout) {
          timeout = true;
          timeoutCounter++;
          break;
        }
      }
      drive.Stop();
      delay(5);
    }
    counter++;
    if (counter > 2 || timeoutCounter == 2) {
      break;
    }
  }
}

// sideWall = true tests for a wall on the left side, otherwise tests for a wall at the front of the robot
bool WallDetected(bool sideWall) {
  bool wallDetected = false;
  if (sideWall) {
    wallDetected = leftFrontIR.validReading() && leftBackIR.validReading();
  } else {
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
  
  if (!leftFrontIR.validReading()) {  // Use mid and back sensors
    *angleError = atan((leftMidDistance - leftBackDistance) / 6.5);
    *displacementError = ((leftMidDistance + leftBackDistance) / 2) - 15;
  } else if (!leftBackIR.validReading()) {  // Use mid and front sensors
    *angleError = atan((leftFrontDistance - leftMidDistance) / 7.5);
    *displacementError = ((leftFrontDistance + leftMidDistance) / 2) - 15;
  } else {  // Use front and back sensors
    *angleError = atan((leftFrontDistance - leftBackDistance) / 14);
    *displacementError = ((leftFrontDistance + leftBackDistance) / 2) - 15;
  }
}
