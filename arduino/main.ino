#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <Servo.h>

int IN1 = D1;
int IN2 = D2;
int IN3 = D3;
int IN4 = D4;

const int SARBO_PIN = D5;
const int ledPin     = D8;
const int wifiLedPin = D0;

String command;
ESP8266WebServer server(80);

unsigned long previousMillis = 0;
String sta_ssid = "";
String sta_password = "";

Servo sarbo;
enum SarboState { S_IDLE, S_MOVING_OUT, S_WAITING, S_RETURNING };
SarboState sarboState = S_IDLE;
unsigned long sarboTs = 0;
const int SARBO_TRIGGER_ANGLE = 180;
const int SARBO_HOME_ANGLE    = 0;
const unsigned long SARBO_HOLD_MS = 4000;

void startSarboAction() {
  if (sarboState != S_IDLE) return;
  sarbo.write(SARBO_TRIGGER_ANGLE);
  sarboState = S_MOVING_OUT;
  sarboTs = millis();
}

void sarboStateMachine() {
  unsigned long now = millis();
  switch (sarboState) {
    case S_IDLE: break;
    case S_MOVING_OUT:
      if (now - sarboTs >= 250) {
        sarboState = S_WAITING;
        sarboTs = now;
      }
      break;
    case S_WAITING:
      if (now - sarboTs >= SARBO_HOLD_MS) {
        sarboState = S_RETURNING;
        sarboTs = now;
        sarbo.write(SARBO_HOME_ANGLE);
      }
      break;
    case S_RETURNING:
      if (now - sarboTs >= 300) {
        sarboState = S_IDLE;
      }
      break;
  }
}

void HTTP_handleRoot() {
  server.send(200, "text/html", "");
  if (server.hasArg("State")) {
    Serial.println(server.arg("State"));
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}

void Forward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void Backward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void TurnRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void TurnLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void ForwardLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void BackwardLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void ForwardRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void BackwardRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void Stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void TurnLightOn()  { digitalWrite(ledPin, HIGH); }
void TurnLightOff() { digitalWrite(ledPin, LOW); }

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(wifiLedPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(wifiLedPin, HIGH);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Stop();

  sarbo.attach(SARBO_PIN);
  sarbo.write(SARBO_HOME_ANGLE);

  String chip_id = String(ESP.getChipId(), HEX);
  chip_id = chip_id.substring(chip_id.length() - 4);
  String hostname = "wificar-" + chip_id;

  WiFi.mode(WIFI_STA);
  WiFi.begin(sta_ssid.c_str(), sta_password.c_str());

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  while (WiFi.status() != WL_CONNECTED && currentMillis - previousMillis <= 10000) {
    delay(500);
    currentMillis = millis();
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(wifiLedPin, LOW);
    delay(3000);
  } else {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(hostname.c_str());
    digitalWrite(wifiLedPin, HIGH);
    delay(3000);
  }

  server.on("/", HTTP_handleRoot);
  server.onNotFound(HTTP_handleRoot);
  server.begin();

  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  sarboStateMachine();

  command = server.arg("State");

  if (command == "F") Forward();
  else if (command == "B") Backward();
  else if (command == "R") TurnRight();
  else if (command == "L") TurnLeft();
  else if (command == "G") ForwardLeft();
  else if (command == "H") BackwardLeft();
  else if (command == "I") ForwardRight();
  else if (command == "J") BackwardRight();
  else if (command == "S") Stop();

  if (command == "V") startSarboAction();

  if (command == "W") TurnLightOn();
  else if (command == "w") TurnLightOff();
}
