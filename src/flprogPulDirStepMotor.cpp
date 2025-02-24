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

void FLProgPulDirStepMotor::tick()
{
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
            if (_pulPin < 255)
            {
                if (_invertPulPin)
                {
                    digitalWrite(_pulPin, true);
                }
                else
                {
                    digitalWrite(_pulPin, false);
                }
            }
        }
    }
    if (!_workStatus)
    {
        return;
    }
    if (_periodCounter < _workPeriod)
    {
        _periodCounter++;
        return;
    }
    _pulseCounter = 0;
    _periodCounter = 0;
    _pullStatus = true;
    if (_dir)
    {
        _currentStep++;
    }
    else
    {
        _currentStep--;
    }
    if (_mode == FLPROG_POSITION_TRANSITION_STEP_MOTOR_MODE)
    {
        if (_currentStep == _targetStep)
        {
            _workStatus = false;
            _status = FLPROG_POSITION_TRANSITION_WAIT_COMMAND_STEP_MOTOR_STATUS;
        }
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
    if (_mode == FLPROG_FIND_ZERO_STEP_MOTOR_MODE)
    {
        if (_zeroSensorPin < 255)
        {
            if (_isInvertedZeroSensorPin)
            {
                if (digitalRead(_zeroSensorPin))
                {
                    _workStatus = false;
                    _status = FLPROG_END_FIND_ZERO_STEP_MOTOR_STATUS;
                }
            }
            else
            {
                if (!digitalRead(_zeroSensorPin))
                {
                    _workStatus = false;
                    _status = FLPROG_END_FIND_ZERO_STEP_MOTOR_STATUS;
                }
            }
        }
    }
    if (_pulPin < 255)
    {
        if (_invertPulPin)
        {
            digitalWrite(_pulPin, false);
        }
        else
        {
            digitalWrite(_pulPin, true);
        }
    }
}

void FLProgPulDirStepMotor::reverseDir()
{
    _dir = !_dir;
    if (_dirPin == 255)
    {
        return;
    }
    if (_invertDirPin)
    {
        digitalWrite(_dirPin, !_dir);
        return;
    }
    digitalWrite(_dirPin, _dir);
    delayMicroseconds(10);
}

void FLProgPulDirStepMotor::pulseTime(uint16_t value)
{
    if (value == _pulseTime)
    {
        return;
    }
    _pulseTime = value;
    calculatePulsePeriod();
}

void FLProgPulDirStepMotor::calculatePulsePeriod()
{
    _pulsePeriod = (uint32_t)(_pulseTime / _tickPeriod);
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
}