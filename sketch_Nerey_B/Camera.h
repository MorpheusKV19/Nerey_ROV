#pragma once
#include <SoftServo.h>
#include "Timer.h"

class Camera {
public:
  Camera(int8_t pin);
  void init();
  void tick();
  void rotate(int8_t angle);
  ~Camera();
private:
  Timer m_timer;
  uint8_t m_angle;
  SoftServo m_driver;
  uint8_t m_pin;
  int8_t m_lastangle = 0;
  int8_t m_defAngle;
};

Camera::Camera(int8_t pin) {
  m_pin = pin;
  m_defAngle = 90;
}

void Camera::init() {
  m_driver.attach(m_pin);
  m_driver.delayMode();
  m_timer.start();
  m_driver.write(m_defAngle);
  m_angle = m_defAngle;
}

Camera::~Camera() {
  m_driver.detach();
}

void Camera::rotate(int8_t angle) {
  if (!angle) return;
  if (m_timer.elapsed() > 30) {
    m_angle += angle;
    m_timer.start();
    m_angle = constrain(m_angle, 10, 170);
    m_driver.write(m_angle);
  }
}

void Camera::tick() {
  m_driver.tick();
}