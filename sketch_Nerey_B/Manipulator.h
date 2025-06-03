#include "Arduino.h"
#pragma once
#include "Motor.h"
#include "Timer.h"
#include <SoftServo.h>

class Manipulator : public Motor {
public:
  Manipulator(uint8_t pin);
  void init();
  void tick();
  void set_power(int8_t power);
  void rotate(int8_t power);
private:
  uint8_t m_pin;
  Timer m_timer;
  SoftServo m_driver;
};

Manipulator::Manipulator(uint8_t pin) {
  m_pin = pin;
}

void Manipulator::init() {
  m_driver.attach(m_pin);
  m_driver.delayMode();
  m_driver.writeMicroseconds(1500);
  m_timer.start();
}

void Manipulator::set_power(int8_t power) {
  m_power = constrain(power, -100, 100);
  m_power = m_is_inverse ? m_power * -1 : m_power;
  int pulse = map(m_power, -100, 100, 1000, 2000);
  m_driver.writeMicroseconds(pulse);
}

void Manipulator::rotate(int8_t power) {
  if (!power) return;
  if (m_timer.elapsed() > 30) {
    m_power += power;
    m_power = m_is_inverse ? m_power * -1 : m_power;
    m_power = constrain(m_power, -100, 100);
    int pulse = map(m_power, -100, 100, 1000, 2000);
    m_timer.start();
    m_driver.writeMicroseconds(pulse);
  }
}

void Manipulator::tick() {
  m_driver.tick();
}
