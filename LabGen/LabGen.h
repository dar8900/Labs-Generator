#ifndef LAB_GEN_H
#define LAB_GEN_H
#include <Arduino.h>
#include <TimerOne.h>
#include <TimerThree.h>
#include <EEPROM.h>

#define DEBUG_SERIAL	

#ifdef DEBUG_SERIAL

#define DBG(Msg)	Serial.println(Msg)

#define ACTIVE_BP(DebugMessage)	while(!Serial.available()){Serial.println(DebugMessage); delay(500);}


#endif


#define RAW_VAL_TO_VOLT			  0.0048
#define MAX_PWM_VALUE		   1024

#define HZ_TO_MICROSEC(Hz)			(uint32_t)((1.0/Hz) * 1000000)

typedef enum
{
	MOSFET_REGULATOR_PIN = 25,
	PWM_OUTPUT_PIN_HF    = 26,
	PWM_OUTPUT_PIN_LF    = 14,
	UP_BUTTON            =  5,
	DOWN_BUTTON          =  7,
	LEFT_BUTTON          =  8,
	OK_BUTTON            = 10,
	EXIT_BUTTON          = 11,
	DIGITAL_OUTPUT_PIN   = 12,
	DIGITAL_INPUT_PIN	 = 13,
	CURRENT_SENSOR       = A0,
	ANALOG_INPUT_12      = A1,
	ANALOG_INPUT_5       = A2,
	ROTARY_INPUT         = A3,
	CS_DISPLAY		     = 20,
	RESET_DISPLAY	     = 9,
	MAX_PINS
}PINS;

typedef struct
{
	bool PwmActive;
	bool LogActive;
	bool LogFull;
}SYSTEM_FLAGS;


extern SYSTEM_FLAGS Flags;

#endif