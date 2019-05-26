#ifndef ANALOG_H
#define ANALOG_H
#include <Arduino.h>
#include "Display.h"

#define GRAPHIC_X_START			 0
#define GRAPHIC_Y_START			 9
#define GRAPHIC_WIDHT	         DISPLAY_WIDTH - 1
#define GRAPHIC_HIGH     		41


#define ANALOG_BOTTOM_FONT 	    u8g2_font_8x13B_mr
#define ANALOG_VALUES_Y_POS 	(DISPLAY_HIGH)


void AnalogFunction(uint8_t V12_V5);

#endif