#include "flprogFourWireControlledStepMotor.h"
#include "RT_HW_CONSOLE.h"

#include "flprogFourWireControlledStepMotor.h"
#include "flprogSystemHardwareTimer.h"
#include "RT_HW_CONSOLE.h"

#ifdef RT_HW_PLC_CODE
#define IN1_PIN RT_HW_Base.getPinDOT(0)
#define IN2_PIN RT_HW_Base.getPinDOT(1)
#define IN3_PIN RT_HW_Base.getPinDOT(2)
#define IN4_PIN RT_HW_Base.getPinDOT(3)
#define EXTERNAL_SET_ZERO_PIN RT_HW_Base.getPinDIN(0)
#define INTERNAL_SET_ZERO_PIN RT_HW_Base.getPinDIN(1)
#define RESTART_FIND_PIN RT_HW_Base.getPinDIN(2)
#define EXTERNAL_DIR_PIN_PIN RT_HW_Base.getPinDIN(2)
#define EXTERNAL_SET_ZERO_PIN_PULL_MODE FLPROG_PULL_NOT_MODE
#define INTERNAL_SET_ZERO_PIN_PULL_MODE FLPROG_PULL_NOT_MODE
#define RESTART_FIND_PIN_PIN_PULL_MODE FLPROG_PULL_NOT_MODE
#define EXTERNAL_DIR_PIN_PIN_PULL_MODE FLPROG_PULL_NOT_MODE
#define CONSOLE_UART 1
#endif

#ifndef IN1_PIN
#ifdef RT_HW_CORE_STM32
#define IN1_PIN PB6
#define IN2_PIN PB7
#define IN3_PIN PB8
#define IN4_PIN PB9
#define EXTERNAL_SET_ZERO_PIN PA4
#define INTERNAL_SET_ZERO_PIN PA5
#define RESTART_FIND_PIN PA6
#define EXTERNAL_DIR_PIN_PIN PA7
#define EXTERNAL_SET_ZERO_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define INTERNAL_SET_ZERO_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define RESTART_FIND_PIN_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define EXTERNAL_DIR_PIN_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define CONSOLE_UART 0
#endif
#endif

#ifndef IN1_PIN
#ifdef RT_HW_CORE_RP2040
#error NOT SELECT PINS
#endif
#endif

#ifndef IN1_PIN
#ifdef RT_HW_CORE_ESP8266
#define IN1_PIN 16 //D0
#define IN2_PIN 5 // D1
#define IN3_PIN 4 // D2
#define IN4_PIN 0 // D3
#define EXTERNAL_SET_ZERO_PIN 2      // D4
#define INTERNAL_SET_ZERO_PIN 14 // D5
#define RESTART_FIND_PIN  12 // D6
#define EXTERNAL_DIR_PIN_PIN  13 // D7
#define EXTERNAL_SET_ZERO_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define INTERNAL_SET_ZERO_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define RESTART_FIND_PIN_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define EXTERNAL_DIR_PIN_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define CONSOLE_UART 0
#endif
#endif

#ifndef IN1_PIN
#ifdef RT_HW_CORE_ESP32
#define IN1_PIN 15
#define IN2_PIN 16
#define IN3_PIN 17
#define IN4_PIN 18
#define EXTERNAL_SET_ZERO_PIN 17
#define INTERNAL_SET_ZERO_PIN 7
#define RESTART_FIND_PIN 15
#define EXTERNAL_DIR_PIN_PIN 16
#define EXTERNAL_SET_ZERO_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define INTERNAL_SET_ZERO_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define RESTART_FIND_PIN_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define EXTERNAL_DIR_PIN_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define CONSOLE_UART 0
#endif
#endif

#ifndef IN1_PIN
#define IN1_PIN 8
#define IN2_PIN 9
#define IN3_PIN 10
#define IN4_PIN 11
#define EXTERNAL_SET_ZERO_PIN 2
#define INTERNAL_SET_ZERO_PIN 3
#define RESTART_FIND_PIN 4
#define EXTERNAL_DIR_PIN_PIN 5
#define EXTERNAL_SET_ZERO_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define INTERNAL_SET_ZERO_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define RESTART_FIND_PIN_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define EXTERNAL_DIR_PIN_PIN_PULL_MODE FLPROG_PULL_UP_MODE
#define CONSOLE_UART 0
#endif


FLProgFourWireControlledStepMotor motor(IN1_PIN, IN2_PIN, IN3_PIN, IN4_PIN, INTERNAL_SET_ZERO_PIN, INTERNAL_SET_ZERO_PIN_PULL_MODE);
FlprogBounceDiscreteInputPin externalZeroPin(EXTERNAL_SET_ZERO_PIN, EXTERNAL_SET_ZERO_PIN_PULL_MODE);
FlprogBounceDiscreteInputPin restartFind(RESTART_FIND_PIN, RESTART_FIND_PIN_PIN_PULL_MODE);
FlprogBounceDiscreteInputPin dirPin(EXTERNAL_DIR_PIN_PIN, EXTERNAL_DIR_PIN_PIN_PULL_MODE);

void setup()
{
  initFlprogSystemTimer(FLProgTimerHandler);
    setFlprogSystemTimerTickPeriod(10);

    RT_HW_console.dev.numUart = CONSOLE_UART;
    
    externalZeroPin.setPeriod(53);
    restartFind.setPeriod(53);
    dirPin.setPeriod(53);

    externalZeroPin.digitalRead();
    restartFind.digitalRead();
    dirPin.digitalRead();

    motor.tickPeriod(getFlprogSystemTimerTickPeriod());
    motor.acceleration(200);
    motor.startAccelerationSpeed(200);
    motor.maxSpeed(500);

    motor.mode(FLPROG_FIND_ZERO_STEP_MOTOR_MODE);
    motor.invertZeroSensorPin(false);
}

void loop()
{
    motor.pool();
    if (!externalZeroPin.digitalRead())
    {
        motor.setZeroStep();
    }
    if (restartFind.digitalRead())
    {
        motor.mode(FLPROG_FIND_ZERO_STEP_MOTOR_MODE);
    }
    else
    {
        motor.mode(FLPROG_STOP_STEP_MOTOR_MODE);
    }
    motor.dir(dirPin.digitalRead());
    printStatus();
}

void printStatus()
{
    if (!motor.getIsChangeStatusWithReset())
    {
        return;
    }
    uint8_t motorStatus = motor.getStatus();
    String statusString = "Not defines status!";
    if (motorStatus == FLPROG_STOP_STEP_MOTOR_STATUS)
    {
        statusString = "Motor Stop!";
    }
    if (motorStatus == FLPROG_FIND_ZERO_STEP_MOTOR_STATUS)
    {
        statusString = "Motor Find Zero!";
    }
    if (motorStatus == FLPROG_END_FIND_ZERO_STEP_MOTOR_STATUS)
    {
        statusString = "Motor Find Zero OK!";
    }
    if (RT_HW_console.getOk())
    {
        RT_HW_console.outVar(statusString);
        RT_HW_console.outCR(1);
    }
}

void FLProgTimerHandler()
{
    motor.tick();
}