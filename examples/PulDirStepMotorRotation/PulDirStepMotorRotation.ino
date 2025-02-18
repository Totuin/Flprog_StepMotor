#include "flprogPulDirStepMotor.h"
#include "STM32TimerInterrupt.h"

STM32Timer FLProg_ITimer_1(TIM1);
FLProgPulDirStepMotor motor((RT_HW_Base.getPinDOT(0)), (RT_HW_Base.getPinDOT(1)));

uint32_t changeModeTime;
bool currentDir = true;
uint8_t currentMode = FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE;

void setup()
{
  FLProg_ITimer_1.setInterval(10, FLProg_ITimer_1_handler);
  changeModeTime = millis();
  motor.pulseTime(20);
  motor.tickPeriod(10);
  motor.acceleration(400);
  motor.maxSpeed(1100);
  motor.mode(currentMode);
  motor.dir(currentDir);
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
      currentDir = !currentDir;
      currentMode = FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE;
      motor.mode(currentMode);
      motor.dir(currentDir);
    }
  }
}

void FLProg_ITimer_1_handler()
{
  motor.tick();
}

