#include "LabGen.h"
#include "TimeEvents.h"
#include "Display.h"
#include "Keyboard.h"
#include "Icons.cpp"
#include "EepromSaves.h"

U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R2,  CS_DISPLAY,  RESET_DISPLAY);

#define MAX_ICON_IN_PAGE 8

const char *MainMenuIconName[MAX_SCREENS] = 
{
	"P-I",
	"Digital",
	"Pwm",
	"Analog",
	"Log",
	"Chrono",
	"Setup",
};

const unsigned char *MenuIconsBmp[] = 
{
	PI_bits,
	digital_bits,
	pwm_bits,
	analog_bits,
	Log_bits,
	chronometer_bits,
	setup_bits,
};

const char *PowerCurrentIconName[] = 
{
	"Potenza",
	"Corrente",
};

const unsigned char *PowerCurrentIconsBmp[] = 
{
	power_bits,
	current_bits,
};

const char *DigitalIconName[] = 
{
	"Dig-IN",
	"Dig-OUT",
};

const unsigned char *DigitalIconsBmp[] = 
{
	DigIn_bits,
	DigOut_bits,
};


const char *PwmIconName[] = 
{
	"PWM HF",
	"PWM LF",
};

const unsigned char *PwmIconsBmp[] = 
{
	PWMHF_bits,
	PWMLF_bits,
};

const char *AnalogIconName[] = 
{
	"An-12V",
	"An-5V",
};

const unsigned char *AnalogIconsBmp[] = 
{
	Analog12V_bits,
	Analog5V_bits,
};

const char *LogIconName[] = 
{
	"List",
	"Graphic",
	"Reset",
};

const unsigned char *LogIconsBmp[] = 
{
	LogLIst_bits,
	LogGraphic_bits,
	ResetLog_bits
};

const char *ChronoIconName[] = 
{
	"Cronometro",
	"Timer",
};

const unsigned char *ChronoIconsBmp[] = 
{
	chrono_bits,
	timer_bits,
};


const char *SetupIconName[] = 
{
	"Tempo",
	"Data",
	"PWM frq",
	"Tempo log",
	"Adc delay",
	"Udm log",
	"S.S timer",
};

const unsigned char *SetupIconsBmp[] = 
{
	setup_clock_bits,
	setup_calendar_bits,
	pwmfrqsetup_bits,
	LogTimesetup_bits,
	AdcTime_bits,
	Logudm_bits,
	scrsavertimer_bits,
};


const ICON_SCREEN MainScreens[MAX_SCREENS] = 
{
	{"Main Menu"    , MAIN_MENU     , false, (MAX_SCREENS - 1)      , MainMenuIconName    , MenuIconsBmp        },
	{"Power-current", POWER_CURRENT , false, MAX_P_I_ELEMENTS       , PowerCurrentIconName, PowerCurrentIconsBmp},
	{"Digital"      , DIGITAL_IN_OUT, true , MAX_DIGITAL_ELEMENTS   , DigitalIconName	  , DigitalIconsBmp     },
	{"PWM"	        , PWM           , false, MAX_PWM_ELEMENTS       , PwmIconName	      , PwmIconsBmp         },
	{"Analog"       , ANALOG        , true , MAX_ANALOG_ELEMENTS    , AnalogIconName      , AnalogIconsBmp      },
	{"Log"          , LOG           , false, MAX_LOG_ELEMENTS       , LogIconName         , LogIconsBmp         },
	{"Chrono"       , CHRONOMETER   , false, MAX_CHRONO_ELEMENTS    , ChronoIconName      , ChronoIconsBmp      },
	{"Setup"        , SETUP         , false, MAX_SETUP_ELEMENTS     , SetupIconName       , SetupIconsBmp       },
};

uint8_t ScreenNumber = MAIN_MENU;
SINGLE_PAGE_DEF SinglePage;
static Chrono SmlIconTimer;
static bool ToggleIcon;

void DrawArrow(uint8_t XPos, uint8_t YPos, uint8_t Direction)
{
	u8g2.setFont(u8g2_font_open_iconic_arrow_1x_t); // Dimensione 8x8px
	switch(Direction)
	{
		case ARROW_UP:
			u8g2.drawGlyph(XPos, YPos, 67);
			break;
		case ARROW_DOWN:
			u8g2.drawGlyph(XPos, YPos, 64);		
			break;
		case ARROW_LEFT:
			u8g2.drawGlyph(XPos, YPos, 65);		
			break;
		case ARROW_RIGHT:
			u8g2.drawGlyph(XPos, YPos, 66);		
			break;
		default:
			break;
	}
}

void ShowMessage(String Message)
{
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_t0_12b_me); // Max 21 caratteri w 6 h 11
    u8g2.drawStr(CENTER_ALIGN(Message.c_str()), CENTER_POS, Message.c_str());
	u8g2.sendBuffer();
	delay(1000);
}

void DrawClock()
{
    char Date[9];//, Time[9];
    float Meno90 = (float)TO_RADIANTS(90.0);
    snprintf(Date, 9, "%02d/%02d/%02d", ActualTimeDate.Day, ActualTimeDate.Month, ActualTimeDate.Year%100);
    // snprintf(Time, 9, "%02d:%02d:%02d", ActualTimeDate.Hour, ActualTimeDate.Minute, ActualTimeDate.Second);
    int16_t XPosHour =   (int16_t) (CLOCK_RADIUS - 10) * cos((double)TO_RADIANTS(30*(ActualTimeDate.Hour%12))- Meno90);
    int16_t YPosHour =   (int16_t) (CLOCK_RADIUS - 10) * sin((double)TO_RADIANTS(30*(ActualTimeDate.Hour%12))- Meno90); 
    int16_t XPosMinute = (int16_t) (CLOCK_RADIUS - 4) * cos((double)TO_RADIANTS(6*ActualTimeDate.Minute ) - Meno90);
    int16_t YPosMinute = (int16_t) (CLOCK_RADIUS - 4) * sin((double)TO_RADIANTS(6*ActualTimeDate.Minute ) - Meno90);
    int16_t XPosSecond = (int16_t) (CLOCK_RADIUS - 2) * cos((double)TO_RADIANTS(6*ActualTimeDate.Second ) - Meno90);
    int16_t YPosSecond = (int16_t) (CLOCK_RADIUS - 2) * sin((double)TO_RADIANTS(6*ActualTimeDate.Second ) - Meno90);
 
    u8g2.clearBuffer();
    u8g2.setFont(u8g_font_7x13B);
	u8g2.drawCircle(CLOCK_X_CENTER, CLOCK_Y_CENTER, CLOCK_RADIUS, U8G2_DRAW_ALL);
	u8g2.drawCircle(CLOCK_X_CENTER, CLOCK_Y_CENTER, 3, U8G2_DRAW_ALL);
	// ORE
	u8g2.drawLine(CLOCK_X_CENTER, CLOCK_Y_CENTER, CLOCK_X_CENTER + XPosHour,   CLOCK_Y_CENTER + YPosHour);  
   // MINUTI
	u8g2.drawLine(CLOCK_X_CENTER, CLOCK_Y_CENTER, CLOCK_X_CENTER + XPosMinute, CLOCK_Y_CENTER + YPosMinute);	
	// SECONDI  
	u8g2.drawLine(CLOCK_X_CENTER, CLOCK_Y_CENTER, CLOCK_X_CENTER + XPosSecond, CLOCK_Y_CENTER + YPosSecond); 
	u8g2.drawStr(RIGHT_ALIGN(Date), CENTER_POS, Date);
    u8g2.sendBuffer();   
}	

void WriteFloatToStr(float FValue, char *Str)
{
	if(FValue < 0)
		FValue = -FValue;
	if(FValue < 1.0)
	{
		FValue = ROUND_FIRST_DECIMAL(FValue);
		snprintf(Str, 5, "0.%d", (int)(FValue * 10));
	}
	else if(FValue >= 1.0 && FValue < 100.0)
	{
		FValue = ROUND_FIRST_DECIMAL(FValue);
		snprintf(Str, 5, "%d.%d", (int)((FValue * 10) / 10), (int)(FValue * 10) % 10);		
	}
}


void DrawTopInfo()
{
	char TimeStr[9], DateStr[9];
	u8g2.setFont(TOP_BOTTOM_FONT);
	snprintf(TimeStr , 9, "%02d:%02d:%02d", ActualTimeDate.Hour, ActualTimeDate.Minute, ActualTimeDate.Second);
	snprintf(DateStr , 9, "%02d/%02d/%02d", ActualTimeDate.Day, ActualTimeDate.Month, ActualTimeDate.Year % 100);
	if(Flags.PwmActive)
	{
		if(ToggleIcon)
			u8g2.drawXBM(PWM_ICON_X_POS, PWM_ICON_Y_POS, PWM_ICON_WIDHT, PWM_ICON_HIGH, PwmActive_bits);
	}
	if(Flags.LogActive)
	{
		if(ToggleIcon)
			u8g2.drawXBM(LOG_ICON_X_POS, LOG_ICON_Y_POS, LOG_ICON_WIDHT, LOG_ICON_HIGH, LogIconSml_bits);
	}
	u8g2.drawStr(LEFT_ALIGN, TOP_POS, TimeStr);	
	u8g2.drawStr(RIGHT_ALIGN(DateStr), TOP_POS, DateStr);	
	if(SmlIconTimer.hasPassed(500, true))
		ToggleIcon = !ToggleIcon;
}

static void DrawIconsForScreens(uint8_t IconNumber, uint8_t IconSelected, unsigned char *IconsBmp[])
{
	for(int i = 0; i < MAX_ICON_IN_PAGE; i++)
	{
		if(i >= IconNumber)
			break;
		u8g2.drawXBM(ICON_X_START + ((i%4)*(ICON_SCREEN_WIDHT + ICON_SCREEN_DELTA_X)), ICON_Y_START + ((i/4)*(ICON_SCREEN_HIGHT + ICON_SCREEN_DELTA_Y)),
					 ICON_BMP_WIDHT, ICON_BMP_HIGH, IconsBmp[i]);			
		if(i == IconSelected)
		{
			u8g2.drawFrame(ICON_X_START + ((i%4) * (ICON_SCREEN_WIDHT + ICON_SCREEN_DELTA_X)) - 1, ICON_Y_START + ((i/4)*(ICON_SCREEN_HIGHT + ICON_SCREEN_DELTA_Y)) - 1, 
							ICON_SCREEN_WIDHT + 1, ICON_SCREEN_HIGHT + 1);				
		}
	
	}
}

void DrawIconMenu(uint8_t WichMenu)
{
	uint8_t ItemSel = 0, Button = NO_PRESS, MinItemSel = 0, MaxItemSel = 0;
	bool ExitFromMenu = false, MenuItemSelected = false, ShowScreenSaver = false;
	Chrono ScreenSaverTimer(Chrono::SECONDS);
	if(WichMenu == MAIN_MENU)
	{
		ItemSel = 1;
		MinItemSel = 1;
		MaxItemSel = MainScreens[WichMenu].IconNumber;
	}
	else
	{
		ItemSel = 0;
		MinItemSel = 0;		
		MaxItemSel = MainScreens[WichMenu].IconNumber - 1;
	}
	while(!ExitFromMenu)
	{
		if(!ShowScreenSaver)
		{
			u8g2.clearBuffer();	
			DrawTopInfo();
			if(WichMenu == MAIN_MENU)
			{
				ItemSel--;
			}
			u8g2.setFont(TITLE_FONT);
			if(!MainScreens[WichMenu].HaveTitle)
			{			
				u8g2.drawStr(CENTER_ALIGN(MainScreens[WichMenu].IconsName[ItemSel]), TITLE_Y_POS, MainScreens[WichMenu].IconsName[ItemSel]);
			}
			else
			{
				u8g2.drawStr(CENTER_ALIGN(MainScreens[WichMenu].ScreenTitle), TITLE_Y_POS, MainScreens[WichMenu].ScreenTitle);
			}
			DrawIconsForScreens(MainScreens[WichMenu].IconNumber, ItemSel, MainScreens[WichMenu].IconsBmp);
			u8g2.sendBuffer();
			if(WichMenu == MAIN_MENU)
			{
				ItemSel++;
			}	
		}
		else
			DrawClock();		
		CheckEvents();
		Button = ButtonPress();
		switch(Button)
		{
			case UP:
				if(!ShowScreenSaver)
				{
					if(ItemSel > MinItemSel)
						ItemSel--;
					else
						ItemSel = MaxItemSel;
				}
				else
					ShowScreenSaver = false;
				break;
			case DOWN:
			case LEFT:
				if(!ShowScreenSaver)
				{
					if(ItemSel < MaxItemSel)
						ItemSel++;
					else
						ItemSel = MinItemSel;
				}
				else
					ShowScreenSaver = false;
				break;
			case OK:
				if(!ShowScreenSaver)
				{
					switch(WichMenu)
					{
						case MAIN_MENU:
							ScreenNumber = ItemSel; 
							MenuItemSelected = true;
							break;
						case POWER_CURRENT:
							ScreenNumber = SINGLE_PAGE;
							SinglePage.WichSinglePage = POWER_CURRENT_SINGLE;
							SinglePage.ItemSelected = ItemSel;
							MenuItemSelected = true;
							break;
						case DIGITAL_IN_OUT:
							ScreenNumber = SINGLE_PAGE;
							SinglePage.WichSinglePage = DIGITAL_SINGLE;
							SinglePage.ItemSelected = ItemSel;
							MenuItemSelected = true;						
							break;
						case PWM:
							ScreenNumber = SINGLE_PAGE;
							SinglePage.WichSinglePage = PULSE_W_SINGLE;
							SinglePage.ItemSelected = ItemSel;
							MenuItemSelected = true;							
							break;
						case ANALOG:
							ScreenNumber = SINGLE_PAGE;
							SinglePage.WichSinglePage = ANALOGIC_SINGLE;
							SinglePage.ItemSelected = ItemSel;
							MenuItemSelected = true;						
							break;
						case LOG:
							ScreenNumber = SINGLE_PAGE;
							SinglePage.WichSinglePage = LOG_SINGLE;
							SinglePage.ItemSelected = ItemSel;
							MenuItemSelected = true;						
							break;
						case CHRONOMETER:
							ScreenNumber = SINGLE_PAGE;
							SinglePage.WichSinglePage = CHRONO_SINGLE;
							SinglePage.ItemSelected = ItemSel;
							MenuItemSelected = true;
							break;
						case SETUP:
							ScreenNumber = SINGLE_PAGE;
							SinglePage.WichSinglePage = SETUP_SCREEN_SINGLE;
							SinglePage.ItemSelected = ItemSel;
							MenuItemSelected = true;
							break;
						default:
							break;
					}
					if(MenuItemSelected)
						ExitFromMenu = true;
				}
				else 
					ShowScreenSaver = false;
				break;
			case EXIT:
				if(!ShowScreenSaver)
				{
					ScreenNumber = MAIN_MENU;
					ExitFromMenu = true;
				}
				else
					ShowScreenSaver = false;
				break;
			default:
				break;
		}
		if(Button != NO_PRESS)
			ScreenSaverTimer.restart();
		if(ScreenSaverTimer.hasPassed(EepParameters[SCR_SAVER_EEP].ParamValue))
		{
			ShowScreenSaver = true;
		}
	}
}
