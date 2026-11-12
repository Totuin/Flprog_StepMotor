#pragma once
#include "flprogAbstractStepMotor.h"

/** @brief Шаговый двигатель с входами STEP/PUL и DIR внешнего драйвера. */
class FLProgPulDirStepMotor : public FLProgAbstractStepMotor
{
public:
    FLProgPulDirStepMotor(uint8_t pulPin, uint8_t dirPin = 255, uint8_t zeroSensorPin = 255, char zeroSensorPinPullMode = FLPROG_PULL_NOT_MODE);

    void pulseTime(uint16_t value); // Длительность импульса в микросекундах
    void invertDirPin(bool value) { _invertDirPin = value; }; // Полярность DIR
    void invertPulPin(bool value) { _invertPulPin = value; }; // Активный уровень STEP
    void tick(); // функция которую необходимо вызвыть во внешнем прерывании таймера

    void setPulPin(uint8_t pin);
    void setDirPin(uint8_t pin);
 
protected:
    virtual void calculatePulsePeriod();
    virtual void reverseDir();

    uint8_t _pulPin = 255;
    uint8_t _dirPin = 255;
    bool _invertDirPin = false;
    bool _invertPulPin = false;
    uint32_t _pulseTime = 20;
    uint32_t _pulsePeriod = 0; // Ожидаемые тики после фронта STEP (мин. один такт HIGH)
    uint32_t _pulseCounter = 0;
    bool _pullStatus = false;
};
