#include "flprogAbstractStepMotor.h"

void FLProgAbstractStepMotor::maxSpeed(uint16_t value)
{
    if (value == _maxSpeed)
    {
        return;
    }
    _maxSpeed = value;
    calculateCurrentSpeed();
}

void FLProgAbstractStepMotor::acceleration(int16_t value)
{
    if (value < 1)
    {
        // Выключение разгона должно немедленно восстановить равномерную скорость.
        _acceleration = 0;
        _accelerationMode = false;
        calculateCurrentSpeed();
        return;
    }
    if (value == _acceleration)
    {
        return;
    }
    _acceleration = value;
    calculateAccelerationPeriod();
    _accelerationMode = false; // Отсчёт с новым ускорением начинается заново
}

void FLProgAbstractStepMotor::tickPeriod(uint16_t value)
{
    if (value == 0 || value == _tickPeriod)
    {
        // Нулевой период недопустим: частота прерываний неизвестна.
        return;
    }
    _tickPeriod = value;
    calculatePulsePeriod();
    calculateCurrentSpeed();
}

void FLProgAbstractStepMotor::calculateAccelerationPeriod()
{
    if (_acceleration < 1)
    {
        return;
    }
    _accelerationPeriod = 1000000UL / static_cast<uint32_t>(_acceleration);
}

bool FLProgAbstractStepMotor::canExternalChangeDir()
{
    if (_mode == FLPROG_POSITION_TRANSITION_STEP_MOTOR_MODE)
    {
        return false;
    }
    return true;
}

/** @brief Меняет направление только вне режима позиционирования. */
void FLProgAbstractStepMotor::dir(bool value)
{
    if (!canExternalChangeDir())
    {
        return;
    }
    if (value == _dir)
    {
        return;
    }
    _currentSpeed = 0;
    calculateCurrentSpeed();
    reverseDir();
}

void FLProgAbstractStepMotor::setZeroStep()
{
    _currentStep = 0;
    if (_mode == FLPROG_FIND_ZERO_STEP_MOTOR_MODE)
    {
        if (_workStatus)
        {
            _status = FLPROG_END_FIND_ZERO_STEP_MOTOR_STATUS;
            _workStatus = false;
            _currentSpeed = 0;
            calculateCurrentSpeed();
        }
    }
    checkTargetStep();
}

void FLProgAbstractStepMotor::targetStep(int32_t value)
{
    if (_targetStep == value)
    {
        return;
    }
    _targetStep = value;
    checkTargetStep();
    calculateCurrentSpeed();
}

void FLProgAbstractStepMotor::checkTargetStep()
{
    if (_mode != FLPROG_POSITION_TRANSITION_STEP_MOTOR_MODE)
    {
        return;
    }
    if (_targetStep == _currentStep)
    {
        // Если цель достигнута (в том числе из setCurrentStep), команда завершена.
        _workStatus = false;
        _status = FLPROG_POSITION_TRANSITION_WAIT_COMMAND_STEP_MOTOR_STATUS;
        _currentSpeed = 0;
        _accelerationMode = false;
        return;
    }
    if (!_workStatus)
    {
        _periodCounter = 0; // Начать новое движение с полного интервала.
    }
    _status = FLPROG_POSITION_TRANSITION_EXECUTE_COMMAND_STEP_MOTOR_STATUS;
    _workStatus = true;
    if (_dir)
    {
        if (_targetStep < _currentStep)
        {
            reverseDir();
            _currentSpeed = 0;
            calculateCurrentSpeed();
        }
    }
    else
    {
        if (_targetStep > _currentStep)
        {
            reverseDir();
            _currentSpeed = 0;
            calculateCurrentSpeed();
        }
    }
}

void FLProgAbstractStepMotor::mode(uint8_t value)
{
    if (value > 4)
    {
        return;
    }
    if (_mode == value)
    {
        return;
    }
    _mode = value;
    if (_mode != FLPROG_GO_STEP_COUNT_STEP_MOTOR_MODE)
    {
        _goStepCounter = 0;
    }
    if ((_mode == FLPROG_STOP_STEP_MOTOR_MODE) || (_mode == FLPROG_GO_STEP_COUNT_STEP_MOTOR_MODE))
    {
        // Статус должен обновиться и при смене режима из уже остановленного состояния.
        _status = (_mode == FLPROG_STOP_STEP_MOTOR_MODE)
                    ? FLPROG_STOP_STEP_MOTOR_STATUS
                    : FLPROG_GO_STEP_COUNT_STEP_WAIT_COMMAND_STEP_MOTOR_STATUS;
        _workStatus = false;
        _currentSpeed = 0;
        _periodCounter = 0;
        _accelerationMode = false;
        return;
    }
    if ((_mode == FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE) || (_mode == FLPROG_FIND_ZERO_STEP_MOTOR_MODE))
    {
        if (!_workStatus)
        {
            if (_mode == FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE)
            {
                _status = FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_STATUS;
            }
            if (_mode == FLPROG_FIND_ZERO_STEP_MOTOR_MODE)
            {
                _status = FLPROG_FIND_ZERO_STEP_MOTOR_STATUS;
            }
            _workStatus = true;
            _currentSpeed = 0;
            calculateCurrentSpeed();
        }
        return;
    }
    _status = FLPROG_POSITION_TRANSITION_WAIT_COMMAND_STEP_MOTOR_STATUS;
    checkTargetStep();
    calculateCurrentSpeed();
}

void FLProgAbstractStepMotor::goThroughSteps(uint32_t value)
{
    if (_mode != FLPROG_GO_STEP_COUNT_STEP_MOTOR_MODE)
    {
        return;
    }
    if (value == 0)
    {
        return;
    }
    // Счётчик хранит количество ДОПОЛНИТЕЛЬНЫХ шагов после ближайшего tick().
    // Во время движения следующий шаг уже запланирован, поэтому новый запрос
    // добавляет value, а из новой команды вычитается только первый шаг.
    if (!_workStatus)
    {
        _currentSpeed = 0;
        _accelerationMode = false;
        _periodCounter = 0; // Новая команда — полный межшаговый интервал.
        calculateCurrentSpeed();
        _goStepCounter = value - 1;
    }
    else
    {
        // Защита от переполнения при очередных командах движения.
        const uint32_t maxValue = 0xFFFFFFFFUL;
        _goStepCounter = (value > maxValue - _goStepCounter)
                             ? maxValue
                             : _goStepCounter + value;
    }
    _status = FLPROG_GO_STEP_COUNT_STEP_EXECUTE_COMMAND_STEP_MOTOR_STATUS;
    _workStatus = true;
}

/** @brief Считывает датчик нуля и отслеживает фронт при ручном управлении. */
void FLProgAbstractStepMotor::checkZeroSensorPinStatus()
{
    if (_zeroSensorPin == 255)
    {
        return;
    }
    bool temp = digitalRead(_zeroSensorPin);
    if (_isInvertedZeroSensorPin)
    {
        temp = !temp;
    }
    if (!_workStatus)
    {
        if (temp)
        {
            if (!_oldZeroSensorPinStatus)
            {
                _oldZeroSensorPinStatus = true;
                if (_isFullControlZeroSensorPin)
                {
                    _currentStep = 0;
                }
            }
        }
        else
        {
            _oldZeroSensorPinStatus = false;
        }
        return;
    }
    if (temp)
    {
        return;
    }
    _oldZeroSensorPinStatus = false;
}

/** @brief Обслуживание из loop(), в отличие от вызываемого таймером tick(). */
void FLProgAbstractStepMotor::pool()
{
    setFlags();
    checkZeroSensorPinStatus();
    checkTargetStep();
    calculateCurrentSpeed();
}

/**
 * @brief Вычисляет число тиков до следующего шага без float.
 *
 * В tick() шаг делается, когда counter == _workPeriod; поэтому храним
 * ceil(1'000'000/(speed*tickUs)) - 1. Верхняя граница speed*tickUs
 * помещается в uint32_t (оба множителя uint16_t).
 */
void FLProgAbstractStepMotor::privateCalulateWorkPeriod()
{
    if (_currentSpeed == 0 || _tickPeriod == 0)
    {
        _workPeriod = 0;
        return;
    }
    const uint32_t divisor = static_cast<uint32_t>(_currentSpeed) * _tickPeriod;
    const uint32_t ticks = 1000000UL / divisor + (1000000UL % divisor != 0);
    _workPeriod = (ticks > 0 ? ticks : 1) - 1;
}

/** @brief Обновляет скорость: без разгона мгновенно, с разгоном по micros(). */
void FLProgAbstractStepMotor::calculateCurrentSpeed()
{
    // Даже при неизменной скорости период нужно пересчитать:
    // это важно после tickPeriod(), maxSpeed() и изменения настроек.
    if (_acceleration < 1 || _currentSpeed == _maxSpeed)
    {
        if (_acceleration < 1)
        {
            _currentSpeed = _maxSpeed;
        }
        _accelerationMode = false;
        privateCalulateWorkPeriod();
        return;
    }
    if (!_accelerationMode)
    {
        _accelerationMode = true;
        _startAccelerationPeriodTime = micros();
    }
    if (_currentSpeed == 0)
    {
        _currentSpeed = (_maxSpeed < _startAccelerationSpeed) ? _maxSpeed : _startAccelerationSpeed;
        privateCalulateWorkPeriod();
        return;
    }
    if (!RT_HW_Base.getIsTimerUs(_startAccelerationPeriodTime, _accelerationPeriod))
    {
        return;
    }
    _startAccelerationPeriodTime = micros();
    if (_currentSpeed < _maxSpeed)
    {
        _currentSpeed++;
    }
    else if (_currentSpeed > _maxSpeed)
    {
        _currentSpeed--;
    }
    privateCalulateWorkPeriod();
}

void FLProgAbstractStepMotor::setZeroSensorPin(uint8_t pin)
{
    if (_zeroSensorPin == pin)
    {
        return;
    }
    _zeroSensorPin = pin;
    _oldZeroSensorPinStatus = false; // Новый вход — новая история фронтов.
    if (_zeroSensorPin == 255)
    {
        return;
    }
    if (_zeroSensorPinPullMode == FLPROG_PULL_UP_MODE)
    {
        pinMode(_zeroSensorPin, INPUT_PULLUP);
        return;
    }
    else
    {
        pinMode(_zeroSensorPin, INPUT);
    }
    checkZeroSensorPinStatus();
}

uint16_t FLProgAbstractStepMotor::currenrSpeed()
{
    if (_workStatus)
    {
        return _currentSpeed;
    }
    return 0;
}

void FLProgAbstractStepMotor::setCurrentStep(int32_t step)
{
    _currentStep = step;
    checkTargetStep();
}