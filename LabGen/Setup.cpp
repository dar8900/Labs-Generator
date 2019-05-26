#include "LabGen.h"
#include "TimeEvents.h"
#include "Display.h"
#include "Keyboard.h"
#include "Setup.h"
#include "EepromSaves.h"

enum
{
	IS_TIME = 0,
	IS_DATE
};

typedef enum
{
	HOUR_DAY = 0,
	MONTH_MINUTE,
	YEAR_SECOND,
	MAX_BOX_SEL
}BOX_SELECTION;

typedef enum
{ 
	HOUR,
	MINUTE,
	SECOND,
	DAY,
	MONTH,
	YEAR,
	MAX_PERIODS
}TIME_DATE_PERIOD;

typedef struct
{
	uint8_t WichPeriod;
	uint8_t MaxVal;
	uint8_t MinVal;
}TIME_DATE_DEF;


static TIME_DATE_DEF TimeDateRanges[] = 
{
	{HOUR  , 23,  0},
	{MINUTE, 59,  0},
	{SECOND, 59,  0},
	{DAY   , 31,  1},
	{MONTH , 12,  1},
	{YEAR  , 99, 19},
};

static const ENUM_DEF LogUdm[MAX_LOG_UDM] = 
{
	{VOLTAGE,    "Volt V"       },
	{AMPERE,     "Ampere A" 	},
	{CELSIUS,    "Celsius °C"	},
	{METER,      "Metri m" 		},
	{CENTIMETER, "Centimetri cm"},
	{LUMEN,      "Lumen ln"     },
};
const char *UdmLog[MAX_LOG_UDM] = 
{
	"V",
	"A",
	"°C",
	"m",
	"cm",
	"ln",
};

static char *LogUdmDescription = "Selezionare udm log";

const char *BottomStr[] = 
{
	"Ok",
	"Indietro",
};

const PARAMETERS_DEF ParametersTab[MAX_ITEM] = 
{
	{"Set tempo" ,            0,        0,   TIME_TYPE,   NO_TYPE, 		  			                  NULL,  NULL,   NULL,                  NULL},
	{"Set data"  ,            0,        0,   DATE_TYPE,   NO_TYPE, 		  			                  NULL,  NULL,   NULL,                  NULL},
	{"P-I frq."  ,       100000,        1,  VALUE_TYPE, UINT_TYPE, &EepParameters[P_I_FREQ_EEP].ParamValue,  "Hz",   NULL,                  NULL},
	{"Tempo log" ,         1800,        1,  VALUE_TYPE, UINT_TYPE, &EepParameters[LOG_TIME_EEP].ParamValue,   "s",   NULL,                  NULL},
	{"ADC Graph.",         2000,       25,  VALUE_TYPE, UINT_TYPE, &EepParameters[ADC_TIME_EEP].ParamValue,  "ms",   NULL,                  NULL},
	{"Log udm."  ,  MAX_LOG_UDM,  VOLTAGE,   ENUM_TYPE,   NO_TYPE, &EepParameters[LOG_UDM_EEP].ParamValue ,   "" , LogUdm,     LogUdmDescription},
	{"S.S. delay",          600,        5,  VALUE_TYPE, UINT_TYPE, &EepParameters[SCR_SAVER_EEP].ParamValue,  "s",   NULL,                  NULL},

};

static void PrintTimeOrDate(uint8_t TimeOrDate, char *TimeDateStr, uint8_t Values[])
{
	if(TimeOrDate == IS_TIME)
	{
		snprintf(TimeDateStr, 9, "%02d:%02d:%02d", Values[HOUR_DAY], Values[MONTH_MINUTE], Values[YEAR_SECOND]);
	}
	else
	{
		snprintf(TimeDateStr, 9, "%02d/%02d/%02d", Values[HOUR_DAY], Values[MONTH_MINUTE], Values[YEAR_SECOND]);
	}
}

static void DrawSetupBottomBar(bool IsEnum, uint8_t ActualItem, uint8_t MaxItem)
{
	u8g2.setFont(TOP_BOTTOM_FONT);	
	u8g2.drawStr(LEFT_ALIGN, BOTTOM_POS, BottomStr[0]);
	u8g2.drawStr(RIGHT_ALIGN(BottomStr[1]), BOTTOM_POS, BottomStr[1]);
	if(IsEnum)
	{
		String HowManyItem = String(ActualItem + 1) + "/" + String(MaxItem);
		u8g2.drawStr(CENTER_ALIGN(HowManyItem.c_str()), BOTTOM_POS, HowManyItem.c_str());
	}
}

static bool ChangeTimeDate(uint8_t TimeOrDate)
{
	uint8_t BoxSel = 0, WichTabIndex = 0, BoxValues[3] = {0}, Button = NO_PRESS; 
	char TimeDateStr[9];
	bool ExitChangeTimeDate = false;
	if(TimeOrDate == IS_TIME)
	{
		WichTabIndex = TIME_ITEM;
		BoxValues[0] = ActualTimeDate.Hour;
		BoxValues[1] = ActualTimeDate.Minute;
		BoxValues[2] = ActualTimeDate.Second;
	}
	else
	{
		WichTabIndex = DATE_ITEM;
		BoxValues[0] = ActualTimeDate.Day;
		BoxValues[1] = ActualTimeDate.Month;
		BoxValues[2] = (uint8_t)(ActualTimeDate.Year % 100);
		TimeDateRanges[DAY - 3].MaxVal = DayInMonth[ActualTimeDate.Month - 1];
	}
	PrintTimeOrDate(TimeOrDate, TimeDateStr, BoxValues);
	while(!ExitChangeTimeDate)
	{
		u8g2.clearBuffer();	
		DrawTopInfo();
		u8g2.setFont(TITLE_FONT);		
		u8g2.drawStr(CENTER_ALIGN(ParametersTab[WichTabIndex].ParamTitle), TITLE_Y_POS, ParametersTab[WichTabIndex].ParamTitle);
		u8g2.setFont(TIME_DATE_STR_FONT);
		PrintTimeOrDate(TimeOrDate, TimeDateStr, BoxValues);
		u8g2.drawStr(CENTER_ALIGN(TimeDateStr), TIME_DATE_STR_Y_POS, TimeDateStr);
		u8g2.drawFrame((CENTER_ALIGN(TimeDateStr) - 1) + (BoxSel * (TIME_DATE_BOX_WIDTH + TIME_SEPAR_WIDHT)) - (1*BoxSel), 
						TIME_DATE_BOX_Y_POS, 
						TIME_DATE_BOX_WIDTH, TIME_DATE_BOX_HIGH);				
		DrawSetupBottomBar(false , 0, 0);
		u8g2.sendBuffer();
		Button = ButtonPressContinue();
		CheckEvents();
		switch(Button)
		{
			case UP:
				if(TimeOrDate == IS_TIME)
				{
					if(BoxValues[BoxSel] > TimeDateRanges[BoxSel].MinVal)
						BoxValues[BoxSel]--;
					else
						BoxValues[BoxSel] = TimeDateRanges[BoxSel].MaxVal;
				}
				else
				{
					if(BoxValues[BoxSel] > TimeDateRanges[BoxSel + 3].MinVal)
						BoxValues[BoxSel]--;
					else
						BoxValues[BoxSel] = TimeDateRanges[BoxSel + 3].MaxVal;				
				}
				break;
			case DOWN:
				if(TimeOrDate == IS_TIME)
				{
					if(BoxValues[BoxSel] < TimeDateRanges[BoxSel].MaxVal)
						BoxValues[BoxSel]++;
					else
						BoxValues[BoxSel] = TimeDateRanges[BoxSel].MinVal;
				}
				else
				{
					if(BoxValues[BoxSel] < TimeDateRanges[BoxSel + 3].MaxVal)
						BoxValues[BoxSel]++;
					else
						BoxValues[BoxSel] = TimeDateRanges[BoxSel + 3].MinVal;				
				}		
				break;
			case LEFT:
				if(BoxSel < (MAX_BOX_SEL - 1))
					BoxSel++;
				else
					BoxSel = 0;
				break;
			case OK:
				if(TimeOrDate == IS_TIME)
				{
					SetTime(BoxValues[HOUR_DAY], BoxValues[MONTH_MINUTE]);
				}
				else
				{
					SetDate(BoxValues[HOUR_DAY], BoxValues[MONTH_MINUTE], BoxValues[YEAR_SECOND] + 2000);
				}
				ExitChangeTimeDate = true;
				break;
			case EXIT:
				ExitChangeTimeDate = true;
				break;
			default:
				break;
		}
	}
	return false;
}


bool ChangeValue(uint8_t ValueType, uint32_t *ValuePtr, uint8_t ParamIdx)
{
	uint32_t ModValue = 0;
	bool NegativeValue = false, ExitChangeValue = false, Save = false, WrongNumber = false;
	uint8_t Button = NO_PRESS, NBox = 0, BoxSel = 0;
	char ValueStr[9], Format[7];
	switch(ValueType)
	{
		case UINT_TYPE:
			ModValue = *ValuePtr;
			break;
		default:
			break;
	}
	if(ParametersTab[ParamIdx].MaxValue < 10)
		NBox = 1;
	else if(ParametersTab[ParamIdx].MaxValue >= 10 && ParametersTab[ParamIdx].MaxValue < 100)
		NBox = 2;
	else if(ParametersTab[ParamIdx].MaxValue >= 100 && ParametersTab[ParamIdx].MaxValue < 1000)
		NBox = 3;
	else if(ParametersTab[ParamIdx].MaxValue >= 1000 && ParametersTab[ParamIdx].MaxValue < 10000)
		NBox = 4;
	else if(ParametersTab[ParamIdx].MaxValue >= 10000 && ParametersTab[ParamIdx].MaxValue < 100000)
		NBox = 5;
	else if(ParametersTab[ParamIdx].MaxValue >= 100000 && ParametersTab[ParamIdx].MaxValue < 1000000)
		NBox = 6;
	else if(ParametersTab[ParamIdx].MaxValue >= 1000000 && ParametersTab[ParamIdx].MaxValue < 10000000)
		NBox = 7;
	if(NegativeValue)
		snprintf(Format, 6, "-%%0%dd", NBox);
	else
		snprintf(Format, 6, "%%0%dd", NBox);
	snprintf(ValueStr, NBox + 1, Format, ModValue);
	while(!ExitChangeValue)
	{
		u8g2.clearBuffer();	
		DrawTopInfo();
		u8g2.setFont(TITLE_FONT);		
		u8g2.drawStr(CENTER_ALIGN(ParametersTab[ParamIdx].ParamTitle), TITLE_Y_POS, ParametersTab[ParamIdx].ParamTitle);
	
		u8g2.setFont(VALUES_FONT);	
		u8g2.drawStr(CENTER_ALIGN(ValueStr), VALUES_STR_Y_POS, ValueStr);
		u8g2.drawFrame((CENTER_ALIGN(ValueStr) - 1) + (BoxSel * VALUES_BOX_WIDHT) - (2 * BoxSel), 
						VALUES_BOX_Y_POS, 
						VALUES_BOX_WIDHT, VALUES_BOX_HIGH);		
		u8g2.setFont(VALUES_UDM_FONT);	
		if(!WrongNumber)
			u8g2.drawStr(CENTER_ALIGN(ParametersTab[ParamIdx].ValuesUdm), VALUES_UDM_Y_POS, ParametersTab[ParamIdx].ValuesUdm);
		else		
			u8g2.drawStr(CENTER_ALIGN("Fuori range"), VALUES_UDM_Y_POS, "Fuori range");
		DrawSetupBottomBar(false, 0, 0);
		u8g2.sendBuffer();
		if(WrongNumber)
		{
			delay(1000);
			WrongNumber = false;
		}
		CheckEvents();
		Button = ButtonPressContinue();	
		switch(Button)
		{
			case UP:
				if(strchr("+-0123456789", ValueStr[BoxSel]) != NULL)
				{
					if(ValueStr[BoxSel] == '+')
					{
						ValueStr[BoxSel] = '-';
						NegativeValue = true;
					}
					else if(ValueStr[BoxSel] == '-')
					{
						ValueStr[BoxSel] = '+';		
						NegativeValue = false;						
					}						
					if(ValueStr[BoxSel] > '0')
						ValueStr[BoxSel]--;
					else
						ValueStr[BoxSel] = '9';
				}
				break;
			case DOWN:
				if(strchr("+-0123456789", ValueStr[BoxSel]) != NULL)
				{
					if(ValueStr[BoxSel] == '+')
					{
						ValueStr[BoxSel] = '-';
						NegativeValue = true;
					}
					else if(ValueStr[BoxSel] == '-')
					{
						ValueStr[BoxSel] = '+';		
						NegativeValue = false;						
					}					
					if(ValueStr[BoxSel] < '9')
						ValueStr[BoxSel]++;
					else
						ValueStr[BoxSel] = '0';
				}
				break;
			case LEFT:
				if(BoxSel < NBox-1)
					BoxSel++;
				else
					BoxSel = 0;
				break;
			case OK:
				ModValue = atol(ValueStr);
				if(ModValue < ParametersTab[ParamIdx].MinValue || ModValue > ParametersTab[ParamIdx].MaxValue)
				{
					WrongNumber = true;
				}
				else
				{
					switch(ValueType)
					{
						case UINT_TYPE:
							*ValuePtr = ModValue;
							break;
						default:
							break;
					}	
					ExitChangeValue = true;
					Save = true;
				}
				break;
			case EXIT:
				ExitChangeValue = true;
				Save = false;
				break;
			default:
				break;
		}
	}
	return Save;
}

bool ChangeEnum(uint32_t *ValuePtr, uint8_t ParamIdx)
{
	bool ExitChangeValue = false, Save = false;
	uint8_t Button = NO_PRESS, Item = (uint8_t)*ValuePtr;
	
	while(!ExitChangeValue)
	{
		u8g2.clearBuffer();	
		DrawTopInfo();
		u8g2.setFont(TITLE_FONT);		
		u8g2.drawStr(CENTER_ALIGN(ParametersTab[ParamIdx].ParamTitle), TITLE_Y_POS, ParametersTab[ParamIdx].ParamTitle);
		u8g2.setFont(ENUM_DESCR_FONT);
		u8g2.drawStr(CENTER_ALIGN(ParametersTab[ParamIdx].EnumDescription), ENUM_DESCR_Y_POS, ParametersTab[ParamIdx].EnumDescription);
		u8g2.setFont(ENUM_VALUE_FONT);
		u8g2.drawStr(CENTER_ALIGN(ParametersTab[ParamIdx].EnumVar[Item].EnumStr), ENUM_VALUE_Y_POS, ParametersTab[ParamIdx].EnumVar[Item].EnumStr);		
		DrawSetupBottomBar(true, Item, ParametersTab[ParamIdx].MaxValue);
		u8g2.sendBuffer();
		CheckEvents();
		Button = ButtonPressContinue();	
		switch(Button)
		{
			case UP:
				if(Item > 0)
					Item--;
				else
					Item = ParametersTab[ParamIdx].MaxValue - 1;
				break;
			case DOWN:
				if(Item < ParametersTab[ParamIdx].MaxValue - 1)
					Item++;
				else
					Item = 0;
				break;
			case LEFT:

				break;
			case OK:
				*ValuePtr = (uint32_t)ParametersTab[ParamIdx].EnumVar[Item].Value;
				ExitChangeValue = true;
				Save = true;
				break;
			case EXIT:
				ExitChangeValue = true;
				Save = false;
				break;
			default:
				break;
		}
	}
	return Save;	
}

void EnterSetup(uint8_t WichSetup)
{
	bool SaveParameters = false;
	switch(ParametersTab[WichSetup].ParamType)
	{
		case TIME_TYPE:
			SaveParameters = ChangeTimeDate(IS_TIME);
			break;
		case DATE_TYPE:
			SaveParameters = ChangeTimeDate(IS_DATE);
			break;
		case VALUE_TYPE:
			SaveParameters = ChangeValue(ParametersTab[WichSetup].ValueType, ParametersTab[WichSetup].ParamValuePtr, WichSetup);
			break;
		case ENUM_TYPE:
			SaveParameters = ChangeEnum(ParametersTab[WichSetup].ParamValuePtr, WichSetup);
			break;
		default:
			break;
	}
	if(SaveParameters)
	{
		EeFlags.SaveParameters = true;// Alzare flag per salvataggio parametri in eeprom
	}
}