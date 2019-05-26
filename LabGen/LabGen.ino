#include "LabGen.h"
#include "Display.h"
#include "PowerCurrent.h"
#include "Setup.h"
#include "TimeEvents.h"
#include "Keyboard.h"
#include "Digital.h"
#include "Analog.h"
#include "Pwm.h"
#include "Log.h"
#include "Chronometer.h"
#include "EepromSaves.h"

SYSTEM_FLAGS Flags;


void setup()
{
	Serial.begin(9600);
	Timer1.initialize(HZ_TO_MICROSEC(1000));
	Timer3.initialize(HZ_TO_MICROSEC(1));
	pinMode(DIGITAL_OUTPUT_PIN, OUTPUT);
	pinMode(DIGITAL_INPUT_PIN, INPUT);
	digitalWrite(DIGITAL_OUTPUT_PIN, LOW);
	u8g2.begin();
	KeyboardInit();
	Timer1.pwm(MOSFET_REGULATOR_PIN, 0);
	Timer1.pwm(PWM_OUTPUT_PIN_HF, 0);
	Timer3.pwm(PWM_OUTPUT_PIN_LF, 0),
	RtcInit();
	EepromInit();
}



void loop()
{
	uint8_t Button = 0;
	if(ScreenNumber != SINGLE_PAGE)
	{
		DrawIconMenu(ScreenNumber);	
	}
	else
	{
		switch(SinglePage.WichSinglePage)
		{
			case POWER_CURRENT_SINGLE:
				PI_Regulation(SinglePage.ItemSelected);
				ScreenNumber = POWER_CURRENT;
				break;
			case DIGITAL_SINGLE:
				DigitalFunction(SinglePage.ItemSelected);
				ScreenNumber = DIGITAL_IN_OUT;
				break;
			case PULSE_W_SINGLE:
				PwmFunction(SinglePage.ItemSelected);
				ScreenNumber = PWM;
				break;
			case ANALOGIC_SINGLE:
				AnalogFunction(SinglePage.ItemSelected);
				ScreenNumber = ANALOG;				
				break;
			case LOG_SINGLE:
				switch(SinglePage.ItemSelected)
				{
					case LOG_LIST:
						DrawListLog();
						break;
					case LOG_GRAPHIC:
						DrawGaphicLog();
						break;
					case RESET_LOG:
						ResetLogFunction();
						break;
					default:
						break;					
				}
				while(Button != NO_PRESS)
					Button = ButtonPress();
				ScreenNumber = LOG;				
				break;
			case CHRONO_SINGLE:
				ChronoTimerFunction(SinglePage.ItemSelected);
				ScreenNumber = CHRONOMETER;
				break;
			case SETUP_SCREEN_SINGLE:
				EnterSetup(SinglePage.ItemSelected);
				ScreenNumber = SETUP;
				break;
			default:
				break;
		}
	}
	
}


