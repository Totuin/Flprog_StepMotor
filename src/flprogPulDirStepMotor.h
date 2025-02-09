#pragma once
#include "Arduino.h"
#include "flprogUtilites.h"

class FLProgPulDirStepMotor
{
public:
    FLProgPulDirStepMotor(uint8_t pulPin, uint8_t dirPin = 255);

    void maxSpeed(uint16_t value);     // Максимальная скорость вращения - обороты в секунду
    void acceleration(uint16_t value); // ускорение - на сколько оборотов в секунду увеличится скорость вращения во времени
    void pulseTime(uint16_t value);    // Длительность импульса в микросекундах
    void tickPeriod(uint16_t value);   // Период вызова прерывания во внешнем таймере  в микросекундах

    void dir(bool value); // Направление вращения

    uint16_t maxSpeed() { return _maxSpeed; };
    uint16_t acceleration() { return _acceleration; };
    uint16_t tickPeriod() { return _tickPeriod; };

    void invertDirPin(bool value) { _invertDirPin = value; };
    void invertPulPin(bool value) { _invertPulPin = value; };

    void tick(); // функция которую необходимо вызвыть во внешнем прерывании таймера

    void pool();

protected:
    void calculatePulsePeriod();
    void calculateCurrentSpeed();

    uint16_t _maxSpeed = 100;
    uint16_t _currenrSpeed = 0;

    uint16_t _tickPeriod = 10;

    uint16_t _acceleration = 100;

    uint8_t _pulPin = 255;
    uint8_t _dirPin = 255;
    bool _invertDirPin = false;
    bool _invertPulPin = false;

    bool _dir = false;

    uint16_t _pulseTime = 20;
    uint16_t _pulsePeriod;
    uint16_t _workPeriod;

    uint16_t _pulseCounter = 0;
    uint16_t _periodCounter = 0;
    bool _workStatus = false;
    bool _pullStatus = false;
};
