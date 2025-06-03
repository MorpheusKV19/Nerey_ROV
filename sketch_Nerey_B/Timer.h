#pragma once

class Timer {
public:
  Timer();
  void start();
  long elapsed();
  bool is_started();
  void stop();
private:
  long m_start = 0;
  bool m_isStarted = false;
};

Timer::Timer() {
  m_isStarted = false;
}

void Timer::start() {
  m_isStarted = true;
  m_start = millis();
}

long Timer::elapsed() {
  return m_isStarted ? millis() - m_start : 0;
}

bool Timer::is_started() {
  return m_isStarted;
}

void Timer::stop() {
  m_isStarted = false;
}

