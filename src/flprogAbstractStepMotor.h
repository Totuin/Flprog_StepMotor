#pragma once
#include "Arduino.h"
#include "flprogUtilites.h"
#include <stdint.h>

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

/**
 * @brief Общая логика шагового двигателя FLProg.
 *
 * pool() вызывается из основного цикла, tick() — из внешнего периодического
 * таймера, реализованного конкретным наследником. tickPeriod задаёт реальный
 * период этого таймера, а не частоту обращения к pool().
 *
 * ВАЖНО: счётчики положения (int32_t) могут обновляться из прерывания.
 * При чтении на 8-битных микроконтроллерах многобайтовые поля должны
 * считываться атомарно средствами вызывающей программы.
 */
class FLProgAbstractStepMotor : public AbstractFLProgClass
{
public:
    void maxSpeed(uint16_t value); // Максимальная скорость вращения - шаги в секунду
    uint16_t maxSpeed() { return _maxSpeed; };

    uint16_t currenrSpeed(); // Историческое имя API: текущая скорость, шаг/с (не переименовывать)

    void acceleration(int16_t value); // ускорение - на сколько шагов в секунду увеличится скорость вращения во времени
    int16_t acceleration() { return _acceleration; };

    // Минимальная стартовая скорость — 1 шаг/с. Ноль приводил к зависанию разгона.
    void startAccelerationSpeed(uint16_t value) { _startAccelerationSpeed = value ? value : 1; };
    uint16_t startAccelerationSpeed() { return _startAccelerationSpeed; };

    void tickPeriod(uint16_t value); // Период внешнего таймера в мкс; ноль игнорируется
    uint16_t tickPeriod() { return _tickPeriod; };

    void dir(bool value); // Направление вращения (не работает в режиме позиционирования по шагу).
    bool dir() { return _dir; };

    // Привязка текущей позиции к нулю; во время поиска нуля завершает команду.
    void setZeroStep();

    // Установка программного счётчика положения (сам двигатель не перемещается).
    void setCurrentStep(int32_t step);

    void targetStep(int32_t value); // Абсолютная целевая позиция, только в режиме POSITION
    int32_t currentStep() { return _currentStep; }; // текущее положение в шагах

    void mode(uint8_t value); // переключение текущего режима
    uint8_t mode() { return _mode; };

    // Добавляет указанное число шагов к текущей команде режима GO_STEP_COUNT.
    void goThroughSteps(uint32_t value);

    // Инверсия электрического уровня датчика нуля.
    void invertZeroSensorPin(bool value) { _isInvertedZeroSensorPin = value; };

    // Контроль датчика нуля не только в режиме FIND_ZERO.
    void fullControlZeroSensorPin(bool value) { _isFullControlZeroSensorPin = value; };

    void pool(); // Обновляет датчик нуля, режим и разгон (в основном цикле)

    void setZeroSensorPin(uint8_t pin);

protected:
    void calculateAccelerationPeriod();
    void calculateCurrentSpeed();
    void checkTargetStep();
    bool canExternalChangeDir();
    void privateCalulateWorkPeriod();
    void checkZeroSensorPinStatus();
    virtual void calculatePulsePeriod() {} // PUL/DIR: пересчёт длительности STEP
    virtual void reverseDir() {};

    uint16_t _currentSpeed = 0;
    uint32_t _accelerationPeriod = 0;
    uint16_t _startAccelerationSpeed = 100;
    uint32_t _startAccelerationPeriodTime = 0;
    bool _accelerationMode = false;
    uint16_t _maxSpeed = 10;
    int16_t _acceleration = 1000;
    uint16_t _tickPeriod = 10;
    uint8_t _mode = FLPROG_STOP_STEP_MOTOR_MODE;
    int32_t _currentStep = 0;
    int32_t _targetStep = 0;
    uint32_t _goStepCounter = 0;
    bool _workStatus = false;
    bool _dir = false;
    uint8_t _zeroSensorPin = 255;
    char _zeroSensorPinPullMode = FLPROG_PULL_NOT_MODE;
    bool _isInvertedZeroSensorPin = false;
    bool _isFullControlZeroSensorPin = false;
    bool _zeroSensorPinStatus = false;
    bool _oldZeroSensorPinStatus = false;
    uint32_t _workPeriod = 0; // Количество полных тиков ожидания до шага
    uint32_t _periodCounter = 0; // Используется из обработчика tick()
};