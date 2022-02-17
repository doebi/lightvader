#include <Arduino.h>

#define PIN_ADC 36
#define PIN_MOTOR_A 16
#define PIN_MOTOR_B 17

int val = 0;

void setup() {
  Serial.begin(115200);

  Serial.println("setup");
  pinMode(PIN_ADC, INPUT);

  pinMode(PIN_MOTOR_A, OUTPUT);
  pinMode(PIN_MOTOR_B, OUTPUT);
}

// Standard Arduino function that is called in an endless loop after setup
void loop() {

  // A
  Serial.println("A");
  digitalWrite(PIN_MOTOR_A, LOW);
  digitalWrite(PIN_MOTOR_B, HIGH);
  delay(2000);

  // B
  Serial.println("B");
  digitalWrite(PIN_MOTOR_A, HIGH);
  digitalWrite(PIN_MOTOR_B, LOW);
  delay(2000);

  //val = analogRead(PIN_ADC);
  //Serial.println(val);
}
