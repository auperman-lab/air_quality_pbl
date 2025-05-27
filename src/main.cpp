#include <Arduino.h>
#include <ESP8266WiFi.h>

const int HALL_SENSOR_1_PIN = D1;
const int HALL_SENSOR_2_PIN = D2;
const int HALL_SENSOR_3_PIN = D6;
const int HALL_SENSOR_4_PIN = RX;

// // Motor control pins
const int MOTOR1_IN1_PIN = D5;    // Connected to L298N IN1 for Motor 1
const int MOTOR1_IN2_PIN = D4;    // Connected to L298N IN2 for Motor 1
const int MOTOR2_IN1_PIN = D7;    // Connected to L298N IN3 for Motor 2
const int MOTOR2_IN2_PIN = D8; // Connected to L298N IN4 for Motor 2 (GPIO10)

#define BLYNK_TEMPLATE_ID "TMPL4UfN9BxZg"
#define BLYNK_TEMPLATE_NAME "wemostest"
#define BLYNK_AUTH_TOKEN "CM3k4BaH2pDS0wvRxtgLYS3GXOCKSjqB"

#include <BlynkSimpleEsp8266.h>
#include <WidgetTerminal.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "MTC-k9pn";
char pass[] = "gZSpR9t3";

WidgetTerminal terminal(V5);

// Functions to control Motor 1
void motor1Forward() {
  digitalWrite(MOTOR1_IN1_PIN, HIGH);
  digitalWrite(MOTOR1_IN2_PIN, LOW);
  terminal.println("Motor 1: FORWARD");
  terminal.flush();
}

void motor1Reverse() {
  digitalWrite(MOTOR1_IN1_PIN, LOW);
  digitalWrite(MOTOR1_IN2_PIN, HIGH);
  terminal.println("Motor 1: REVERSE");
  terminal.flush();
}

void motor1Stop() {
  digitalWrite(MOTOR1_IN1_PIN, LOW);
  digitalWrite(MOTOR1_IN2_PIN, LOW);
  terminal.println("Motor 1: STOP");
  terminal.flush();
}

// Functions to control Motor 2
void motor2Forward() {
  digitalWrite(MOTOR2_IN1_PIN, HIGH);
  digitalWrite(MOTOR2_IN2_PIN, LOW);
  terminal.println("Motor 2: FORWARD");
  terminal.flush();
}

void motor2Reverse() {
  digitalWrite(MOTOR2_IN1_PIN, LOW);
  digitalWrite(MOTOR2_IN2_PIN, HIGH);
  terminal.println("Motor 2: REVERSE");
  terminal.flush();
}

void motor2Stop() {
  digitalWrite(MOTOR2_IN1_PIN, LOW);
  digitalWrite(MOTOR2_IN2_PIN, LOW);
  terminal.println("Motor 2: STOP");
  terminal.flush();
}

// Blynk callback for Motor 1 control (Virtual Pin V6)
BLYNK_WRITE(V6) {
  int controlValue = param.asInt();
  if (controlValue == 1) {
    motor1Forward();
  } else if (controlValue == 2) {
    motor1Reverse();
  } else { // controlValue == 0 or any other value
    motor1Stop();
  }
}

// Blynk callback for Motor 2 control (Virtual Pin V7)
BLYNK_WRITE(V7) {
  int controlValue = param.asInt();
  if (controlValue == 1) {
    motor2Forward();
  } else if (controlValue == 2) {
    motor2Reverse();
  } else { // controlValue == 0 or any other value
    motor2Stop();
  }
}

void setup() {
  Serial.begin(115200);

  while (!Serial) {
  }

  Serial.println("\n--- Wemos D1 Mini Hall Sensor & Motor Reader ---");
  terminal.println("\n--- Wemos D1 Mini Hall Sensor & Motor Reader ---");

  Serial.println("Initializing sensors and motors...");
  terminal.println("Initializing sensors and motors...");

  pinMode(HALL_SENSOR_1_PIN, INPUT_PULLUP);
  pinMode(HALL_SENSOR_2_PIN, INPUT_PULLUP);
  pinMode(HALL_SENSOR_3_PIN, INPUT_PULLUP);
  pinMode(HALL_SENSOR_4_PIN, INPUT_PULLUP);

  // Set motor control pins as OUTPUT
  pinMode(MOTOR1_IN1_PIN, OUTPUT);
  pinMode(MOTOR1_IN2_PIN, OUTPUT);
  pinMode(MOTOR2_IN1_PIN, OUTPUT);
  pinMode(MOTOR2_IN2_PIN, OUTPUT);

  // Ensure motors are stopped at startup
  motor1Stop();
  motor2Stop();

  terminal.clear();
  terminal.flush();

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  terminal.print("Connecting to WiFi: ");
  terminal.println(ssid);

  Blynk.begin(auth, ssid, pass);

  Serial.println("All initialized. Reading data and ready for motor control...");
  terminal.println("All initialized. Reading data and ready for motor control...");
  terminal.flush();
}

void loop() {
  Blynk.run();

  int sensor1State = digitalRead(HALL_SENSOR_1_PIN);
  int sensor2State = digitalRead(HALL_SENSOR_2_PIN);
  int sensor3State = digitalRead(HALL_SENSOR_3_PIN);
  int sensor4State = digitalRead(HALL_SENSOR_4_PIN);

  Blynk.virtualWrite(V0, sensor1State);
  Blynk.virtualWrite(V1, sensor2State);
  Blynk.virtualWrite(V2, sensor3State);
  Blynk.virtualWrite(V3, sensor4State);

  String outputString = "";
  outputString += "S1 (D1/GPIO5): ";
  outputString += (sensor1State == LOW ? "DETECTED " : "NO_DETECTION ");

  outputString += "| S2 (D2/GPIO4): ";
  outputString += (sensor2State == LOW ? "DETECTED " : "NO_DETECTION ");

  outputString += "| S3 (D6/GPIO12): ";
  outputString += (sensor3State == LOW ? "DETECTED " : "NO_DETECTION ");

  outputString += "| S4 (RX/GPIO3): ";
  outputString += (sensor4State == LOW ? "DETECTED" : "NO_DETECTION");

  Serial.println(outputString);
  // terminal.println(outputString);
  // terminal.flush();

  delay(1000);
}
