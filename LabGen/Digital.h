#ifndef DIGITAL_H
#define DIGITAL_H
#include <Arduino.h>

#define PORT_STATE_STR_FONT 	          u8g2_font_t0_22b_mf // alto 21 largo 11
#define PORT_STATE_STR_Y_POS	         (30 + STR_HIGH)
#define FRAME_PORT_STR_STATE_WIDHT(Str)  (STR_WIDTH(Str) + 1)
#define FRAME_PORT_Y_POS                  29


void DigitalFunction(uint8_t InOrOut);

#endif