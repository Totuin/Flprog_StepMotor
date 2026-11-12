#pragma once
#include "Arduino.h"
#define FLPROG_PULL_NOT_MODE 0
#define FLPROG_PULL_UP_MODE 1
class AbstractFLProgClass {
protected: uint8_t _status = 0;
public:
  uint8_t getStatus() const { return _status; }
  void setFlags() {}
};
struct FakeRT_HW_Base {
 bool getIsTimerUs(uint32_t start, uint32_t period) const { return (uint32_t)(micros()-start)>=period; }
};
extern FakeRT_HW_Base RT_HW_Base;
