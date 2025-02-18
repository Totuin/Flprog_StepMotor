#include "flprogPulDirStepMotor.h"
#include "STM32TimerInterrupt.h"

STM32Timer FLProg_ITimer_1(TIM1);
FLProgPulDirStepMotor motor((RT_HW_Base.getPinDOT(0)), (RT_HW_Base.getPinDOT(1)));

uint32_t changeModeTime;
int32_t currentStep = 10;

void setup()
{
    FLProg_ITimer_1.setInterval(10, FLProg_ITimer_1_handler);
    changeModeTime = millis();
    motor.pulseTime(20);
    motor.tickPeriod(10);
    motor.acceleration(100);
    motor.startAccelerationSpeed(500);
    motor.maxSpeed(1000);
    motor.targetStep(currentStep);
    motor.mode(FLPROG_POSITION_TRANSITION_STEP_MOTOR_MODE);
    motor.invertDirPin(true) ;
    motor.invertPulPin(true);
}

void loop()
{
    motor.pool();
    if (RT_HW_Base.getIsTimerMs(changeModeTime, 5000))
    {
        changeModeTime = millis();
        if (currentStep < 0)
        {
            currentStep = 50;
        }
        else
        {
            currentStep = -50;
        }
        motor.targetStep(currentStep);
    }
}

void FLProg_ITimer_1_handler()
{
    motor.tick();
}