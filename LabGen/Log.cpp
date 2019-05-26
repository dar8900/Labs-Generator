#include "LabGen.h"
#include "Log.h"
#include "TimeEvents.h"
#include "Display.h"
#include "Keyboard.h"
#include "EepromSaves.h"
#include "Setup.h"


#define MAX_LIST_ELEMENT   6 // Font alto 8 che parte da 56 e con distanza tra una riga e l'altra di 1px
#define MAX_LIST_PAGES    (LOG_ARRAY_SIZE / MAX_LIST_ELEMENT) // 126 elementi nel log diviso MAX_LIST_ELEMENT

#define UP_DIRECTION	false
#define DOWN_DIRECTION	true

#define LOG_LIST_FONT 	u8g2_font_5x8_mf 

const String RecordMessage[] = 
{
	"Nessun Log",
	"Log trovati",
	"Errore!",
};



LOG_DEF LogList[MAX_LIST_ELEMENT];
uint8_t LogGraphic[LOG_ARRAY_SIZE];

static void EraseLogList(LOG_DEF *List, uint8_t Size)
{
	for(int i = 0; i < Size; i++)
	{
		List[i].LogValue = 0.0;
		List[i].Timestamp = 0;
	}
}

static int16_t SearchLastRecord()
{
	int16_t MaxTSIndex = 0;
	uint32_t MaxTimeStamp = 0;
	MaxTimeStamp = LogArray[0].Timestamp;
	for(int TSIndex = 1; TSIndex < LOG_ARRAY_SIZE; TSIndex++)
	{
		if(LogArray[TSIndex].Timestamp == 0)
			continue;
		if(MaxTimeStamp < LogArray[TSIndex].Timestamp)
		{
			MaxTimeStamp = LogArray[TSIndex].Timestamp;
			MaxTSIndex = (int16_t)TSIndex;
		}
	}
	return MaxTSIndex;
}

static uint8_t LoadListRecords(uint8_t Page, uint8_t *MaxPages, uint8_t *NumElements)
{
	int16_t LastRecord = 0;
	uint8_t ListItems = 0;
	uint8_t RetMsg = NO_RECORDS;
	LastRecord = SearchLastRecord();
	EraseLogList(LogList, MAX_LIST_ELEMENT);
	if(LogArray[LastRecord].Timestamp == 0)
		RetMsg = NO_RECORDS; 
	else
	{
		if(!Flags.LogFull)
		{
			*NumElements = LastRecord + 1;
			*MaxPages = (LastRecord / MAX_LIST_ELEMENT);
			if((LastRecord - (Page * MAX_LIST_ELEMENT)) >= 0)
				LastRecord = LastRecord - (Page * MAX_LIST_ELEMENT);
			while(ListItems < MAX_LIST_ELEMENT && LastRecord >= 0)
			{
				LogList[ListItems++] = LogArray[LastRecord--];
			}	
			if(ListItems > 0)
				RetMsg = SUCCESS;
			else
				RetMsg = FAIL;			
		}
		else
		{
			int16_t RecordIndex = LastRecord;
			*NumElements = LOG_ARRAY_SIZE;
			*MaxPages = ((LOG_ARRAY_SIZE - 1) / MAX_LIST_ELEMENT);		
			if(RecordIndex < MAX_LIST_ELEMENT - 1)
			{
				if(Page == 0)
				{
					while(RecordIndex >= 0)
					{
						LogList[ListItems++] = LogArray[RecordIndex--];
					}
				}
				RecordIndex = LOG_ARRAY_SIZE - 1 - (MAX_LIST_ELEMENT * Page);
				while(RecordIndex >= (LOG_ARRAY_SIZE - 1 - LastRecord) && ListItems < MAX_LIST_ELEMENT)
				{
					LogList[ListItems++] = LogArray[RecordIndex--];
				}
				if(ListItems > 0)
					RetMsg = SUCCESS;
				else
					RetMsg = FAIL;
			}
			else
			{
				if((RecordIndex - (Page * MAX_LIST_ELEMENT)) >= 0)
				{
					if((RecordIndex - (Page * MAX_LIST_ELEMENT)) > 0)
					{
						RecordIndex = RecordIndex - (Page * MAX_LIST_ELEMENT);
						if(RecordIndex > MAX_LIST_ELEMENT - 1)
						{
							while(ListItems < MAX_LIST_ELEMENT && RecordIndex >= 0)
							{
								LogList[ListItems++] = LogArray[RecordIndex--];
							}
						}
						else
						{
							while(RecordIndex >= 0)
							{
								LogList[ListItems++] = LogArray[RecordIndex--];
							}
							RecordIndex = LOG_ARRAY_SIZE - 1;
							while(ListItems < MAX_LIST_ELEMENT && RecordIndex >= 0)
							{
								LogList[ListItems++] = LogArray[RecordIndex--];
							}							
						}							
					}
					else
					{
						LogList[ListItems++] = LogArray[0];
						RecordIndex = LOG_ARRAY_SIZE - 1;
						while(ListItems < MAX_LIST_ELEMENT && RecordIndex >= 0)
						{
							LogList[ListItems++] = LogArray[RecordIndex--];
						}
					}				
				}
				else
				{
					RecordIndex = LOG_ARRAY_SIZE - ((Page * MAX_LIST_ELEMENT) - RecordIndex);
					while(ListItems < MAX_LIST_ELEMENT && RecordIndex > LastRecord)
					{
						LogList[ListItems++] = LogArray[RecordIndex--];
					}					
				}
				if(ListItems > 0)
					RetMsg = SUCCESS;
				else
					RetMsg = FAIL;	
			}
		}
	}
	return RetMsg;
}

static uint8_t LoadGraphicRecords(uint8_t *LogsFound)
{
	uint8_t RetMsg = 0, LastRecord = 0;
	LastRecord = SearchLastRecord();
	if(LogArray[LastRecord].Timestamp == 0)
		RetMsg = NO_RECORDS; 
	else
	{
		if(!Flags.LogFull)
		{
			for(int i = 0; i < LOG_ARRAY_SIZE; i++)
			{
				LogGraphic[i] = (uint8_t)roundf(LogArray[i].LogValue);
			}
			*LogsFound = LastRecord + 1;
			RetMsg = SUCCESS;
		}
		else
		{
			for(int i = 0; i <= LastRecord; i++)
			{
				LogGraphic[LOG_ARRAY_SIZE - 1 - i] = (uint8_t)roundf(LogArray[LastRecord - i].LogValue);
			}
			for(int i = LOG_ARRAY_SIZE - 1; i > LastRecord; i--)
			{
				LogGraphic[i - LastRecord] = (uint8_t)roundf(LogArray[i].LogValue);
			}
			*LogsFound = LOG_ARRAY_SIZE - 1;
			RetMsg = SUCCESS;		
		}
	}

	return RetMsg;
}

static uint8_t SearchGraphicChangedIndex(uint8_t ActualCursor)
{
	uint8_t Index = ActualCursor;
	int16_t LastRecord = 0;
	if(Flags.LogFull)
	{
		LastRecord = SearchLastRecord();
		Index = LastRecord - (LOG_ARRAY_SIZE - 1  - ActualCursor);
	}
	return Index;
}

void DrawListLog()
{
	bool ExitLogList = false, ReloadPage = true;
	uint8_t Button = NO_PRESS, LogMessage = 0, ListPage = 0, MaxPages = 0, NumElements = 0;
	char LogTimeDateStr[19];
	Chrono ReloadPageTimer;
	TIME_DATE_VAR LogTimeDate_t;
	String LogString = "";
	LogMessage = LoadListRecords(ListPage, &MaxPages, &NumElements);
	if(LogMessage == SUCCESS)
	{
		String SuccMsg = "";
		SuccMsg = "Trovati " + String(NumElements) + " log";
		ShowMessage(SuccMsg);
	}
	while(!ExitLogList)
	{
		switch(LogMessage)
		{
			case NO_RECORDS:
				ShowMessage(RecordMessage[NO_RECORDS]);		
				ExitLogList = true;
				break;
			case FAIL:
				ShowMessage(RecordMessage[FAIL]);		
				ExitLogList = true;
				break;
			default:
				break;
		}
		if(ExitLogList)
			break;
		if(ReloadPage)
		{
			Button = NO_PRESS;
			u8g2.clearBuffer();
			DrawTopInfo();
			u8g2.setFont(LOG_LIST_FONT);
			for(int i = 0; i < MAX_LIST_ELEMENT; i++)
			{
				if(LogList[i].Timestamp == 0)
					break;
				DateTime LogTimeDate(LogList[i].Timestamp + SECONDS_FROM_1970_TO_2000);
				// LogTimeDate_t.Hour   = LogTimeDate.hour();
				// LogTimeDate_t.Minute = LogTimeDate.minute();
				// LogTimeDate_t.Second = LogTimeDate.second();
				// LogTimeDate_t.Day    = LogTimeDate.day();
				// LogTimeDate_t.Month  = LogTimeDate.month();
				// LogTimeDate_t.Year  = LogTimeDate.year();
				snprintf(LogTimeDateStr, 18, "%02d/%02d/%02d %02d:%02d:%02d", LogTimeDate.day(), LogTimeDate.month(), LogTimeDate.year() % 100, 
							LogTimeDate.hour(), LogTimeDate.minute(), LogTimeDate.second());
				LogString = String(LogList[i].LogValue, 2) + String(UdmLog[EepParameters[LOG_UDM_EEP].ParamValue]); 
				u8g2.drawStr(LEFT_ALIGN, LOG_LIST_Y_POS + (i * (STR_HIGH + 3)), LogTimeDateStr);
				u8g2.drawStr(RIGHT_ALIGN(LogString.c_str()), LOG_LIST_Y_POS + (i * (STR_HIGH + 3)), LogString.c_str());
			}
			u8g2.sendBuffer();
			ReloadPage = false;
		}
		CheckEvents();
		Button = ButtonPress();
		switch(Button)
		{
			case UP:
				if(ListPage > 0)
					ListPage--;
				else 
					ListPage = MaxPages;
				break;
			case DOWN:			
				if(ListPage < MaxPages)
					ListPage++;
				else 
					ListPage = 0;
				break;
			case LEFT:
				break;
			case OK:
				break;
			case EXIT:
				ExitLogList = true;
				break;
			default:
				break;			
		}
		if(Button != NO_PRESS)
		{
			// ReloadPage = true;
			DBG(Button);
			LogMessage = LoadListRecords(ListPage, &MaxPages, &NumElements);
		}
		if(ReloadPageTimer.hasPassed(500, true))
			ReloadPage = true;
	}

}

static uint8_t SearchMaxValueGraphicArray()
{
	uint8_t MaxVal = LogGraphic[0];
	for(int i = 1; i < LOG_ARRAY_SIZE; i++)
	{
		if(MaxVal < LogGraphic[i])
			MaxVal = LogGraphic[i];
	}
	return MaxVal;
}

void DrawGaphicLog()
{
	bool ExitLogGraphic = false, CursorActive = false;
	uint8_t LoadMsg = 0, Button = NO_PRESS, CursorPos = LOG_ARRAY_SIZE - 1, RealCursor = 0, YFocus = 0;
	uint8_t LogsFound = 0, MaxValue = 0;
	uint8_t XPos = 0, YPos = 0, YPosOld = 0;
	int8_t YValue = 0;
	char LogTimeDateStr[28];
	String LogString;
	bool First = true;	
	LoadMsg = LoadGraphicRecords(&LogsFound);
	if(LoadMsg != SUCCESS)
		ShowMessage(RecordMessage[NO_RECORDS]);
	else
	{
		ShowMessage("Trovati " + String(LogsFound) + " log");
		MaxValue = SearchMaxValueGraphicArray();
		while(!ExitLogGraphic)
		{
			u8g2.clearBuffer();
			DrawTopInfo();	
			if(CursorActive)
			{
				RealCursor = SearchGraphicChangedIndex(CursorPos);
				DateTime LogTimeDate(LogArray[RealCursor].Timestamp + SECONDS_FROM_1970_TO_2000);
				LogString = String(LogArray[RealCursor].LogValue, 2) + String(UdmLog[EepParameters[LOG_UDM_EEP].ParamValue]);
				snprintf(LogTimeDateStr, 24, "%02d/%02d/%02d %02d:%02d:%02d %s", LogTimeDate.day(), LogTimeDate.month(), LogTimeDate.year() % 100, 
							LogTimeDate.hour(), LogTimeDate.minute(), LogTimeDate.second(), LogString.c_str()); 
				u8g2.setFont(LOG_GRAPHIC_FONT);
				u8g2.drawStr(CENTER_ALIGN(LogTimeDateStr), LOG_GRAPHIC_STR_Y_POS, LogTimeDateStr);
			}
			u8g2.drawFrame(LOG_GRAPHIC_X_START, LOG_GRAPHIC_Y_START, LOG_GRAPHIC_WIDHT, LOG_GRAPHIC_HIGH);
			for(int i = 0; i < LOG_ARRAY_SIZE; i++)
			{
				XPos = i + 1;
				YValue = (int8_t)((LogGraphic[i] * LOG_GRAPHIC_HIGH) / MaxValue);
				YPos = ((LOG_GRAPHIC_HIGH - 1) + LOG_GRAPHIC_Y_START + 1) - YValue;
				if(YPos > LOG_GRAPHIC_HIGH + LOG_GRAPHIC_Y_START - 1)
					YPos = LOG_GRAPHIC_HIGH + LOG_GRAPHIC_Y_START - 2;
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
				if(CursorActive)
				{
					
					if(XPos == CursorPos)
						YFocus = YPos;
					u8g2.drawCircle(CursorPos, YFocus, 2, U8G2_DRAW_ALL);
				}
			}
			u8g2.sendBuffer();
			CheckEvents();
			Button = ButtonPress();			
			switch(Button)
			{
				case UP:
					if(CursorActive)
					{
						if(CursorPos > 1)
							CursorPos--;
						else
							CursorPos = LOG_ARRAY_SIZE - 1;
					}
					break;
				case DOWN:
					if(CursorActive)
					{
						if(CursorPos < LOG_ARRAY_SIZE - 1)
							CursorPos++;
						else
							CursorPos = 1;						
					}				
					break;
				case LEFT:
					if(CursorActive)
					{
						if(CursorPos < LOG_ARRAY_SIZE - 1)
							CursorPos++;
						else
							CursorPos = 1;						
					}	
					break;
				case OK:	
					CursorActive = !CursorActive;
					break;
				case EXIT:
					ExitLogGraphic = true;
					break;
				default:
					break;
			}

		}
	}
}

void ResetLogFunction()
{
	ShowMessage("Reset in corso");
	ResetLogRecords();
	ShowMessage("Reset eseguito");
}