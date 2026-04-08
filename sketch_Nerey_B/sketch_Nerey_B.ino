#include "Manipulator.h"
#include "Dvig.h"
#include "Camera.h"
#include <GyverTransfer.h>

#define LEFT_MOTOR_PIN1 3
#define RIGHT_MOTOR_PIN1 4
#define LEFT_MOTOR_PIN2 5
#define RIGHT_MOTOR_PIN2 6
#define VERTICAL_MOTOR_PIN1 7
#define VERTICAL_MOTOR_PIN2 8

#define MANIPULATOR_PIN 9
#define CAMERA_PIN 10

#define RX_PIN 2

Dvig leftMotor1(LEFT_MOTOR_PIN1);
Dvig rightMotor1(RIGHT_MOTOR_PIN1);
Dvig leftMotor2(LEFT_MOTOR_PIN2);
Dvig rightMotor2(RIGHT_MOTOR_PIN2);
Dvig verticalMotor1(VERTICAL_MOTOR_PIN1);
Dvig verticalMotor2(VERTICAL_MOTOR_PIN2);

Camera camera(CAMERA_PIN);
Manipulator manipulator(MANIPULATOR_PIN);
GyverTransfer<RX_PIN, GT_RX, 5000, 20> rx;

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
  Serial.println("Setup");
  attachInterrupt(digitalPinToInterrupt(RX_PIN), isr, CHANGE);
  leftMotor1.init();
  rightMotor1.init();
  leftMotor2.init();
  rightMotor2.init();
  verticalMotor1.init();
  verticalMotor2.init();
  camera.init();
  manipulator.init();
  delay(1000);
  Serial.println("Entering loop");
}

void isr() {
  rx.tickISR();
}

#define DEBUG

void loop() {
  camera.tick();
  manipulator.tick();
  if (rx.gotData()) {
    digitalWrite(LED_BUILTIN, HIGH);
    Data data;
    if (rx.readDataCRC(data)) {
      leftMotor1.set_power(data.lm1);
      rightMotor1.set_power(-data.rm1);
      leftMotor2.set_power(data.lm2);
      rightMotor2.set_power(-data.rm2);
      verticalMotor1.set_power(data.vm);
      verticalMotor2.set_power(-data.vm);
      camera.rotate(data.c * 3);
      manipulator.rotate(data.m * 20);

#ifdef DEBUG
      Serial.print(data.lm1);
      Serial.print(" ");
      Serial.print(data.rm1);
      Serial.print(" ");
      Serial.print(data.lm2);
      Serial.print(" ");
      Serial.print(data.rm2);
      Serial.print(" ");
      Serial.print(data.vm);
      Serial.print(" ");
      Serial.print(data.c);
      Serial.print(" ");
      Serial.println(data.m);
#endif

    } else {
      Serial.println("ERROR");
    }
    rx.clearBuffer();
  }
}