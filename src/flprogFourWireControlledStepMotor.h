#pragma once
#include "flprogAbstractStepMotor.h"

class FLProgFourWireControlledStepMotor : public FLProgAbstractStepMotor
{
public:
    FLProgFourWireControlledStepMotor(uint8_t in1Pin, uint8_t in2Pin, uint8_t in3Pin, uint8_t in4Pin, uint8_t zeroSensorPin = 255, char zeroSensorPinPullMode = FLPROG_PULL_NOT_MODE);

    void isInvertIn1Pin(bool value) { _isInvertIn1Pin = value; };
    void isInvertIn2Pin(bool value) { _isInvertIn2Pin = value; };
    void isInvertIn3Pin(bool value) { _isInvertIn3Pin = value; };
    void isInvertIn4Pin(bool value) { _isInvertIn4Pin = value; };
    void isInvertAllInPin(bool value);

    void isHalfStepMode(bool value);

    void tick(); // функция которую необходимо вызвыть во внешнем прерывании таймера

    void setIn1Pin(uint8_t pin);
    void setIn2Pin(uint8_t pin);
    void setIn3Pin(uint8_t pin);
    void setIn4Pin(uint8_t pin);

protected:
    virtual void reverseDir();
    void setStartStep();

    void setIn1PinValue(bool value);
    void setIn2PinValue(bool value);
    void setIn3PinValue(bool value);
    void setIn4PinValue(bool value);

    uint8_t _in1Pin = 255;
    uint8_t _in2Pin = 255;
    uint8_t _in3Pin = 255;
    uint8_t _in4Pin = 255;
    bool _isInvertIn1Pin = false;
    bool _isInvertIn2Pin = false;
    bool _isInvertIn3Pin = false;
    bool _isInvertIn4Pin = false;

    bool _isHalfStepMode = false;

    uint8_t _currentPfaze = 0;
};