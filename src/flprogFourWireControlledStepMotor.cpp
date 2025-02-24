#include "flprogFourWireControlledStepMotor.h"

FLProgFourWireControlledStepMotor::FLProgFourWireControlledStepMotor(uint8_t in1Pin, uint8_t in2Pin, uint8_t in3Pin, uint8_t in4Pin, uint8_t zeroSensorPin, char zeroSensorPinPullMode)
{
    _zeroSensorPinPullMode = zeroSensorPinPullMode;
    setIn1Pin(in1Pin);
    setIn2Pin(in2Pin);
    setIn3Pin(in3Pin);
    setIn4Pin(in4Pin);
    setZeroSensorPin(zeroSensorPin);
    calculateAccelerationPeriod();
    calculateCurrentSpeed();
    setStartStep();
    _status = FLPROG_STOP_STEP_MOTOR_STATUS;
}

void FLProgFourWireControlledStepMotor::isInvertAllInPin(bool value)
{
    _isInvertIn1Pin = value;
    _isInvertIn2Pin = value;
    _isInvertIn3Pin = value;
    _isInvertIn4Pin = value;
}

void FLProgFourWireControlledStepMotor::reverseDir()
{
    _dir = !_dir;
}

void FLProgFourWireControlledStepMotor::isHalfStepMode(bool value)
{
    if (value == _isHalfStepMode)
    {
        return;
    }
    _isHalfStepMode = value;
    setStartStep();
}

void FLProgFourWireControlledStepMotor::setStartStep()
{
    _currentPfaze = 0;
    setIn1PinValue(true);
    setIn2PinValue(!_isHalfStepMode);
    setIn3PinValue(false);
    setIn4PinValue(false);
}

void FLProgFourWireControlledStepMotor::setIn1PinValue(bool value)
{
    if (_in1Pin == 255)
    {
        return;
    }
    if (_isInvertIn1Pin)
    {
        digitalWrite(_in1Pin, value);
    }
    else
    {
        digitalWrite(_in1Pin, !value);
    }
}

void FLProgFourWireControlledStepMotor::setIn2PinValue(bool value)
{
    if (_in2Pin == 255)
    {
        return;
    }
    if (_isInvertIn2Pin)
    {
        digitalWrite(_in2Pin, value);
    }
    else
    {
        digitalWrite(_in2Pin, !value);
    }
}

void FLProgFourWireControlledStepMotor::setIn3PinValue(bool value)
{
    if (_in3Pin == 255)
    {
        return;
    }
    if (_isInvertIn3Pin)
    {
        digitalWrite(_in3Pin, value);
    }
    else
    {
        digitalWrite(_in3Pin, !value);
    }
}

void FLProgFourWireControlledStepMotor::setIn4PinValue(bool value)
{
    if (_in4Pin == 255)
    {
        return;
    }
    if (_isInvertIn4Pin)
    {
        digitalWrite(_in4Pin, value);
    }
    else
    {
        digitalWrite(_in4Pin, !value);
    }
}

void FLProgFourWireControlledStepMotor::tick()
{
    if (!_workStatus)
    {
        return;
    }
    if (_periodCounter < _workPeriod)
    {
        _periodCounter++;
        return;
    }
    _periodCounter = 0;
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
    if (!_isHalfStepMode)
    {
        if (_dir)
        {
            _currentPfaze++;
            if (_currentPfaze > 3)
            {
                _currentPfaze = 0;
            }
        }
        else
        {
            if (_currentPfaze == 0)
            {
                _currentPfaze = 3;
            }
            else
            {
                _currentPfaze--;
            }
        }
        if (_currentPfaze == 0)
        {
            setIn2PinValue(true);
            setIn4PinValue(false);
            return;
        }
        if (_currentPfaze == 1)
        {
            setIn1PinValue(false);
            setIn3PinValue(true);
            return;
        }
        if (_currentPfaze == 2)
        {
            setIn2PinValue(false);
            setIn4PinValue(true);
            return;
        }
        if (_currentPfaze == 3)
        {
            setIn1PinValue(true);
            setIn3PinValue(false);
            return;
        }
        return;
    }
    if (_dir)
    {
        _currentPfaze++;
        if (_currentPfaze > 7)
        {
            _currentPfaze = 0;
        }
    }
    else
    {
        if (_currentPfaze == 0)
        {
            _currentPfaze = 7;
        }
        else
        {
            _currentPfaze--;
        }
    }
    if (_currentPfaze == 0)
    {
        setIn4PinValue(false);
        return;
    }
    if (_currentPfaze == 1)
    {
        setIn2PinValue(true);
        return;
    }
    if (_currentPfaze == 2)
    {
        setIn1PinValue(false);
        return;
    }
    if (_currentPfaze == 3)
    {
        setIn3PinValue(true);
        return;
    }
    if (_currentPfaze == 4)
    {
        setIn2PinValue(false);
        return;
    }
    if (_currentPfaze == 5)
    {
        setIn4PinValue(true);
        return;
    }
    if (_currentPfaze == 6)
    {
        setIn3PinValue(false);
        return;
    }
    if (_currentPfaze == 7)
    {
        setIn1PinValue(true);
        return;
    }
}

void FLProgFourWireControlledStepMotor::setIn1Pin(uint8_t pin)
{
    if (_in1Pin == pin)
    {
        return;
    }
    _in1Pin = pin;
    if (_in1Pin == 255)
    {
        return;
    }
    pinMode(_in1Pin, OUTPUT);
}

void FLProgFourWireControlledStepMotor::setIn2Pin(uint8_t pin)
{
    if (_in2Pin == pin)
    {
        return;
    }
    _in2Pin = pin;
    if (_in2Pin == 255)
    {
        return;
    }
    pinMode(_in2Pin, OUTPUT);
}

void FLProgFourWireControlledStepMotor::setIn3Pin(uint8_t pin)
{
    if (_in3Pin == pin)
    {
        return;
    }
    _in3Pin = pin;
    if (_in3Pin == 255)
    {
        return;
    }
    pinMode(_in3Pin, OUTPUT);
}

void FLProgFourWireControlledStepMotor::setIn4Pin(uint8_t pin)
{
    if (_in4Pin == pin)
    {
        return;
    }
    _in4Pin = pin;
    if (_in4Pin == 255)
    {
        return;
    }
    pinMode(_in4Pin, OUTPUT);
}