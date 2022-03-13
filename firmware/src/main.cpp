#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char *SSID = "DTNSLT";
const char *PWD = "datensalat.at";

bool canSend = false;

#define PIN_ADC 34
#define PIN_MOTOR_A 22
#define PIN_MOTOR_B 23

float MAX_ADC = 4095;
float val = 0;
float last = 0;
float wanted = 0;
float threshold = 0.02;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
char *mqttServer = "mqtt.devlol.org";
int mqttPort = 1883;

String clientId = "esp-" + String(random(0xffff), HEX);

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

String getIdFromTopic(char* topic) {
  String id = "";

  int slashCount = 0;
  for(int i =0; i < strlen(topic); i++ ) {
    char c = topic[i];
    if (c == '/'){
      slashCount++;
    } else {
      if (slashCount == 2) id.concat(c);
      if (slashCount >= 3) break;
    }
  }

  return id;
}

void setState(float newVal) {

  if (newVal > 1) newVal = 1;
  if (newVal < 0) newVal = 0;
  wanted = newVal;

  val = readState();
  if (val == newVal) return;

  while (val < newVal && val != newVal) {
    digitalWrite(PIN_MOTOR_A, HIGH);
    digitalWrite(PIN_MOTOR_B, LOW);
    val = readState();
  }

  while (val > newVal && val != newVal) {
    digitalWrite(PIN_MOTOR_A, LOW);
    digitalWrite(PIN_MOTOR_B, HIGH);
    val = readState();
  }

  digitalWrite(PIN_MOTOR_A, LOW);
  digitalWrite(PIN_MOTOR_B, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (getIdFromTopic(topic) != clientId) {
    canSend = false;
    payload[length] = '\n';
    setState(String((char*)payload).toFloat());
  }
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

    if (mqtt.connect(String("ESP-"+String(clientId)).c_str())) {
      Serial.println("Connected.");
      // subscribe to topic
      mqtt.subscribe("doebi/vader/+/in");
    }

  }
}
void loop() {
  if (!mqtt.connected())
    reconnect();
  mqtt.loop();
  val = readState();
  if (max(val, last) - min(val, last) >= threshold) {
    if (canSend) {
      mqtt.publish(String("doebi/vader/"+String(clientId)+"/in").c_str(), String(val).c_str());
    }
    last = val;
  }
  if (!canSend && max(val, wanted) - min(val, wanted) <= threshold) {
    canSend = true;
  }
  delay(10);
}
