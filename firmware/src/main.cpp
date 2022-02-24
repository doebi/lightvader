#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char *SSID = "/dev/lol";
const char *PWD = "4dprinter";

#define PIN_ADC 34
#define PIN_MOTOR_A 22
#define PIN_MOTOR_B 23

float MAX_ADC = 4095;
float val = 0;
float last = 0;
float threshold = 0.02;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
char *mqttServer = "mqtt.devlol.org";
int mqttPort = 1883;

float readState() {
  return analogRead(PIN_ADC) / MAX_ADC;
}

void connectToWiFi() {
  Serial.print("Connecting to ");

  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected.");
}

void setState(float newVal) {
  val = readState();

  if (val == newVal) return;

  if (val < newVal) {
    while (val < newVal) {
      digitalWrite(PIN_MOTOR_A, HIGH);
      digitalWrite(PIN_MOTOR_B, LOW);
      val = readState();
    }
  }

  if (val > newVal) {
    while (val > newVal) {
      digitalWrite(PIN_MOTOR_A, LOW);
      digitalWrite(PIN_MOTOR_B, HIGH);
      val = readState();
    }
  }

  digitalWrite(PIN_MOTOR_A, LOW);
  digitalWrite(PIN_MOTOR_B, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  setState(String((char*)payload).toFloat());
}

void setup() {
  Serial.begin(115200);
  Serial.println("setup");

  pinMode(PIN_ADC, INPUT);
  pinMode(PIN_MOTOR_A, OUTPUT);
  pinMode(PIN_MOTOR_B, OUTPUT);

  last = readState();

  connectToWiFi();
  mqtt.setServer(mqttServer, mqttPort);
  mqtt.setCallback(callback);
}

void demo() {
  for (float i = 0; i <= 1; i += 0.1) {
    setState(i);
    delay(50);
  }
  for (float i = 1; i > 0; i -= 0.25) {
    setState(i);
    delay(100);
  }
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqtt.connected()) {
    Serial.println("Reconnecting to MQTT Broker..");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (mqtt.connect(clientId.c_str())) {
      Serial.println("Connected.");
      // subscribe to topic
      mqtt.subscribe("doebi/zigvader/out");
    }

  }
}
void loop() {
  if (!mqtt.connected())
    reconnect();
  mqtt.loop();
  val = readState();
  //Serial.println(val);
  if (max(val, last) - min(val, last) >= threshold) {
    Serial.println("change detected");
    Serial.println(val);
    mqtt.publish("doebi/zigvader/in", String(val).c_str());
    last = val;
  }
  delay(10);
}
