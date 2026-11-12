#pragma once
#include <stdint.h>
#define INPUT 0
#define INPUT_PULLUP 2
#define OUTPUT 1
#define HIGH 1
#define LOW 0
uint32_t micros();
void pinMode(uint8_t, uint8_t);
int digitalRead(uint8_t);
void digitalWrite(uint8_t, int);
void delayMicroseconds(unsigned int);
