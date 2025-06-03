#include "GamepadUtils.h"

#include "PsxControllerBitBang.h"

const byte PIN_PS2_ATT = 9;
const byte PIN_PS2_CMD = 6;
const byte PIN_PS2_DAT = 12;
const byte PIN_PS2_CLK = 10;

bool haveController = false;
byte stickState[4];
bool buttonState[PSX_BUTTONS_NO + 1];

PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;

byte psxButtonToIndex(PsxButtons psxButtons) {
    byte i;

    for (i = 0; i < PSX_BUTTONS_NO; ++i) {
        if (psxButtons & 0x01) break;
        psxButtons >>= 1U;
    }

    return i;
}

void clearButtonState() {
    for (byte i = 0; i < PSX_BUTTONS_NO; ++i) {
        buttonState[i] = false;
    }
}

void clearStickState() {
    for (byte i = 0; i < 4; i++) {
        stickState[i] = 128;
    }
}

void dumpSticks() {
    psx.getLeftAnalog(stickState[stickLX], stickState[stickLY]);
    psx.getRightAnalog(stickState[stickRX], stickState[stickRY]);
}

void dumpButtons(PsxButtons psxButtons) {
    static PsxButtons lastB = 0;
	  if (psxButtons == lastB) return;

    lastB = psxButtons;
    clearButtonState();

    for (byte i = 0; i < PSX_BUTTONS_NO; ++i) {
        byte b = psxButtonToIndex(psxButtons);
        buttonState[b] = b < PSX_BUTTONS_NO;
        psxButtons &= ~(1 << b);
    }
}

void printButtons() {
    for (byte i = 0; i < PSX_BUTTONS_NO; ++i) {
        if (buttonState[i]) {
            Serial.print("#");
        } else {
            Serial.print("_");
        }
        Serial.print(" ");
    }

    Serial.println("");
}

void readGamepad() {
    if (!haveController) {
        clearStickState();
        clearButtonState();

        if (psx.begin()) {
            Serial.println(F("Controller found"));
            haveController = true;

            if (!psx.enterConfigMode()) {
                Serial.println(F("Cannot enter config mode"));
            } else {
                if (!psx.enableAnalogSticks()) {
                    Serial.println(F("Cannot enable analog sticks"));
                }
                if (!psx.enableAnalogButtons()) {
                    Serial.println(F("Cannot enable analog buttons"));
                }
                if (!psx.exitConfigMode()) {
                    Serial.println(F("Cannot exit config mode"));
                }
            }

            haveController = true;
        }
    } else {
        if (!psx.read ()) {
            Serial.println(F("Controller lost"));
            haveController = false;
        } else {
            dumpButtons(psx.getButtonWord());
            dumpSticks();
        }
    }
}

bool getButtonState(uint16_t index) {
    return buttonState[index];
}

byte getStickState(uint16_t index) {
    return stickState[index];
}
