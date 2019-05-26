#include "LabGen.h"
#include "TimeEvents.h"
#include "Keyboard.h"
#include "Setup.h"
#include "EepromSaves.h"
#include "Analog.h"

#define N_SAMPLE            126
#define ADC_HALF_VALUE 		512
#define ADC_BUFFER_DELAY	 20

uint16_t AdcBuffer[N_SAMPLE], AdcBufferGraphic[N_SAMPLE];
float VoltageConversion;
Chrono BufferAdcTimer;
uint8_t AdcBufferIndex;

static void AdcManage(uint8_t V12_V5)
{
	uint8_t AdcChannel = 0;
	uint32_t AdcSumMean = 0;
	if(V12_V5 == ANALOG_12V)
		AdcChannel = ANALOG_INPUT_12;
	else
		AdcChannel = ANALOG_INPUT_5;
	for(int i = 0; i < N_SAMPLE; i++)
	{
		AdcBuffer[i] = analogRead(AdcChannel);
		AdcSumMean += AdcBuffer[i];
	}
	AdcSumMean /= N_SAMPLE;
	VoltageConversion = (float)AdcSumMean * RAW_VAL_TO_VOLT;
	if(V12_V5 == ANALOG_12V)
	{
		VoltageConversion = (12.0 * VoltageConversion) / 5.0;
	}
}

static void CatchDataGraphic(uint8_t V12_V5)
{
	uint8_t AdcChannel = 0;
	if(V12_V5 == ANALOG_12V)
		AdcChannel = ANALOG_INPUT_12;
	else
		AdcChannel = ANALOG_INPUT_5;
	if(BufferAdcTimer.hasPassed(EepParameters[ADC_TIME_EEP].ParamValue), true)
	{
		AdcBufferGraphic[AdcBufferIndex++] = analogRead(AdcChannel);
		if(AdcBufferIndex == N_SAMPLE - 1)
			AdcBufferIndex = 0;
	}	
}

static void DrawGraphic()
{
	uint8_t XPos = 0, YPos = 0, YPosOld = 0;
	int8_t YValue = 0;
	bool First = true;	
	u8g2.drawFrame(GRAPHIC_X_START, GRAPHIC_Y_START, GRAPHIC_WIDHT, GRAPHIC_HIGH);
	// u8g2.drawHLine(GRAPHIC_X_START, (GRAPHIC_HIGH / 2) + GRAPHIC_Y_START, GRAPHIC_WIDHT - 1);
	for(int i = 0; i < N_SAMPLE; i++)
	{
		XPos = i + 1;
		YValue = (int8_t)((AdcBufferGraphic[i] * GRAPHIC_HIGH) / 1023);
		YPos = ((GRAPHIC_HIGH - 1) + GRAPHIC_Y_START + 1) - YValue;
		if(YPos > GRAPHIC_HIGH + GRAPHIC_Y_START - 1)
			YPos = GRAPHIC_HIGH + GRAPHIC_Y_START - 2;
		u8g2.drawPixel(XPos, YPos);
		if(First)
		{
		  YPosOld = YPos;
		  First = false;
		}
		if(YPosOld != YPos)
		{
		  if((YPos - YPosOld) > 1)
			u8g2.drawVLine(XPos, YPosOld, YPos - YPosOld);
		  else if((YPos - YPosOld) < -1)
			u8g2.drawVLine(XPos, YPos, YPosOld - YPos);
		}
		YPosOld = YPos;
	}
}

static void DrawAnalogBottom()
{
	// char AnalogValueStr[6];
	String AnalogValueStr;
	float VoltageValue = VoltageConversion;
	// VoltageValue = ROUND_FIRST_DECIMAL(VoltageValue);
	// WriteFloatToStr(VoltageValue, AnalogValueStr);
	AnalogValueStr = String(VoltageValue, 2) + "V";
	u8g2.setFont(ANALOG_BOTTOM_FONT);
	u8g2.drawStr(CENTER_ALIGN(AnalogValueStr.c_str()), ANALOG_VALUES_Y_POS, AnalogValueStr.c_str());
}

void AnalogFunction(uint8_t V12_V5)
{
	bool ExitAnalog = false, ActivateLog = false;
	uint8_t Button = NO_PRESS;
	LOG_DEF LogData;
	memset(AdcBufferGraphic, 0x00, sizeof(AdcBufferGraphic)/sizeof(AdcBufferGraphic[0]));
	
	while(!ExitAnalog)
	{
		u8g2.clearBuffer();
		DrawTopInfo();
		DrawGraphic();
		DrawAnalogBottom();	
		u8g2.sendBuffer();
		CatchDataGraphic(V12_V5);
		CheckEvents();
		Button = ButtonPress();
		switch(Button)
		{
			case UP:
			case DOWN:
			case LEFT:
			case OK:
				ActivateLog = !ActivateLog;
				break;
			case EXIT:
				ExitAnalog = true;
				Flags.LogActive = false;
				ActivateLog = false;
				break;
			default:
				break;
		}
		Flags.LogActive = ActivateLog;
		AdcManage(V12_V5);
		LogData.Timestamp = TimeDate.secondstime();
		LogData.LogValue = VoltageConversion;
		LogAdcValue(LogData);
	}
}

