#include "Manipulator.h"
#include "Dvig.h"
#include "Camera.h"
#include "SoftwareSerial.h"

#define RS485_CONTROL_PIN 2

#define LEFT_MOTOR_PIN1 3
#define RIGHT_MOTOR_PIN1 4
#define LEFT_MOTOR_PIN2 5
#define RIGHT_MOTOR_PIN2 6
#define VERTICAL_MOTOR_PIN1 7
#define VERTICAL_MOTOR_PIN2 8

#define MANIPULATOR_PIN 9
#define CAMERA_PIN 10

#define RX 11
#define TX 12

Dvig leftMotor1(LEFT_MOTOR_PIN1);
Dvig rightMotor1(RIGHT_MOTOR_PIN1);
Dvig leftMotor2(LEFT_MOTOR_PIN2);
Dvig rightMotor2(RIGHT_MOTOR_PIN2);
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
  leftMotor1.init();
  rightMotor1.init();
  leftMotor2.init();
  rightMotor2.init();
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

    uint8_t buffer[10] = {};
    buffer[0] = first_received_byte;
    RS485.readBytes(buffer + 1, 9);

    if (buffer[0] == START_BYTE && buffer[9] == END_BYTE) {
      leftMotor1.set_power(buffer[1]);
      rightMotor1.set_power(-buffer[2]);
      leftMotor2.set_power(buffer[3]);
      rightMotor2.set_power(-buffer[4]);
      verticalMotor1.set_power(buffer[5]);
      verticalMotor2.set_power(-buffer[5]);
      camera.rotate(buffer[6] * 3);
      manipulator.rotate(buffer[7] * 20);
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
      Serial.print(" ");
      Serial.println((int8_t)buffer[7]);
      Serial.print(" ");
      Serial.println((int8_t)buffer[8]);
#endif  // DEBUG
    } else {
      Serial.println("ERROR");
      Serial.println(RS485.readString());
    }
  }
}
