#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <Arduino.h>

typedef enum
{
	UP = 0,
	DOWN,
	LEFT,
	OK,
	EXIT,
	MAX_BUTTONS,
	NO_PRESS
}BUTTONS;

void KeyboardInit(void);
uint8_t ButtonPress(void);
uint8_t ButtonPressContinue(void);
uint32_t CheckRotarySelector(uint32_t MaxValue);
float CheckRotarySelector(float MaxValue);

#endif