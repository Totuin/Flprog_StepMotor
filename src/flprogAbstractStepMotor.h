#pragma once
#include "Arduino.h"
#include "flprogUtilites.h"

#define FLPROG_STOP_STEP_MOTOR_MODE 0
#define FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE 1
#define FLPROG_POSITION_TRANSITION_STEP_MOTOR_MODE 2
#define FLPROG_GO_STEP_COUNT_STEP_MOTOR_MODE 3
#define FLPROG_FIND_ZERO_STEP_MOTOR_MODE 4

#define FLPROG_STOP_STEP_MOTOR_STATUS 1

#define FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_STATUS 2

#define FLPROG_POSITION_TRANSITION_WAIT_COMMAND_STEP_MOTOR_STATUS 3
#define FLPROG_POSITION_TRANSITION_EXECUTE_COMMAND_STEP_MOTOR_STATUS 4

#define FLPROG_GO_STEP_COUNT_STEP_WAIT_COMMAND_STEP_MOTOR_STATUS 5
#define FLPROG_GO_STEP_COUNT_STEP_EXECUTE_COMMAND_STEP_MOTOR_STATUS 6

#define FLPROG_FIND_ZERO_STEP_MOTOR_STATUS 7
#define FLPROG_END_FIND_ZERO_STEP_MOTOR_STATUS 8



class FLProgAbstractStepMotor : public AbstractFLProgClass
{
public:
    void maxSpeed(uint16_t value); // Максимальная скорость вращения - шаги в секунду
    uint16_t maxSpeed() { return _maxSpeed; };

    uint16_t currenrSpeed() { return _currenrSpeed; }; // текущая скорость вращения шагов в секунду

    void acceleration(uint16_t value); // ускорение - на сколько шагов в секунду увеличится скорость вращения во времени
    uint16_t acceleration() { return _acceleration; };

    void startAccelerationSpeed(uint16_t value) { _startAccelerationSpeed = value; }; // Начальная скорость ускорения шагов в секунду
    uint16_t startAccelerationSpeed() { return _startAccelerationSpeed; };

    void tickPeriod(uint16_t value); // Период вызова прерывания во внешнем таймере  в микросекундах
    uint16_t tickPeriod() { return _tickPeriod; };

    void dir(bool value); // Направление вращения (не работает в режиме позиционирования по шагу).
    bool dir() { return _dir; };

    void setZeroStep(); // установка текущего положения как нулевого шага. в режиме поиска нуля - останавливает поиск.

    void targetStep(int32_t value);                 // переход на определлённый шаг в режиме позиционирования по шагу.
    int32_t currentStep() { return _currentStep; }; // текущее положение в шагах

    void mode(uint8_t value); // переключение текущего режима
    uint8_t mode() { return _mode; };

    void goThroughSteps(uint32_t value); // команда на исполнение определённого количества шагов в режиме движения по количеству шагов

    void invertZeroSensorPin(bool value) { _isInvertedZeroSensorPin = value; };

protected:
    void calculateAccelerationPeriod();
    void checkTargetStep();
    virtual void calculatePulsePeriod() {};
    virtual void calculateCurrentSpeed() {};
    virtual void reverseDir() {};
    bool canExternalChangeDir();
    uint16_t _currenrSpeed = 0;
    uint32_t _accelerationPeriod = 0;
    uint16_t _startAccelerationSpeed = 100;
    uint32_t _startAccelerationPeriodTime;
    bool _accelerationMode = false;
    uint16_t _maxSpeed = 10;
    uint16_t _acceleration = 1000;
    uint16_t _tickPeriod = 10;
    uint8_t _mode = FLPROG_STOP_STEP_MOTOR_MODE;
    int32_t _currentStep = 0;
    int32_t _targetStep = 0;
    uint32_t _goStepCounter = 0;
    bool _workStatus = false;
    bool _dir = false;
    uint8_t _zeroSensorPin = 255;
    bool _isInvertedZeroSensorPin = false;
};