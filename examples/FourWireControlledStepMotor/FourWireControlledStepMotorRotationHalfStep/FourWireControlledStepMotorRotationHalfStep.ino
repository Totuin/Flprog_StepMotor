#include "flprogFourWireControlledStepMotor.h"
#include "RT_HW_CONSOLE.h"

FLProgFourWireControlledStepMotor motor(8, 9, 10, 11);

uint32_t changeModeTime;
uint8_t currentMode = FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE;

void setup()
{
    RT_HW_console.dev.numUart = 0;
    changeModeTime = millis();
    motor.tickPeriod(64);
    motor.isHalfStepMode(true);
    motor.acceleration(200);
    motor.startAccelerationSpeed(500);
    motor.maxSpeed(1000);
    motor.mode(currentMode);

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

ISR(TIMER4_COMPA_vect)
{
    motor.tick();
}