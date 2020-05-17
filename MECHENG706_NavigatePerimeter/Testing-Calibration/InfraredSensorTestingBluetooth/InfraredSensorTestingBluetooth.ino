HardwareSerial *serialCom;

int sensor1 = A7;
int sensor2 = A8;
int sensor3 = A11;

void setup() {
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);

  serialCom = &Serial1;
  serialCom->begin(115200);
  serialCom->println("Starting...");
}

void loop() {
  serialCom->print(analogRead(sensor1));
  serialCom->print(", ");
  serialCom->print(analogRead(sensor2));
  serialCom->print(", ");
  serialCom->println(analogRead(sensor3));
  delay(100);
}
