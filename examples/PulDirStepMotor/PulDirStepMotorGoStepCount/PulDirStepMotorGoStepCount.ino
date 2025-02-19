#include "flprogPulDirStepMotor.h"
#include "RT_HW_CONSOLE.h"
#include "STM32TimerInterrupt.h"

STM32Timer FLProg_ITimer_1(TIM1);
FLProgPulDirStepMotor motor((RT_HW_Base.getPinDOT(0)), (RT_HW_Base.getPinDOT(1)));

uint32_t changeModeTime;

void setup()
{
    RT_HW_console.dev.numUart = 1;
    FLProg_ITimer_1.setInterval(10, FLProg_ITimer_1_handler);
    changeModeTime = millis();
    motor.pulseTime(20);
    motor.tickPeriod(10);
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

void FLProg_ITimer_1_handler()
{
    motor.tick();
}