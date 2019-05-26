#include "LabGen.h"
#include "TimeEvents.h"
#include "Display.h"
#include "Keyboard.h"
#include "PowerCurrent.h"
#include "EepromSaves.h"

#define N_SAMPLE_OFFSET		     50	       
#define N_SAMPLE_CURRENT    	126 // 12.6ms
#define VOLT_TO_CURRENT			  0.100 
#define CALC_CURR_PWR_DELAY	    250 

typedef enum
{
	SET_VALUE_ITEM,
	GO,
	STOP,
	MAX_P_I_ITEMS	
}P_I_ITEMS;


enum
{
	GO_STR = 0,
	STOP_STR,
};

const char *ComandStr[]
{
	"GO!",
	"STOP",
};

uint16_t PwmToMosfet = 0;
uint16_t RawCurrentOffset;
float VoltageOffset;
float PowerSetted = 0, CurrentSetted = 0;
float PowerCalculated = 0, CurrentCalculated = 0;
Chrono CalcCurrentPowerTimer;

static void CalcCurrentOffset()
{
	uint32_t RawSensorSum = 0;
	for(int i = 0; i < N_SAMPLE_OFFSET; i++)
	{
		RawSensorSum += analogRead(CURRENT_SENSOR);		
	}
	RawSensorSum /= N_SAMPLE_OFFSET;
	RawCurrentOffset = (uint16_t)RawSensorSum;
	VoltageOffset = (float)RawSensorSum * RAW_VAL_TO_VOLT; // In Volt
}

static void CalcCurrent()
{
	uint32_t RawSensorSum = 0;
	for(int i = 0; i < N_SAMPLE_CURRENT; i++)
	{
		RawSensorSum += analogRead(CURRENT_SENSOR);		
	}
	RawSensorSum /= N_SAMPLE_CURRENT;
	CurrentCalculated = (float)RawSensorSum * RAW_VAL_TO_VOLT; // In Volt
	CurrentCalculated -= VoltageOffset; // Tolgo l'offset del sensore
	CurrentCalculated /= VOLT_TO_CURRENT; // In Ampere
	if(CurrentCalculated < 0)
		CurrentCalculated = -CurrentCalculated;
	PowerCalculated = 12.0 * CurrentCalculated;
	// CurrentCalculated = ROUND_FIRST_DECIMAL(CurrentCalculated);
	// PowerCalculated = ROUND_FIRST_DECIMAL(PowerCalculated);
}

static void PilotMosfet(bool StartTest, float ValueSet, float ValueCalc)
{
	if(StartTest)
	{
		if(ValueSet > ValueCalc)
		{
			if(PwmToMosfet > 0)
				PwmToMosfet--;
		}
		else
		{
			if(PwmToMosfet < MAX_PWM_VALUE)
				PwmToMosfet++;				
		}
		Timer1.setPwmDuty(MOSFET_REGULATOR_PIN, PwmToMosfet);
	}
	else
	{
		Timer1.setPwmDuty(MOSFET_REGULATOR_PIN, 0);
	}
}

void DrawPowerCurrentPage(uint8_t PowerOrCurrent, float ValueToReach, float ValueMeasured, uint8_t ItemSel)
{
	String ValueSet ,ValueCalc;

	u8g2.setFont(VALUE_FONT);
	
	// Scrivo misura da raggiungere
	ValueSet = String(ValueToReach, 1);
	u8g2.drawStr(LEFT_ALIGN, VALUES_STR_Y_POS_WA, ValueSet.c_str());
	
	// Scrivo misura calcolata

	ValueCalc = String(ValueMeasured, 1);
	u8g2.drawStr(RIGHT_ALIGN(ValueCalc.c_str()), VALUES_STR_Y_POS_WA, ValueCalc.c_str());
	
	u8g2.setFont(VALUE_UDM_FONT);
	// Scrivo udm
	if(PowerOrCurrent == POWER)
		u8g2.drawStr(/*CENTER_ALIGN("W")*/ 58, VALUE_UDM_Y_POS, "W");
	else
		u8g2.drawStr(/*CENTER_ALIGN("A")*/ 58, VALUE_UDM_Y_POS, "A");
	
	u8g2.setFont(COMANDS_STR_FONT);
	u8g2.drawStr(LEFT_ALIGN, COMANDS_STR_Y_POS, ComandStr[GO_STR]);
	u8g2.drawStr(RIGHT_ALIGN(ComandStr[STOP_STR]), COMANDS_STR_Y_POS, ComandStr[STOP_STR]);
	switch(ItemSel)
	{
		case SET_VALUE_ITEM:
			// DrawArrow(VALUE_ARROW_X_START , VALUE_ARROW_Y_START, ARROW_UP);
			// DrawArrow(VALUE_ARROW_X_START, VALUE_ARROW_Y_START + VALUES_STR_Y_POS_WA + 1, ARROW_DOWN);
			u8g2.drawFrame(LEFT_ALIGN, 17 - 1, 52 + 1, 13 + 1);
			break;
		case GO:
			u8g2.drawFrame(LEFT_ALIGN, COMANDS_FRAME_Y_POS - 1, STR_WIDTH(ComandStr[GO_STR]) + 1, STR_HIGH + 1);
			break;
		case STOP:
			u8g2.drawFrame(RIGHT_ALIGN(ComandStr[STOP_STR]) - 1, COMANDS_FRAME_Y_POS - 1, STR_WIDTH(ComandStr[STOP_STR]) + 1, STR_HIGH + 1);
			break;
		default:
			break;
	}
}

void PI_Regulation(uint8_t PowerOrCurrent)
{
	uint8_t Button = NO_PRESS, ItemSel = 0;
	bool ExitPwrCurr = false, StartTest = false;
	float *FlValueSettetPtr, *FlValueCalculatedPtr, MaxValue = 0;
	if(PowerOrCurrent == POWER)
	{
		FlValueSettetPtr = &PowerSetted;
		FlValueCalculatedPtr = &PowerCalculated;
		MaxValue = 65.0;
	}
	else
	{
		FlValueSettetPtr = &CurrentSetted;
		FlValueCalculatedPtr = &CurrentCalculated;
        MaxValue = 6.0;		
	}
	Timer1.setPeriod(HZ_TO_MICROSEC(EepParameters[P_I_FREQ_EEP].ParamValue));
	CalcCurrentOffset();
	while(!ExitPwrCurr)
	{
		u8g2.clearBuffer();
		DrawTopInfo();
		DrawPowerCurrentPage(PowerOrCurrent, *FlValueSettetPtr, *FlValueCalculatedPtr, ItemSel);
		u8g2.sendBuffer();
		CheckEvents();
		Button = ButtonPress();
		switch(Button)
		{
			case UP:
				if(ItemSel > 0)
					ItemSel--;
				else
					ItemSel = MAX_P_I_ITEMS - 1;
				break;
			case DOWN:
			case LEFT:
				if(ItemSel < MAX_P_I_ITEMS - 1)
					ItemSel++;
				else
					ItemSel = 0;
				break;
			case OK:
				if(ItemSel == GO)
				{
					StartTest = true;
				}
				else if(ItemSel == STOP && StartTest)
				{
					StartTest = false;
				}
				break;
			case EXIT:
				ExitPwrCurr = true;
				StartTest = false;
				break;
			default:
				break;
		}
		if(ItemSel == SET_VALUE_ITEM)
		{
			*FlValueSettetPtr = CheckRotarySelector(MaxValue);
		}
		if(CalcCurrentPowerTimer.hasPassed(CALC_CURR_PWR_DELAY, true))
		{
			CalcCurrent();
		}
		PilotMosfet(StartTest, *FlValueSettetPtr, *FlValueCalculatedPtr);
	}
	
}