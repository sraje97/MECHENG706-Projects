/***********************************
* Filename: SharpIR706.h
* Purpose: Take readings from the infrared sensors used for the MECHENG 706 project
* Author: Michael Jessup, MJES247
* Date created: 12/3/18
***********************************/

#include <Arduino.h>

#define IR_TYPE_LONG 0
#define IR_TYPE_SHORT 1
#define IR_TYPE_LONG_NO_OFFSET 2

class SharpIR706 {
  public:
    // Constructor
    SharpIR706 (int setSensorType, uint8_t setSensorPin, double setOffset);

    // Public methods
    double getRawReading();
    double getSingleDistance();
    double getBurstDistance();
    double getBurstDistance(int numReadings, int delayTime);
    bool validReading();

  private:
    // Private members
    int sensorType;
    uint8_t sensorPin;
    double offset;

    // Private functions
    double convertToDistance(double reading);
    double clip(double valueToClip, double maxVal, double minVal);
};
