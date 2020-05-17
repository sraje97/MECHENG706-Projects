#include <MECHENG706.h>
#include <LightTracker.h>

LightTracker lightTracker = LightTracker();

void setup() {
  Serial.begin(9600);
  lightTracker.Init();
}

void loop() {
//  if (lightTracker.FireDetected()) {
    lightTracker.Track();
    lightTracker.Debug();
    delay(20);
//  }
}
