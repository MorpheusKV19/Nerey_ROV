#include "GamepadUtils.h"
#include <GW_TX.h>

#define TX_PIN 11

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

GW_TX<TX_PIN> tx;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting MiddleROV…"));
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

  Serial.print("\t");
  Serial.print(getStickState(stickLY));
  Serial.print("\t");
  Serial.print(getStickState(stickLX));
  Serial.print("\t");
  Serial.print(getStickState(stickRY));
  Serial.print("\t");
  Serial.print(getStickState(stickRX));
  Serial.print("\t");
  Serial.println("");

  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(x);
  Serial.print("\t");
  Serial.print(z);
  Serial.print("\t");
  Serial.print(w);
  Serial.print("\t");
  Serial.println("");
#endif

  Data data;
  data.lm1 = getLeftPower1(y, x, w) / getSpeedDivider();
  data.rm1 = getRightPower1(y, x, w) / getSpeedDivider();
  data.lm2 = getLeftPower2(y, x, w) / getSpeedDivider();
  data.rm2 = getRightPower2(y, x, w) / getSpeedDivider();
  data.vm = getVerticalPower(z) / getSpeedDivider();
  data.c = getCamera();
  data.m = getManipulator();

  noInterrupts();
  tx.sendPacket(data);
  interrupts();

  Serial.print("\t");
  Serial.print(data.lm1);
  Serial.print("\t");
  Serial.print(data.rm1);
  Serial.print("\t");
  Serial.print(data.lm2);
  Serial.print("\t");
  Serial.print(data.rm2);
  Serial.print("\t");
  Serial.print(data.vm);
  Serial.print("\t");
  Serial.print(data.c);
  Serial.print("\t");
  Serial.println(data.m);
  Serial.print("\t");
  Serial.print(" / ");
  Serial.println(getSpeedDivider());

  delay(50);
}