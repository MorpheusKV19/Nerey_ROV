#include "Manipulator.h"
#include "Dvig.h"
#include "Camera.h"
#include <GW_RX.h>

#define RX_PIN 2

#define LEFT_MOTOR_PIN1 3
#define RIGHT_MOTOR_PIN1 4
#define LEFT_MOTOR_PIN2 5
#define RIGHT_MOTOR_PIN2 6
#define VERTICAL_MOTOR_PIN1 7
#define VERTICAL_MOTOR_PIN2 8

#define MANIPULATOR_PIN 9
#define CAMERA_PIN 10

#define DEBUG

Dvig leftMotor1(LEFT_MOTOR_PIN1);
Dvig rightMotor1(RIGHT_MOTOR_PIN1);
Dvig leftMotor2(LEFT_MOTOR_PIN2);
Dvig rightMotor2(RIGHT_MOTOR_PIN2);
Dvig verticalMotor1(VERTICAL_MOTOR_PIN1);
Dvig verticalMotor2(VERTICAL_MOTOR_PIN2);

Camera camera(CAMERA_PIN);
Manipulator manipulator(MANIPULATOR_PIN);
GW_RX<RX_PIN> rx;

struct Data {
  int8_t lm1;
  int8_t rm1;
  int8_t lm2;
  int8_t rm2;
  int8_t vm;
  int8_t c;
  int8_t m;
};

void setup() {
  Serial.begin(9600);
  Serial.println(F("Setup"));
  attachInterrupt(digitalPinToInterrupt(RX_PIN), []() { rx.pinChange(); }, CHANGE);
  leftMotor1.init();
  rightMotor1.init();
  leftMotor2.init();
  rightMotor2.init();
  verticalMotor1.init();
  verticalMotor2.init();
  camera.init();
  manipulator.init();
  delay(1000);
  Serial.println(F("Entering loop"));

  rx.onPacket([](uint8_t type, void* data, size_t len) {
    if (sizeof(Data) != len) { Serial.println(F("ERROR")); return; }
    digitalWrite(LED_BUILTIN, HIGH);
    Data& d = *((Data*)data);

    leftMotor1.set_power(d.lm1);
    rightMotor1.set_power(-d.rm1);
    leftMotor2.set_power(d.lm2);
    rightMotor2.set_power(-d.rm2);
    verticalMotor1.set_power(d.vm);
    verticalMotor2.set_power(-d.vm);
    camera.rotate(d.c * 3);
    manipulator.rotate(d.m * 20);

#ifdef DEBUG
    Serial.print(static_cast<Data*>(data)->lm1);
    Serial.print(F("\t"));
    Serial.print(static_cast<Data*>(data)->rm1);
    Serial.print(F("\t"));
    Serial.print(static_cast<Data*>(data)->lm2);
    Serial.print(F("\t"));
    Serial.print(static_cast<Data*>(data)->rm2);
    Serial.print(F("\t"));
    Serial.print(static_cast<Data*>(data)->vm);
    Serial.print(F("\t"));
    Serial.print(static_cast<Data*>(data)->c);
    Serial.print(F("\t"));
    Serial.println(static_cast<Data*>(data)->m);
#endif

  });
}

void loop() {
  camera.tick();
  manipulator.tick();
  rx.tick();
}