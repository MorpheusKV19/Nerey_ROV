#pragma once

class Motor {
public:
  virtual void init() = 0;
  virtual void set_power(int8_t power) = 0;
  void set_inverse(bool inverse);
  bool get_inverse();
  int8_t get_power();
protected:
  int8_t m_power;
  bool m_is_inverse;
};

bool Motor::get_inverse() {
  return m_is_inverse;
}

void Motor::set_inverse(bool inverse) {
  m_is_inverse = inverse;
}

int8_t Motor::get_power() {
  return m_power;
}

