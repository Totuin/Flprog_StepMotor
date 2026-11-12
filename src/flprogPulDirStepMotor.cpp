#include "flprogPulDirStepMotor.h"

FLProgPulDirStepMotor::FLProgPulDirStepMotor(uint8_t pulPin, uint8_t dirPin, uint8_t zeroSensorPin, char zeroSensorPinPullMode)
{
    _zeroSensorPinPullMode = zeroSensorPinPullMode;
    setPulPin(pulPin);
    setDirPin(dirPin);
    setZeroSensorPin(zeroSensorPin);
    calculatePulsePeriod();
    calculateAccelerationPeriod();
    calculateCurrentSpeed();
    _status = FLPROG_STOP_STEP_MOTOR_STATUS;
}

/**
 * @brief Таймерный такт STEP/DIR.
 *
 * Независимо считаются интервал между шагами и длительность активного
 * импульса. После спада обязательно остаётся хотя бы один такт неактивного
 * уровня, поэтому при избыточном maxSpeed реальная скорость ограничивается.
 */
void FLProgPulDirStepMotor::tick()
{
    bool justFinishedPulse = false;
    if (_pullStatus)
    {
        if (_pulseCounter < _pulsePeriod)
        {
            _pulseCounter++;
        }
        else
        {
            _pulseCounter = 0;
            _pullStatus = false;
            justFinishedPulse = true;
            if (_pulPin != 255)
            {
                digitalWrite(_pulPin, _invertPulPin ? HIGH : LOW);
            }
        }
    }
    if (!_workStatus || _currentSpeed == 0)
    {
        return;
    }
    // Отсчёт периода продолжается, пока STEP находится в активном уровне.
    if (_periodCounter < _workPeriod)
    {
        _periodCounter++;
        return;
    }
    // Не допускаем слияния импульсов или LOW->HIGH в одном тике.
    if (_pullStatus || justFinishedPulse)
    {
        return;
    }
    _pulseCounter = 0;
    _periodCounter = 0;
    _pullStatus = true;
    // signed overflow шага в continuous режиме недопустим.
    if (_dir)
    {
        if (_currentStep < 2147483647L) _currentStep++;
    }
    else
    {
        if (_currentStep > (-2147483647L - 1L)) _currentStep--;
    }
    if (_mode == FLPROG_POSITION_TRANSITION_STEP_MOTOR_MODE && _currentStep == _targetStep)
    {
        _workStatus = false;
        _status = FLPROG_POSITION_TRANSITION_WAIT_COMMAND_STEP_MOTOR_STATUS;
    }
    if (_mode == FLPROG_GO_STEP_COUNT_STEP_MOTOR_MODE)
    {
        if (_goStepCounter == 0)
        {
            _workStatus = false;
            _status = FLPROG_GO_STEP_COUNT_STEP_WAIT_COMMAND_STEP_MOTOR_STATUS;
        }
        else
        {
            _goStepCounter--;
        }
    }
    if ((_mode == FLPROG_FIND_ZERO_STEP_MOTOR_MODE) || _isFullControlZeroSensorPin)
    {
        if (_zeroSensorPin != 255)
        {
            _zeroSensorPinStatus = digitalRead(_zeroSensorPin);
            if (_isInvertedZeroSensorPin)
            {
                _zeroSensorPinStatus = !_zeroSensorPinStatus;
            }
            if (_zeroSensorPinStatus)
            {
                if (!_oldZeroSensorPinStatus)
                {
                    _oldZeroSensorPinStatus = true;
                    _currentStep = 0;
                    if (_mode == FLPROG_FIND_ZERO_STEP_MOTOR_MODE)
                    {
                        _workStatus = false;
                        _status = FLPROG_END_FIND_ZERO_STEP_MOTOR_STATUS;
                    }
                }
            }
            else
            {
                _oldZeroSensorPinStatus = false;
            }
        }
    }
    if (_pulPin != 255)
    {
        digitalWrite(_pulPin, _invertPulPin ? LOW : HIGH);
    }
}

void FLProgPulDirStepMotor::reverseDir()
{
    _dir = !_dir;
    if (_dirPin == 255)
    {
        return;
    }
    digitalWrite(_dirPin, _invertDirPin ? !_dir : _dir);
    delayMicroseconds(10); // Время установления направления независимо от полярности
}

void FLProgPulDirStepMotor::pulseTime(uint16_t value)
{
    if (value == _pulseTime)
    {
        return;
    }
    uint16_t temp = value;
    if (temp < 10)
    {
        temp = 10;
    }
    _pulseTime = temp;
    calculatePulsePeriod();
}

/** @brief Переводит длительность STEP в тики внешнего таймера. */
void FLProgPulDirStepMotor::calculatePulsePeriod()
{
    if (_tickPeriod == 0) // Защита на случай нестандартного наследника.
    {
        _pulsePeriod = 0;
        return;
    }
    // ceil(pulseTime / tickPeriod) - 1: спад после нужного числа тиков.
    const uint32_t ticks = _pulseTime / _tickPeriod + (_pulseTime % _tickPeriod != 0);
    _pulsePeriod = (ticks > 0 ? ticks : 1) - 1;
}

void FLProgPulDirStepMotor::setPulPin(uint8_t pin)
{
    if (_pulPin == pin)
    {
        return;
    }
    _pulPin = pin;
    if (_pulPin == 255)
    {
        return;
    }
    pinMode(_pulPin, OUTPUT);
    digitalWrite(_pulPin, _invertPulPin ? HIGH : LOW); // STEP неактивен
}

void FLProgPulDirStepMotor::setDirPin(uint8_t pin)
{
    if (_dirPin == pin)
    {
        return;
    }
    _dirPin = pin;
    if (_dirPin == 255)
    {
        return;
    }
    pinMode(_dirPin, OUTPUT);
    digitalWrite(_dirPin, _invertDirPin ? !_dir : _dir);
}