#include "flprogAbstractStepMotor.h"

void FLProgAbstractStepMotor::maxSpeed(uint16_t value)
{
    if (value == 0)
    {
        mode(FLPROG_STOP_STEP_MOTOR_MODE);
        return;
    }
    if (value == _maxSpeed)
    {
        return;
    }
    _maxSpeed = value;
    calculateCurrentSpeed();
}

void FLProgAbstractStepMotor::acceleration(uint16_t value)
{
    if (value == _acceleration)
    {
        return;
    }
    _acceleration = value;
    calculateAccelerationPeriod();
}

void FLProgAbstractStepMotor::tickPeriod(uint16_t value)
{
    if (value == _tickPeriod)
    {
        return;
    }
    _tickPeriod = value;
    calculatePulsePeriod();
    calculateCurrentSpeed();
}

void FLProgAbstractStepMotor::calculateAccelerationPeriod()
{
    _accelerationPeriod = (uint16_t)(1000000.0 / _acceleration);
}

bool FLProgAbstractStepMotor::canExternalChangeDir()
{
    if (_mode == FLPROG_POSITION_TRANSITION_STEP_MOTOR_MODE)
    {
        return false;
    }
    return true;
}

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
    _currenrSpeed = 0;
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
            _workStatus = false;
            _currenrSpeed = 0;
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
        _currenrSpeed = 0;
        calculateCurrentSpeed();
        return;
    }
    _workStatus = true;
    if (_dir)
    {
        if (_targetStep < _currentStep)
        {
            reverseDir();
            _currenrSpeed = 0;
            calculateCurrentSpeed();
        }
    }
    else
    {
        if (_targetStep > _currentStep)
        {
            reverseDir();
            _currenrSpeed = 0;
            calculateCurrentSpeed();
        }
    }
}

void FLProgAbstractStepMotor::mode(uint8_t value)
{
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
        if (_workStatus)
        {
            _workStatus = false;
            _currenrSpeed = 0;
            calculateCurrentSpeed();
        }
        return;
    }
    if ((_mode == FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE) || (_mode == FLPROG_FIND_ZERO_STEP_MOTOR_MODE))
    {
        if (!_workStatus)
        {
            _workStatus = true;
            _currenrSpeed = 0;
            calculateCurrentSpeed();
        }
        return;
    }
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
    if (_goStepCounter == 0)
    {
        _currenrSpeed = 0;
        calculateCurrentSpeed();
    }
    _goStepCounter = _goStepCounter + value;
    _workStatus = true;
}
