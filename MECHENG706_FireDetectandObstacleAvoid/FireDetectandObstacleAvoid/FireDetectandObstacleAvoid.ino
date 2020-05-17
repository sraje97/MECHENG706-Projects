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

int numOfFiresExtinguished = 0;
int numOfStrafes = 0;
bool strafedPreviously = false;

void setup() {
  drive.Init(FL_MOTOR_PIN, FR_MOTOR_PIN, BL_MOTOR_PIN, BR_MOTOR_PIN);
  ultrasonic.clearTrigPin();
  lightTracker.Init();

  pinMode(FAN_PIN, OUTPUT);

//  Serial.begin(9600);
  //Serial1.begin(115200);
}

void loop() {
//  lightTracker.Debug();
  if (numOfFiresExtinguished < 2) {
    if (lightTracker.FireDetected()) {
      lightTracker.TrackAndSweep();
      GoToFire();
      if (lightTracker.FireExtinguished()) {
        lightTracker.ResetFireExtinguished();
        numOfFiresExtinguished++;
        numOfStrafes = 0;
      } else if (numOfStrafes < 12 && !lightTracker.FireIsClose()) {
        AvoidObstacle();
      }
    } else {
      drive.Move(0,1);
      drive.Turn(LEFT, 50);
    }
  } else {
    drive.Stop();
  }
}

void GoToFire() {
  // Go to fire
  if (ultrasonic.getBurstDistance() > 10 || (numOfStrafes > 12 && !lightTracker.FireIsClose())) {
    if (lightTracker.IsTracking()) {
      drive.Move(0, 35);
    } else {
      drive.Move(0,20);
    }
    drive.Turn(lightTracker.GetAngle() < 0, 1.5*abs(lightTracker.GetAngle()));
  } else {
    if (lightTracker.FireIsClose()) {
      drive.Stop();
      digitalWrite(FAN_PIN, HIGH);
      float desiredDistance = 5;
      unsigned long fanStartTime = millis();
      while (!lightTracker.FireExtinguished() && millis() - fanStartTime < 10000) {
        lightTracker.Track();
//        lightTracker.Debug();
        if (abs(lightTracker.GetAngle()) > 3) {
          drive.Move(0,1);
          drive.Turn(lightTracker.GetAngle() < 0, 4*abs(lightTracker.GetAngle()));
        } else {
          double ultrasonicDistance = ultrasonic.getBurstDistance();
//          Serial1.println(ultrasonicDistance);
          if (ultrasonicDistance < 100 && ultrasonicDistance > desiredDistance * 1.05) {
            drive.Move(0, 10*(ultrasonicDistance-desiredDistance));
          } else {
            if (ultrasonicDistance < desiredDistance * 0.95) {
              drive.Move(180, 3*abs(ultrasonicDistance - desiredDistance));
            } else {
              drive.Move(180, 5);
            }
          }
        }
      }
      digitalWrite(FAN_PIN, LOW);
      // Reverse ready to start the next run  
      delay(500);
      drive.Move(180, 30);
      delay(300);
      drive.Stop();
      delay(300);
    }
  }
}

void AvoidObstacle() {
  unsigned long prevMillis;
  bool previousStrafeLeft = true;
  float frontLeftDistance = frontLeftIR.getBurstDistance();
  float frontRightDistance  = frontRightIR.getBurstDistance();
  float backLeftDistance = backLeftIR.getBurstDistance();
  float backRightDistance = backRightIR.getBurstDistance();
  if ((frontLeftDistance < 9) && frontRightDistance > 12 && backRightDistance > 20 && !lightTracker.FireIsClose()) {
    drive.Move(90, 50);
    delay(500);
    drive.Move(0,1);
    drive.Turn(LEFT, 50);
    delay(100);
  } else if ((frontRightDistance < 9) && frontLeftDistance > 12 && backLeftDistance > 20 && !lightTracker.FireIsClose()) {
    drive.Move(-90, 50);
    delay(500);
    drive.Move(0,1);
    drive.Turn(RIGHT, 50);
    delay(100);
  } else if ((ultrasonic.getDistance() < 10 || frontLeftDistance < 9 || frontRightDistance < 9) && !lightTracker.FireIsClose()) {
    drive.Stop();
    if (lightTracker.IsTracking() || lightTracker.SweepPos() == -1 || lightTracker.SweepPos() == 1 ||
          (lightTracker.SweepPos() == 0 && (frontRightDistance < 9 || backRightDistance < 20)) ||
          (lightTracker.SweepPos() == 2 && (frontLeftDistance < 9 || backLeftDistance < 20))) {
      // Drive back slightly
      drive.Move(180, 50);
      delay(300);
      //delay(1000);
      bool objectInFront = true;
      while (objectInFront) {
        prevMillis = millis();
        while ((millis() - prevMillis) < 450) {
          drive.Move(0,1);
          drive.Turn(RIGHT, 100);
        }
        prevMillis = millis();
        //int i = 0;
        unsigned long leftDistances = 0;
        unsigned long rightDistances = 0;
        while ((millis() - prevMillis) < 950) {
          drive.Move(0,1);
          drive.Turn(LEFT, 100);
          /*if ((int)frontLeftIR.getSingleDistance() == 4) { leftDistances += 30; } 
          else { leftDistances += (int)frontLeftIR.getSingleDistance(); }
          
          if ((int)backLeftIR.getSingleDistance() == 10) { leftDistances += 80; }
          else { leftDistances += (int)backLeftIR.getSingleDistance(); }
          
          if ((int)backRightIR.getSingleDistance() == 10) { rightDistances += 80; }
          else { rightDistances += (int)backRightIR.getSingleDistance(); }
          
          if ((int)frontRightIR.getSingleDistance() == 4) { rightDistances += 30; }
          else { rightDistances += (int)frontRightIR.getSingleDistance(); }*/
          leftDistances += (int)frontLeftIR.getSingleDistance();
          leftDistances += (int)backLeftIR.getSingleDistance();
          rightDistances += (int)backRightIR.getSingleDistance();
          rightDistances += (int)frontRightIR.getSingleDistance();
          delay(1);
        }
        prevMillis = millis();
        while ((millis() - prevMillis) < 450) {
          drive.Move(0,1);
          drive.Turn(RIGHT, 100);
        }
        drive.Stop();
    
        // Strafe left or right whichever side has no obstacle
        prevMillis = millis();
//        Serial1.println(leftDistances);
//        Serial1.println(rightDistances);
//        Serial1.println(leftDistances > rightDistances ? -90 : 90);
//        Serial1.println("---");
        while ((millis() - prevMillis) < 500) {
          if (strafedPreviously && abs(leftDistances - rightDistances) < 5000) {
            drive.Move(previousStrafeLeft ? -90 : 90, 50);
          }
          drive.Move(leftDistances > rightDistances ? -90 : 90, 50);
          previousStrafeLeft = leftDistances > rightDistances;
        }
        
        // Strafe at 45 degrees
        bool OkTo45 = true;
        frontLeftDistance = frontLeftIR.getBurstDistance();
        frontRightDistance = frontRightIR.getBurstDistance();
        backLeftDistance = backLeftIR.getBurstDistance();
        backRightDistance = backRightIR.getBurstDistance();
        if (previousStrafeLeft) {
          if (frontLeftDistance < 9 || backLeftDistance < 12) {
            OkTo45 = false;
          }
        } else {
          if (frontRightDistance < 9 || backRightDistance < 12) {
            OkTo45 = false;
          }
        }
        if (OkTo45) {
          drive.Move(previousStrafeLeft ? -45 : 45, 50);
          delay(500);
        }
        
        strafedPreviously = true;
        numOfStrafes++;
        drive.Stop();
  
        prevMillis = millis();
        /*while (((millis() - prevMillis) < 500) && frontLeftIR.getSingleDistance() > 7 && frontRightIR.getSingleDistance() > 7) { 
          drive.Move(0,50);
        }*/
  
        // Sweep the area in front to check for new obstacles
        objectInFront = false;
        // Check the two IR sensors
        frontLeftDistance = frontLeftIR.getBurstDistance();
        frontRightDistance = frontRightIR.getBurstDistance();
        int ultrasonicReadings[500];
        int numOfReadings = 0;
        int sum = 0;
        if (frontLeftDistance < 9 || frontRightDistance < 9) {
  //      if (false) {
          objectInFront = true;
        } else {  // Sweep with the ultrasonic
          lightTracker.StartSweep();
          while (!lightTracker.IsTracking()) {
            lightTracker.TrackAndSweep();
            ultrasonicReadings[numOfReadings] = (int)ultrasonic.getDistance();
            numOfReadings++;
            delay(3);
          }
          for(int i = 4; i < numOfReadings; i++) {
            sum = 0;
            for (int j = 0; j < 5; j++) {
              sum += ultrasonicReadings[i-j];
            }
            sum = sum / 5;
          }
          if (sum < 9) {
            objectInFront = true;
          }
        }
      }

      lightTracker.StartSweep();
      // Move forward past the obstacle
      prevMillis = millis();
      /*while (millis() - prevMillis < 500) {
        drive.Move(0,50);
      }*/
    } else if (lightTracker.SweepPos() == 0) {
      drive.Move(90, 50);
      delay(500);
    } else if (lightTracker.SweepPos() == 2) {
      drive.Move(-90, 50);
      delay(500);
    }
  }
}

