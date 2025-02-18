#pragma once
#include "Arduino.h"
#include "flprogUtilites.h"

#define FLPROG_STOP_STEP_MOTOR_MODE 0
#define FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE 1
#define FLPROG_POSITION_TRANSITION_STEP_MOTOR_MODE 2

class FLProgAbstractStepMotor
{
public:
    void maxSpeed(uint16_t value);     // Максимальная скорость вращения - шаги в секунду
    void acceleration(uint16_t value); // ускорение - на сколько шагов в секунду увеличится скорость вращения во времени
    void tickPeriod(uint16_t value);   // Период вызова прерывания во внешнем таймере  в микросекундах
    void dir(bool value);              // Направление вращения
    bool dir() { return _dir; };

    uint16_t maxSpeed() { return _maxSpeed; };
    uint16_t acceleration() { return _acceleration; };
    uint16_t tickPeriod() { return _tickPeriod; };

    void setZeroStep();
    void targetStep(int32_t value);
    int32_t currentStep() { return _currentStep; };

    void mode(uint8_t value);

protected:
    void calculateAccelerationPeriod();
    void checkTargetStep();
    virtual void calculatePulsePeriod() {};
    virtual void calculateCurrentSpeed() {};
    virtual void reverseDir() {};
    uint16_t _currenrSpeed = 0;
    uint16_t _accelerationPeriod = 0;
    uint32_t _startAccelerationPeriodTime;
    bool _accelerationMode = false;
    uint16_t _maxSpeed = 10;
    uint16_t _acceleration = 1000;
    uint16_t _tickPeriod = 10;
    uint8_t _mode = FLPROG_STOP_STEP_MOTOR_MODE;
    int32_t _currentStep = 0;
    int32_t _targetStep = 0;
    bool _workStatus = false;
    bool _dir = false;
};