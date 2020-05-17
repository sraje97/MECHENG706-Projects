#define IRPIN A12

String input;
int readings[10];

void setup() {
  pinMode(IRPIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  input = Serial.readString();
  // When a newline character is received
  if (input.indexOf("\n") != -1) {
    // Take readings
    for (int i = 0; i < 10; i++) {
      readings[i] = analogRead(IRPIN);
    }
    // Print readings
    for (int i = 0; i < 10; i++) {
      Serial.println(readings[i]);
    }
    Serial.println("---");
  }
}
