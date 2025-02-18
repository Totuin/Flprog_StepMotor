#pragma once
#include "flprogAbstractStepMotor.h"

class FLProgPulDirStepMotor : public FLProgAbstractStepMotor
{
public:
    FLProgPulDirStepMotor(uint8_t pulPin, uint8_t dirPin = 255);

    void pulseTime(uint16_t value); // Длительность импульса в микросекундах
    void invertDirPin(bool value) { _invertDirPin = value; };
    void invertPulPin(bool value) { _invertPulPin = value; };
    void tick(); // функция которую необходимо вызвыть во внешнем прерывании таймера
    void pool();

protected:
    virtual void calculatePulsePeriod();
    virtual void calculateCurrentSpeed();
    virtual void reverseDir();

    uint8_t _pulPin = 255;
    uint8_t _dirPin = 255;
    bool _invertDirPin = false;
    bool _invertPulPin = false;
    uint16_t _pulseTime = 20;
    uint16_t _pulsePeriod;
    uint16_t _workPeriod;
    uint16_t _pulseCounter = 0;
    uint16_t _periodCounter = 0;
    bool _pullStatus = false;
};
