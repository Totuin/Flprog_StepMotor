#include "flprogPulDirStepMotor.h"

FLProgPulDirStepMotor::FLProgPulDirStepMotor(uint8_t pulPin, uint8_t dirPin, uint8_t zeroSensorPin)
{
    _pulPin = pulPin;
    _dirPin = dirPin;
    _zeroSensorPin = zeroSensorPin;
    if (_pulPin < 255)
    {
        pinMode(_pulPin, OUTPUT);
    }
    if (_dirPin < 255)
    {
        pinMode(_dirPin, OUTPUT);
    }
    if (_zeroSensorPin < 255)
    {
        pinMode(_zeroSensorPin, INPUT);
    }
    calculatePulsePeriod();
    calculateAccelerationPeriod();
    calculateCurrentSpeed();
    _status = FLPROG_STOP_STEP_MOTOR_STATUS;
}

void FLProgPulDirStepMotor::pool()
{
    setFlags();
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
    if (!(_dirPin < 255))
    {
        delayMicroseconds(10);
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

void FLProgPulDirStepMotor::calculateCurrentSpeed()
{
    if (_currenrSpeed == _maxSpeed)
    {
        _accelerationMode = false;
        return;
    }
    if (!_accelerationMode)
    {
        _accelerationMode = true;
        _startAccelerationPeriodTime = micros();
    }
    if (_currenrSpeed == 0)
    {
        if (!(_maxSpeed > _startAccelerationSpeed))
        {
            _currenrSpeed = _maxSpeed;
            _accelerationMode = false;
            privateCalulateWorkPeriod();
            return;
        }
        _currenrSpeed = _startAccelerationSpeed;
        privateCalulateWorkPeriod();
        return;
    }
    if (!RT_HW_Base.getIsTimerUs(_startAccelerationPeriodTime, _accelerationPeriod))
    {
        return;
    }
    _startAccelerationPeriodTime = micros();
    if (_currenrSpeed < _maxSpeed)
    {
        _currenrSpeed++;
    }
    else
    {
        _currenrSpeed--;
    }

    privateCalulateWorkPeriod();
}

void FLProgPulDirStepMotor::privateCalulateWorkPeriod()
{
    _workPeriod = (uint32_t)((1000000.0 / (_currenrSpeed)) / _tickPeriod);
}