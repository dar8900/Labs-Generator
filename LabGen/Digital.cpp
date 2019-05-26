#include "LabGen.h"
#include "TimeEvents.h"
#include "Display.h"
#include "Keyboard.h"
#include "Setup.h"
#include "EepromSaves.h"
#include "Digital.h"

enum
{
	LOW_STR,
	HIGH_STR
};

const char *TitoloStatoUscitaStr[] = 
{
	"Stato entrata",
	"Stato uscita",
};

const char *StatoPorta[] = 
{
	"LOW",
	"HIGH",
};


static uint8_t CheckDigitalInput()
{
	if(digitalRead(DIGITAL_INPUT_PIN))
		return HIGH;
	else
		return LOW;
}

static void SetDigitalOutput(uint8_t HighLow)
{
	digitalWrite(DIGITAL_OUTPUT_PIN, HighLow);
	return;
}


static void DrawDigitalFunction(uint8_t InOrOut, uint8_t PortState)
{
	u8g2.drawStr(CENTER_ALIGN(TitoloStatoUscitaStr[InOrOut]), TITLE_Y_POS, TitoloStatoUscitaStr[InOrOut]);
	u8g2.setFont(PORT_STATE_STR_FONT);
	u8g2.drawStr(CENTER_ALIGN(StatoPorta[PortState]), PORT_STATE_STR_Y_POS, StatoPorta[PortState]);
	if(PortState == LOW)
	{
		u8g2.drawFrame(CENTER_ALIGN(StatoPorta[PortState]) - 1, FRAME_PORT_Y_POS - 1, 
						FRAME_PORT_STR_STATE_WIDHT(StatoPorta[PortState]) + 1, STR_HIGH + 4);	
	}
	else
	{
		u8g2.setFontMode(0);
		u8g2.setDrawColor(2);
		u8g2.drawBox(CENTER_ALIGN(StatoPorta[PortState]) - 1, FRAME_PORT_Y_POS, 
					 FRAME_PORT_STR_STATE_WIDHT(StatoPorta[PortState]) + 1, STR_HIGH + 2);
	}
	if(InOrOut == DIGITAL_OUTPUT)
	{
		u8g2.setFont(TOP_BOTTOM_FONT);
		if(PortState == LOW)
			u8g2.drawStr(CENTER_ALIGN("Ok = HIGH"), BOTTOM_POS, "Ok = HIGH");
		else
			u8g2.drawStr(CENTER_ALIGN("Ok = LOW"), BOTTOM_POS, "Ok = LOW");
	}

}

void DigitalFunction(uint8_t InOrOut)
{
	bool ExitDigital = false;
	uint8_t PortState = LOW;
	uint8_t Button = NO_PRESS;
	
	while(!ExitDigital)
	{
		u8g2.clearBuffer();	
		DrawTopInfo();
		DrawDigitalFunction(InOrOut, PortState);
		u8g2.sendBuffer();
		CheckEvents();
		Button = ButtonPress();
		switch(Button)
		{
			case UP:
				break;
			case DOWN:
				break;
			case LEFT:
				break;
			case OK:
				if(InOrOut == DIGITAL_OUTPUT)
				{
					if(PortState == HIGH)
						PortState = LOW;
					else
						PortState = HIGH;
					SetDigitalOutput(PortState);
				}
				break;
			case EXIT:
				ExitDigital = true;
				break;
			default:
				break;
		}
		if(InOrOut == DIGITAL_INPUT)
			PortState = CheckDigitalInput();
	}
}
