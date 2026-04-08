#include "GamepadUtils.h"

#define TX_PIN 11
#define GT_MICRO_TX

#include <GyverTransfer.h>
GyverTransfer<TX_PIN, GT_TX, 5000> tx;

#define SERIAL_DEBUG Serial

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

struct Data {
  int8_t lm1;
  int8_t rm1;
  int8_t lm2;
  int8_t rm2;
  int8_t vm;
  int8_t c;
  int8_t m;
};

void
setup() {
  SERIAL_DEBUG.begin(9600);
  SERIAL_DEBUG.println(F("Starting MiddleROV…"));
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

  Data data;
  data.lm1 = getLeftPower1(y, x, w) / getSpeedDivider();
  data.rm1 = getRightPower1(y, x, w) / getSpeedDivider();
  data.lm2 = getLeftPower2(y, x, w) / getSpeedDivider();
  data.rm2 = getRightPower2(y, x, w) / getSpeedDivider();
  data.vm = getVerticalPower(z) / getSpeedDivider();
  data.c = getCamera();
  data.m = getManipulator();

  tx.writeDataCRC(data);

  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(data.lm1);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(data.rm1);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(data.lm2);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(data.rm2);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(data.vm);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(data.c);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.println(data.m);
  SERIAL_DEBUG.print("\t");
  SERIAL_DEBUG.print(" / ");
  SERIAL_DEBUG.println(getSpeedDivider());

  delay(50);
}
