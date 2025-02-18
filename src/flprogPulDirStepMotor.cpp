#include "flprogPulDirStepMotor.h"

FLProgPulDirStepMotor::FLProgPulDirStepMotor(uint8_t pulPin, uint8_t dirPin)
{
    _pulPin = pulPin;
    _dirPin = dirPin;
    if (_pulPin < 255)
    {
        pinMode(_pulPin, OUTPUT);
    }
    if (_dirPin < 255)
    {
        pinMode(_dirPin, OUTPUT);
    }
    calculatePulsePeriod();
    calculateAccelerationPeriod();
    calculateCurrentSpeed();
}

void FLProgPulDirStepMotor::pool()
{
    calculateCurrentSpeed();
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
    if (_mode == FLPROG_POSITION_TRANSITION_STEP_MOTOR_MODE)
    {
        checkTargetStep();
    }
}

void FLProgPulDirStepMotor::reverseDir()
{
    _dir = !_dir;
    if (!(_dirPin < 255))
    {
        return;
    }
    if (_invertDirPin)
    {
        digitalWrite(_dirPin, !_dir);
        return;
    }
    digitalWrite(_dirPin, _dir);
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
    _pulsePeriod = (uint16_t)(_pulseTime / _tickPeriod);
}

void FLProgPulDirStepMotor::calculateCurrentSpeed()
{
    if (_maxSpeed == 0)
    {
        _workStatus = false;
        return;
    }
    if (_currenrSpeed == _maxSpeed)
    {
        _accelerationMode = false;
        return;
    }
    if (!_accelerationMode)
    {
        _accelerationMode = true;
        _startAccelerationPeriodTime = micros();
        return;
    }
    if (!RT_HW_Base.getIsTimerUs(_startAccelerationPeriodTime, _accelerationPeriod))
    {
        return;
    }
    _startAccelerationPeriodTime = micros();
    if (_currenrSpeed == 0)
    {
        _currenrSpeed = 1;
    }
    else
    {
        if (_currenrSpeed < _maxSpeed)
        {
            _currenrSpeed++;
        }
        else
        {
            _currenrSpeed--;
        }
    }
    _workPeriod = (uint16_t)((1000000.0 / (_currenrSpeed)) / _tickPeriod);
}
