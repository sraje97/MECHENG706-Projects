// Testing the IR rangefinder library

#include <MECHENG706.h>
#include <SharpIR706.h>

SharpIR706 IR_F(IR_TYPE_F, IR_F_PIN);
SharpIR706 IR_LF(IR_TYPE_LF, IR_LF_PIN);
SharpIR706 IR_LM(IR_TYPE_LM, IR_LM_PIN);
SharpIR706 IR_LB(IR_TYPE_LB, IR_LB_PIN);

void setup() {
  Serial.begin(9600);
}

void loop() {
  String input = Serial.readString();
  if (input.indexOf("\n") != -1) {
    if (input == "raw\n") {
      Serial.print("Raw readings: ");
      Serial.print(IR_F.getRawReading());
      Serial.print(", ");
      Serial.print(IR_LF.getRawReading());
      Serial.print(", ");
      Serial.print(IR_LM.getRawReading());
      Serial.print(", ");
      Serial.println(IR_LB.getRawReading());
    } else if (input == "raw continuous\n") {
      while (1) {
        Serial.print(IR_F.getRawReading());
        Serial.print(", ");
        Serial.print(IR_LF.getRawReading());
        Serial.print(", ");
        Serial.print(IR_LM.getRawReading());
        Serial.print(", ");
        Serial.println(IR_LB.getRawReading());
        delay(100);
      }
    } else if (input == "battery\n") {
      pinMode(A0, INPUT);
      int batteryLevelRaw = analogRead(A0);
      int batteryLevel = batteryLevelRaw - 717;
      batteryLevel = batteryLevel * 100;
      batteryLevel = batteryLevel / 143;
      Serial.print("Battery level = ");
      Serial.print(batteryLevel);
      Serial.println("%");
    } else {
      Serial.print("Readings: ");
      Serial.print(IR_F.getBurstDistance());
      Serial.print(", ");
      Serial.print(IR_LF.getBurstDistance());
      Serial.print(", ");
      Serial.print(IR_LM.getBurstDistance());
      Serial.print(", ");
      Serial.println(IR_LB.getBurstDistance());
    }
  }
}
