#include <Arduino.h>

#define PIN_ADC 34
#define PIN_MOTOR_A 16
#define PIN_MOTOR_B 17

int MAX_ADC = 4095;

float val = 0;

float readState() {
  return analogRead(PIN_ADC) / MAX_ADC;
}

/*
void setState(float newVal) {
  val = readState();

  if (val == newVal) return;

  if (val < newVal) {
    while (val < newVal) {
      pinMode(PIN_MOTOR_A, LOW);
      pinMode(PIN_MOTOR_B, HIGH);
      val = readState();
      Serial.println(val);
      delay(20);
    }
  }

  if (val > newVal) {
    while (val > newVal) {
      pinMode(PIN_MOTOR_A, HIGH);
      pinMode(PIN_MOTOR_B, LOW);
      val = readState();
      Serial.println(val);
    }
  }

  Serial.println(".");
  //pinMode(PIN_MOTOR_A, LOW);
  //pinMode(PIN_MOTOR_B, LOW);
}
*/


void setup() {
  Serial.begin(115200);

  Serial.println("setup");
  pinMode(PIN_ADC, INPUT);

  pinMode(PIN_MOTOR_A, OUTPUT);
  pinMode(PIN_MOTOR_B, OUTPUT);
}

void loop() {

  // A
  Serial.println("A");
  pinMode(PIN_MOTOR_A, LOW);
  pinMode(PIN_MOTOR_B, HIGH);
  //setState(1);
  delay(2000);

  // B
  Serial.println("B");
  pinMode(PIN_MOTOR_A, HIGH);
  pinMode(PIN_MOTOR_B, LOW);
  //setState(0.5);
  delay(2000);

}
