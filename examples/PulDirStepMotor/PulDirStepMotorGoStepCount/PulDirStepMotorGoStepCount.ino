#include "flprogPulDirStepMotor.h"
#include "flprogSystemHardwareTimer.h"
#include "RT_HW_CONSOLE.h"

#ifdef RT_HW_PLC_CODE
#define PULL_PIN RT_HW_Base.getPinDOT(0)
#define DIR_PIN RT_HW_Base.getPinDOT(1)
#define CONSOLE_UART 1
#endif

#ifndef PULL_PIN
#ifdef RT_HW_CORE_STM32
#define PULL_PIN PB10
#define DIR_PIN PB11
#define CONSOLE_UART 0
#endif
#endif

#ifndef PULL_PIN
#ifdef RT_HW_CORE_RP2040
#error NOT SELECT PINS
#endif
#endif

#ifndef PULL_PIN
#ifdef RT_HW_CORE_ESP8266
#define PULL_PIN 16 // D0
#define DIR_PIN 5   // D1
#define CONSOLE_UART 0
#endif
#endif

#ifndef PULL_PIN
#ifdef RT_HW_CORE_ESP32
#define PULL_PIN 4
#define DIR_PIN 5
#define CONSOLE_UART 0
#endif
#endif

#ifndef PULL_PIN
#define PULL_PIN 8
#define DIR_PIN 9
#define CONSOLE_UART 0
#endif

FLProgPulDirStepMotor motor(PULL_PIN, DIR_PIN);

uint32_t changeModeTime;

void setup()
{
    initFlprogSystemTimer(FLProgTimerHandler);
    setFlprogSystemTimerTickPeriod(10);

    RT_HW_console.dev.numUart = CONSOLE_UART;

    changeModeTime = millis();
    motor.pulseTime(20);
    motor.tickPeriod(getFlprogSystemTimerTickPeriod());
    motor.acceleration(100);
    motor.startAccelerationSpeed(500);
    motor.maxSpeed(1000);
    motor.mode(FLPROG_GO_STEP_COUNT_STEP_MOTOR_MODE);
    motor.invertDirPin(true);
    motor.invertPulPin(true);
}

void loop()
{
    motor.pool();
    if (RT_HW_Base.getIsTimerMs(changeModeTime, 5000))
    {
        changeModeTime = millis();
        motor.dir(!motor.dir());
        motor.goThroughSteps(50);
    }
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
    if (motorStatus == FLPROG_GO_STEP_COUNT_STEP_WAIT_COMMAND_STEP_MOTOR_STATUS)
    {
        statusString = "Motor Wite Command To Go!";
    }
    if (motorStatus == FLPROG_GO_STEP_COUNT_STEP_EXECUTE_COMMAND_STEP_MOTOR_STATUS)
    {
        statusString = "Motor Execute Command To Go!";
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