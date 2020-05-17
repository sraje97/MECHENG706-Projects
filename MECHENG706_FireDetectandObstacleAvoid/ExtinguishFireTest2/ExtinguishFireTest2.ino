#include <MECHENG706.h>
#include <UltrasonicSensor706.h>
#include <MecanumWheel706.h>
#include <LightTracker.h>

UltrasonicSensor706 ultrasonic = UltrasonicSensor706(ULTRASONIC_OUT, ULTRASONIC_IN); 
LightTracker lightTracker = LightTracker();
MecanumWheel706 drive = MecanumWheel706();

bool inRangeOfFire = false;

void setup() {
  drive.Init(FL_MOTOR_PIN, FR_MOTOR_PIN, BL_MOTOR_PIN, BR_MOTOR_PIN);
  ultrasonic.clearTrigPin();
  lightTracker.Init();

  pinMode(FAN_PIN, OUTPUT);
}

void loop() {
  if (lightTracker.FireDetected()) {
    lightTracker.Track();
    ExtinguishFire();
  } else {
    drive.Move(0,1);
    drive.Turn(LEFT, 100);
  }
}

void ExtinguishFire() {
  // Go to fire
  if (ultrasonic.getDistance() > 9 && !inRangeOfFire) {
    drive.Move(0,100);
    drive.Turn(lightTracker.GetAngle() < 0, 4*abs(lightTracker.GetAngle()));
  } else {
    if (lightTracker.FireDetected()) {
      inRangeOfFire = true;
      drive.Stop();
      digitalWrite(FAN_PIN, HIGH);
      float desiredDistance = 4;
      if (abs(lightTracker.GetAngle()) > 3) {
        drive.Move(0,1);
        drive.Turn(lightTracker.GetAngle() < 0, 2*abs(lightTracker.GetAngle()));
      } else {
        if (ultrasonic.getDistance() < 100 && ultrasonic.getDistance() > desiredDistance) {
          drive.Move(0, 10*(ultrasonic.getDistance()-desiredDistance));
        } else {
          if (ultrasonic.getDistance() < desiredDistance) {
            drive.Move(180, 10*abs(ultrasonic.getDistance() - desiredDistance));
          } else {
            drive.Move(180, 20);
          }
        }
      }
    }
  }

  // If fire is extinguished, turn off fan
  if (!lightTracker.FireDetected() && inRangeOfFire) {
    digitalWrite(FAN_PIN, LOW);
    inRangeOfFire = false;
    // Reverse ready to start the next run
    drive.Move(180, 50);
    delay(500);
    drive.Stop();
  }
}
