#ifndef POWER_CURRENT_H
#define POWER_CURRENT_H
#include <Arduino.h>

#define VALUE_ARROW_X_START   26
#define VALUE_ARROW_Y_START   16
#define VALUE_ARROW_WIDHT 	  40
#define VALUE_ARROW_HIGH      20
#define VALUE_ARROW_DELTA_X	  24

#define VALUE_FONT 			 u8g2_font_crox3cb_mf  // Largo 13 alto 20
#define VALUES_STR_Y_POS_WA	(17 + u8g2.getAscent())

#define VALUE_UDM_FONT      u8g2_font_8x13B_mf   // Largo 8 alto 13
#define VALUE_UDM_Y_POS		(34 + u8g2.getAscent()) //38 + 

#define COMANDS_STR_FONT     u8g2_font_5x8_mf
#define COMANDS_STR_Y_POS    64
#define COMANDS_FRAME_Y_POS  (COMANDS_STR_Y_POS - u8g2.getAscent())

void PI_Regulation(uint8_t PowerOrCurrent);

#endif