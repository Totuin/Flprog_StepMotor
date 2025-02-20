#include "flprogPulDirStepMotor.h"
#include "RT_HW_CONSOLE.h"
#include "STM32TimerInterrupt.h"

STM32Timer FLProg_ITimer_1(TIM1);
FLProgPulDirStepMotor motor((RT_HW_Base.getPinDOT(0)), (RT_HW_Base.getPinDOT(1)), (RT_HW_Base.getPinDIN(1)));
FlprogBounceDiscreteInputPin externalZeroPin((RT_HW_Base.getPinDIN(0)), FLPROG_PULL_NOT_MODE);
FlprogBounceDiscreteInputPin restartFind((RT_HW_Base.getPinDIN(2)), FLPROG_PULL_NOT_MODE);
FlprogBounceDiscreteInputPin dirPin((RT_HW_Base.getPinDIN(3)), FLPROG_PULL_NOT_MODE);

void setup()
{
  RT_HW_console.dev.numUart = 1;
  externalZeroPin.setPeriod(53);
  restartFind.setPeriod(53);
  dirPin.setPeriod(53);

  externalZeroPin.digitalRead();
  restartFind.digitalRead();
  dirPin.digitalRead();

  FLProg_ITimer_1.setInterval(10, FLProg_ITimer_1_handler);
  motor.pulseTime(20);
  motor.tickPeriod(10);
  motor.acceleration(100);
  motor.startAccelerationSpeed(500);
  motor.maxSpeed(1000);

  motor.mode(FLPROG_FIND_ZERO_STEP_MOTOR_MODE);
  motor.invertDirPin(true);
  motor.invertPulPin(true);
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
  if (RT_HW_console.getOk  ())
  {
    RT_HW_console.outVar (statusString);
    RT_HW_console.outCR (1);
  }
}

void FLProg_ITimer_1_handler()
{
  motor.tick();
}