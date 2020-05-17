#include <MECHENG706.h>
#include <UltrasonicSensor706.h>
#include <MecanumWheel706.h>
#include <LightTracker.h>

UltrasonicSensor706 ultrasonic = UltrasonicSensor706(ULTRASONIC_OUT, ULTRASONIC_IN); 
LightTracker lightTracker = LightTracker();
MecanumWheel706 drive = MecanumWheel706();

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
  }
}

void ExtinguishFire() {
  // Go to fire
  if (ultrasonic.getDistance() > 9) {
    
    // If angle is > +10 deg, turn left. If angle is < -10 deg turn right.
    if (lightTracker.GetAngle() > 10) {
      drive.Move(0,1);
      drive.Turn(RIGHT,75);
    } else if (lightTracker.GetAngle() < -10) {
      drive.Move(0,1);
      drive.Turn(LEFT,75);
    } else {
      // If angle is fairly straight, go straight ahead
      drive.Move(0,100);
    }
    
  } else {
    if (lightTracker.FireDetected()) {
   
      if (lightTracker.GetAngle() > 7) {
        drive.Move(0,1);
        drive.Turn(RIGHT,25);
      } else if (lightTracker.GetAngle() < -7) {
        drive.Move(0,1);
        drive.Turn(LEFT,25);
      } 
      
      // Fire reached (15cm), turn on fan
      drive.Stop();
      digitalWrite(FAN_PIN, HIGH);
    }
  }

  // If fire is extinguished, turn off fan
  if (!lightTracker.FireDetected()) {
    digitalWrite(FAN_PIN, LOW);
  }
}
