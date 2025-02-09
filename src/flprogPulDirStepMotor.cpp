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

void FLProgPulDirStepMotor::dir(bool value)
{
    if (dir == _dir)
    {
        return;
    }
    _dir = dir;
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

void FLProgPulDirStepMotor::maxSpeed(uint16_t value)
{
    if (value == _maxSpeed)
    {
        return;
    }
    _maxSpeed = value;
    calculateCurrentSpeed();
}

void FLProgPulDirStepMotor::acceleration(uint16_t value)
{
    if (value == _acceleration)
    {
        return;
    }
    _acceleration = value;
}

void FLProgPulDirStepMotor::tickPeriod(uint16_t value)
{
    if (value == _tickPeriod)
    {
        return;
    }
    _tickPeriod = value;
    calculatePulsePeriod();
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
        return;
    }
}