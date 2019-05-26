#include "LabGen.h"
#include "Keyboard.h"
#include "TimeEvents.h"
#include <Bounce2.h>

#define DEBOUNCE_INTERVAL	10
#define RITRIGGER_DELAY	   150
#define SAMPLE_FOR_ROTARY	10 


Bounce *Keyboard = new Bounce[MAX_BUTTONS];

static bool ButtonUpdated[MAX_BUTTONS];
static bool ButtonState[MAX_BUTTONS];
static Chrono ButtonRetriggerTimer[MAX_BUTTONS];

void KeyboardInit()
{

	Keyboard[UP].attach( UP_BUTTON , INPUT_PULLUP  );   
	Keyboard[UP].interval(DEBOUNCE_INTERVAL);    
	
	Keyboard[DOWN].attach( DOWN_BUTTON , INPUT_PULLUP  );  
    Keyboard[DOWN].interval(DEBOUNCE_INTERVAL);   
	
	Keyboard[LEFT].attach( LEFT_BUTTON , INPUT_PULLUP  );  
	Keyboard[LEFT].interval(DEBOUNCE_INTERVAL); 
	
	Keyboard[OK].attach( OK_BUTTON , INPUT_PULLUP  );  
	Keyboard[OK].interval(DEBOUNCE_INTERVAL); 
	
	Keyboard[EXIT].attach( EXIT_BUTTON , INPUT_PULLUP  );  
	Keyboard[EXIT].interval(DEBOUNCE_INTERVAL);              	              	
}

uint8_t ButtonPress()
{
	uint8_t Button = NO_PRESS;
	for(int i = 0; i < MAX_BUTTONS; i++)
	{
		Keyboard[i].update();
		ButtonState[i] = false;
		if ( Keyboard[i].fell() ) 
		{
		  return i;
		}
	}
	return Button;
}

uint8_t ButtonPressContinue()
{
	uint8_t Button = NO_PRESS;
	for(int i = 0; i < MAX_BUTTONS; i++)
	{
		ButtonUpdated[i] = Keyboard[i].update();
		if(ButtonUpdated[i])
		{
			int ButtonRead = Keyboard[i].read();
			if(ButtonRead == HIGH)
			{
				ButtonState[i] = false;
				Button = NO_PRESS;
			}
			else
			{
				ButtonState[i] = true;
				ButtonRetriggerTimer[i].restart();
				Button = i;
				break;
			}
		}
		if(ButtonState[i])
		{
			if(ButtonRetriggerTimer[i].hasPassed(RITRIGGER_DELAY))
			{
				ButtonRetriggerTimer[i].restart();
				Button = i;
				break;
			}
		}
	}
	return Button;
}

uint32_t CheckRotarySelector(uint32_t MaxValue)
{
	uint32_t RotaryRawRead = 0;
	uint32_t ValueRead = 0.0;
	for(int i = 0; i < SAMPLE_FOR_ROTARY; i++)
	{
		RotaryRawRead += analogRead(ROTARY_INPUT);
	}
	RotaryRawRead /= SAMPLE_FOR_ROTARY;
	ValueRead = (uint32_t)((RotaryRawRead * RAW_VAL_TO_VOLT)*MaxValue)/5;
	return ValueRead;
}

float CheckRotarySelector(float MaxValue)
{
	uint32_t RotaryRawRead = 0;
	float ValueRead = 0.0;
	for(int i = 0; i < SAMPLE_FOR_ROTARY; i++)
	{
		RotaryRawRead += analogRead(ROTARY_INPUT);
	}
	RotaryRawRead /= SAMPLE_FOR_ROTARY;
	ValueRead = (((float)RotaryRawRead * RAW_VAL_TO_VOLT)*MaxValue)/5.0;
	return ValueRead;
}
