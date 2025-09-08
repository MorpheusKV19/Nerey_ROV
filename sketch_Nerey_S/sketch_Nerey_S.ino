#include "GamepadUtils.h"
#include "SoftwareSerial.h"

#define RX 8
#define TX 11

SoftwareSerial RS485(RX, TX);

#define SERIAL_DEBUG Serial
#define SERIAL_CONTROL RS485

#define RS485_CONTROL_PIN 2
#define START_BYTE 0xFE
#define END_BYTE 0xEF

constexpr int MAX_POWER = 100;

int treshold(int v, int b) {
  if (abs(v) < b) return 0;
  return v;
}

int filterAxis(byte axis) {
  int an = getStickState(axis);
  an -= 127;
  an = treshold(an, 10) * -1;
  an = constrain(an, -127, 127);
  int val = map(an, -127, 127, -MAX_POWER, MAX_POWER);
  return val;
}

int8_t getLeftPower1(int y, int x, int w) {
  return constrain(y + x + w, -MAX_POWER, MAX_POWER);
}

int8_t getRightPower1(int y, int x, int w) {
  return constrain(y - x - w, -MAX_POWER, MAX_POWER);
}

int8_t getLeftPower2(int y, int x, int w) {
  return constrain(y - x + w, -MAX_POWER, MAX_POWER);
}

int8_t getRightPower2(int y, int x, int w) {
  return constrain(y + x - w, -MAX_POWER, MAX_POWER);
}

int8_t getVerticalPower(int z) {
  return -constrain(z, -MAX_POWER, MAX_POWER);
}

int8_t checkBtn(uint16_t btn1, uint16_t btn2) {
  if (getButtonState(btn1)) {
    return 1;
  }
  if (getButtonState(btn2)) {
    return -1;
  }
  return 0;
}

int8_t getCamera() {
  int speed = 1;
  return checkBtn(buttonR1, buttonR2) * -speed;
}

int8_t getManipulator() {
  int speed = 1;
  return checkBtn(buttonL1, buttonL2) * speed;
}

int8_t getSpeedDivider() {
  int multiplier = checkBtn(buttonSquare, buttonCircle);
  if (multiplier == 0) {
    return 2;
  } else if (multiplier == 1) {
    return 4;
  } else if (multiplier == -1) {
    return 1;
  }
}

void setup() {
  SERIAL_CONTROL.begin(9600);
  SERIAL_DEBUG.begin(9600);

  pinMode(RS485_CONTROL_PIN, OUTPUT);
  digitalWrite(RS485_CONTROL_PIN, HIGH);

  SERIAL_DEBUG.println(F("Starting MiddleROV…"));
}

int8_t getMotorButton() {
  int speed = 1;
  return checkBtn(buttonTriangle, buttonCross) * speed;
}

void loop() {
  readGamepad();

  int y, x, z, w = 0;
  y = filterAxis(stickLY);
  x = filterAxis(stickLX);
  z = filterAxis(stickRY);
  w = filterAxis(stickRX);

#ifdef DEBUG_OUTPUT
  printButtons();

  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(getStickState(stickLY));
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(getStickState(stickLX));
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(getStickState(stickRY));
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(getStickState(stickRX));
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.println("");

  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(y);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(x);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(z);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(w);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.println("");
#endif

  uint8_t buffer[10];
  buffer[0] = START_BYTE;
  buffer[1] = getLeftPower1(y, x, w) / getSpeedDivider();
  buffer[2] = getRightPower1(y, x, w) / getSpeedDivider();
  buffer[3] = getLeftPower2(y, x, w) / getSpeedDivider();
  buffer[4] = getRightPower2(y, x, w) / getSpeedDivider();
  buffer[5] = getVerticalPower(z) / getSpeedDivider();
  buffer[6] = getCamera();
  buffer[7] = getManipulator();
  buffer[8] = getMotorButton();
  buffer[9] = END_BYTE;

  SERIAL_CONTROL.write(buffer, 10);

  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print((int8_t)buffer[1]);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print((int8_t)buffer[2]);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print((int8_t)buffer[3]);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print((int8_t)buffer[4]);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print((int8_t)buffer[5]);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print((int8_t)buffer[6]);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print((int8_t)buffer[7]);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print((int8_t)buffer[8]);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(" / ");
  SERIAL_DEBUG.println(getSpeedDivider());

  delay(50);
}
