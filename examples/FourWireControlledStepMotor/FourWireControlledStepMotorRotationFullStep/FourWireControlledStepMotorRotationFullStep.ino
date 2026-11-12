#include "flprogFourWireControlledStepMotor.h"
#include "flprogSystemHardwareTimer.h"
#include "RT_HW_CONSOLE.h"

#ifdef RT_HW_PLC_CODE
#define IN1_PIN RT_HW_Base.getPinDOT(0)
#define IN2_PIN RT_HW_Base.getPinDOT(1)
#define IN3_PIN RT_HW_Base.getPinDOT(2)
#define IN4_PIN RT_HW_Base.getPinDOT(3)
#define CONSOLE_UART 1
#endif

#ifndef IN1_PIN
#ifdef RT_HW_CORE_STM32
#define IN1_PIN PB6
#define IN2_PIN PB7
#define IN3_PIN PB8
#define IN4_PIN PB9
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
#define CONSOLE_UART 0
#endif
#endif

#ifndef IN1_PIN
#ifdef RT_HW_CORE_ESP32
#define IN1_PIN 15
#define IN2_PIN 16
#define IN3_PIN 17
#define IN4_PIN 18
#define CONSOLE_UART 0
#endif
#endif

#ifndef IN1_PIN
#define IN1_PIN 8
#define IN2_PIN 9
#define IN3_PIN 10
#define IN4_PIN 11
#define CONSOLE_UART 0
#endif

FLProgFourWireControlledStepMotor motor(IN1_PIN, IN2_PIN, IN3_PIN, IN4_PIN);

uint32_t changeModeTime;
uint8_t currentMode = FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE;

void setup()
{
    initFlprogSystemTimer(FLProgTimerHandler);
    setFlprogSystemTimerTickPeriod(10);

    RT_HW_console.dev.numUart = CONSOLE_UART;
    changeModeTime = millis();
    motor.tickPeriod(getFlprogSystemTimerTickPeriod());
    motor.acceleration(200);
    motor.startAccelerationSpeed(200);
    motor.maxSpeed(500);
    motor.mode(currentMode);
}

void loop()
{
    motor.pool();
    if (RT_HW_Base.getIsTimerMs(changeModeTime, 10000))
    {
        changeModeTime = millis();
        if (currentMode == FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE)
        {
            currentMode = FLPROG_STOP_STEP_MOTOR_MODE;
            motor.mode(currentMode);
        }
        else
        {
            currentMode = FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE;
            motor.dir(!motor.dir());
            motor.mode(currentMode);
        }
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
    if (motorStatus == FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_STATUS)
    {
        statusString = "Motor Rotation!";
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