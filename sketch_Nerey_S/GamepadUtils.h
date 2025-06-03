#pragma once

#include <Arduino.h>

enum GamepadButtons {
  buttonSelect,
  buttonL3,
  buttonR3,
  buttonStart,
  buttonUp,
  buttonRight,
  buttonDown,
  buttonLeft,
  buttonL2,
  buttonR2,
  buttonL1,
  buttonR1,
  buttonTriangle,
  buttonCircle,
  buttonCross,
  buttonSquare,
};

enum GamepadSticks {
  stickLX,
  stickLY,
  stickRX,
  stickRY
};

void readGamepad();
void printButtons();

bool getButtonState(uint16_t index);
byte getStickState(uint16_t index);
