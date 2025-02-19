#include "flprogFourWireControlledStepMotor.h"
#include "RT_HW_CONSOLE.h"

FLProgFourWireControlledStepMotor motor(8, 9, 10, 11, 2, FLPROG_PULL_UP_MODE);
FlprogBounceDiscreteInputPin externalZeroPin(3, FLPROG_PULL_UP_MODE);
FlprogBounceDiscreteInputPin restartFind(4, FLPROG_PULL_UP_MODE);
FlprogBounceDiscreteInputPin dirPin(5, FLPROG_PULL_UP_MODE);

void setup()
{
    RT_HW_console.dev.numUart = 0;
    externalZeroPin.setPeriod(53);
    restartFind.setPeriod(53);
    dirPin.setPeriod(53);

    externalZeroPin.digitalRead();
    restartFind.digitalRead();
    dirPin.digitalRead();

    motor.tickPeriod(64);
    motor.acceleration(200);
    motor.startAccelerationSpeed(200);
    motor.maxSpeed(500);

    motor.mode(FLPROG_FIND_ZERO_STEP_MOTOR_MODE);
    motor.invertZeroSensorPin(false);

    // Настройка таймера для меги
    cli();
    TCCR4A = 0;
    TCCR4B = 0;
    TCNT4 = 0;
    OCR4A = 0;
    TCCR4B |= (1 << WGM12);
    TCCR4B |= (1 << CS12) | (1 << CS10);
    TIMSK4 |= (1 << OCIE4A);
    sei();
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

ISR(TIMER4_COMPA_vect)
{
    motor.tick();
}