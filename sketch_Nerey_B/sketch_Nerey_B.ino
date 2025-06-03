#include "Manipulator.h"
#include "Dvig.h"
#include "Camera.h"
#include "SoftwareSerial.h"

#define RS485_CONTROL_PIN 2

#define LEFT_MOTOR_PIN 9
#define RIGHT_MOTOR_PIN 5
#define VERTICAL_MOTOR_PIN1 3
#define VERTICAL_MOTOR_PIN2 6

#define MANIPULATOR_PIN 10
#define CAMERA_PIN 11

#define RX 8
#define TX 12

Dvig leftMotor(LEFT_MOTOR_PIN);
Dvig rightMotor(RIGHT_MOTOR_PIN);
Dvig verticalMotor1(VERTICAL_MOTOR_PIN1);
Dvig verticalMotor2(VERTICAL_MOTOR_PIN2);

Camera camera(CAMERA_PIN);
Manipulator manipulator(MANIPULATOR_PIN);

SoftwareSerial RS485(RX, TX);

void setup() {
  Serial.begin(9600);
  RS485.begin(9600);
  Serial.println("Setup");
  // RS485.println("Setup");
  pinMode(RS485_CONTROL_PIN, OUTPUT);
  digitalWrite(RS485_CONTROL_PIN, HIGH);
  leftMotor.init();
  rightMotor.init();
  verticalMotor1.init();
  verticalMotor2.init();
  camera.init();
  manipulator.init();
  delay(1000);

  digitalWrite(RS485_CONTROL_PIN, LOW);
  Serial.println("Entering loop");
}

#define START_BYTE 0xFE
#define END_BYTE 0xEF

#define DEBUG

void loop() {
  camera.tick();
  manipulator.tick();
  if (RS485.available()) {
    digitalWrite(LED_BUILTIN, HIGH);
    uint8_t first_received_byte = 0;
    RS485.readBytes(&first_received_byte, 1);
    if (first_received_byte != START_BYTE) {
      Serial.println("ERROR (wrong first byte)");
      return;
    }

    uint8_t buffer[8] = {};
    buffer[0] = first_received_byte;
    RS485.readBytes(buffer + 1, 7);

    if (buffer[0] == START_BYTE && buffer[7] == END_BYTE) {
      leftMotor.set_power(buffer[1]);
      rightMotor.set_power(-buffer[2]);
      verticalMotor1.set_power(buffer[3]);
      verticalMotor2.set_power(-buffer[3]);
      camera.rotate(buffer[4] * 3);
      manipulator.rotate(buffer[5] * 20);
#ifdef DEBUG
      Serial.print((int8_t)buffer[1]);
      Serial.print(" ");
      Serial.print((int8_t)buffer[2]);
      Serial.print(" ");
      Serial.print((int8_t)buffer[3]);
      Serial.print(" ");
      Serial.print((int8_t)buffer[4]);
      Serial.print(" ");
      Serial.print((int8_t)buffer[5]);
      Serial.print(" ");
      Serial.println((int8_t)buffer[6]);
#endif  // DEBUG
    } else {
      Serial.println("ERROR");
      Serial.println(RS485.readString());
    }
  }
}