#include "usbd_cat_if.h"
#include "functions.h"
#include "settings.h"
#include "trx_manager.h"
#include <stdlib.h>
#include "lcd.h"
#include "fpga.h"
#include "cw.h"
#include "audio_filters.h"

#define CAT_APP_RX_DATA_SIZE 32
#define CAT_APP_TX_DATA_SIZE 32

#define CAT_BUFFER_SIZE 64
static char cat_buffer[CAT_BUFFER_SIZE] = {0};
static uint8_t cat_buffer_head = 0;
static char command_to_parse1[CAT_BUFFER_SIZE] = {0};
static char command_to_parse2[CAT_BUFFER_SIZE] = {0};
static uint8_t CAT_UserRxBufferFS[CAT_APP_RX_DATA_SIZE];
static uint8_t CAT_UserTxBufferFS[CAT_APP_TX_DATA_SIZE];
static uint8_t lineCoding[7] = {0x00, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x08}; // 115200bps, 1stop, no parity, 8bit

extern USBD_HandleTypeDef hUsbDeviceFS;

static void getFT450Mode(uint8_t VFO_Mode, char* out);
static uint8_t setFT450Mode(char *FT450_Mode);
static int8_t CAT_Init_FS(void);
static int8_t CAT_DeInit_FS(void);
static int8_t CAT_Control_FS(uint8_t cmd, uint8_t *pbuf);
static int8_t CAT_Receive_FS(uint8_t *pbuf, uint32_t *Len);
static void CAT_Transmit(char *data);
static uint8_t CAT_Transmit_FS(uint8_t *Buf, uint16_t Len);

USBD_CAT_ItfTypeDef USBD_CAT_fops_FS =
	{
		CAT_Init_FS,
		CAT_DeInit_FS,
		CAT_Control_FS,
		CAT_Receive_FS};

static int8_t CAT_Init_FS(void)
{
	/* USER CODE BEGIN 3 */
	/* Set Application Buffers */
	USBD_CAT_SetTxBuffer(&hUsbDeviceFS, CAT_UserTxBufferFS, 0);
	USBD_CAT_SetRxBuffer(&hUsbDeviceFS, CAT_UserRxBufferFS);
	return (USBD_OK);
	/* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CAT_DeInit_FS(void)
{
	/* USER CODE BEGIN 4 */
	return (USBD_OK);
	/* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CAT_Control_FS(uint8_t cmd, uint8_t *pbuf)
{
	/* USER CODE BEGIN 5 */
	switch (cmd)
	{
	case CDC_SEND_ENCAPSULATED_COMMAND:

		break;

	case CDC_GET_ENCAPSULATED_RESPONSE:

		break;

	case CDC_SET_COMM_FEATURE:

		break;

	case CDC_GET_COMM_FEATURE:

		break;

	case CDC_CLEAR_COMM_FEATURE:

		break;

		/*******************************************************************************/
		/* Line Coding Structure                                                       */
		/*-----------------------------------------------------------------------------*/
		/* Offset | Field       | Size | Value  | Description                          */
		/* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
		/* 4      | bCharFormat |   1  | Number | Stop bits                            */
		/*                                        0 - 1 Stop bit                       */
		/*                                        1 - 1.5 Stop bits                    */
		/*                                        2 - 2 Stop bits                      */
		/* 5      | bParityType |  1   | Number | Parity                               */
		/*                                        0 - None                             */
		/*                                        1 - Odd                              */
		/*                                        2 - Even                             */
		/*                                        3 - Mark                             */
		/*                                        4 - Space                            */
		/* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
		/*******************************************************************************/
	case CDC_SET_LINE_CODING:
		memcpy(lineCoding, pbuf, sizeof(lineCoding));
		break;

	case CDC_GET_LINE_CODING:
		memcpy(pbuf, lineCoding, sizeof(lineCoding));
		break;

	case CDC_SET_CONTROL_LINE_STATE:
		if ((pbuf[2] & 0x2) == 0x2) //RTS
		{
			TRX_ptt_soft = true;
		}
		else
		{
			TRX_ptt_soft = false;
		}
		if ((pbuf[2] & 0x1) == 0x1) //DTR
		{
			CW_key_serial = true;
		}
		else
		{
			CW_key_serial = false;
		}
		break;

	case CDC_SEND_BREAK:

		break;

	default:
		break;
	}

	return (USBD_OK);
	/* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CAT_Receive_FS(uint8_t *Buf, uint32_t *Len)
{
	char charBuff[CAT_BUFFER_SIZE] = {0};
	strncpy(charBuff, (char *)Buf, Len[0]);
	memset(&Buf, 0, Len[0]);
	if (Len[0] <= CAT_BUFFER_SIZE)
	{
		for (uint16_t i = 0; i < Len[0]; i++)
		{
			if (charBuff[i] != 0)
			{
				cat_buffer[cat_buffer_head] = charBuff[i];
				if (cat_buffer[cat_buffer_head] == ';')
				{
					if(strlen(command_to_parse1) == 0)
					{
						memset(command_to_parse1, 0, sizeof(command_to_parse1));
						memcpy(command_to_parse1, cat_buffer, cat_buffer_head);
					}
					else if(strlen(command_to_parse2) == 0)
					{
						memset(command_to_parse2, 0, sizeof(command_to_parse2));
						memcpy(command_to_parse2, cat_buffer, cat_buffer_head);
					}
					
					cat_buffer_head = 0;
					memset(cat_buffer, 0, CAT_BUFFER_SIZE);
					continue;
				}
				cat_buffer_head++;
				if (cat_buffer_head >= CAT_BUFFER_SIZE)
				{
					cat_buffer_head = 0;
					memset(&cat_buffer, 0, CAT_BUFFER_SIZE);
				}
			}
		}
	}
	return (USBD_OK);
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
static uint8_t CAT_Transmit_FS(uint8_t *Buf, uint16_t Len)
{
	uint8_t result = USBD_OK;
	USBD_CAT_HandleTypeDef *hcdc = (USBD_CAT_HandleTypeDef *)hUsbDeviceFS.pClassDataCAT;
	if (hcdc->TxState != 0)
	{
		return USBD_BUSY;
	}
	USBD_CAT_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
	result = USBD_CAT_TransmitPacket(&hUsbDeviceFS);
	return result;
}

static void CAT_Transmit(char *data)
{
	CAT_Transmit_FS((uint8_t *)data, (uint16_t)strlen(data));
	//sendToDebug_str3("CAT command answer: |",data,"|\r\n");
}

void ua3reo_dev_cat_parseCommand(void)
{
	USBD_CAT_ReceivePacket(&hUsbDeviceFS); //prepare next command
	if (command_to_parse1[0] == 0 && command_to_parse2[0] == 0)
		return;

	char _command_buffer[CAT_BUFFER_SIZE] = {0};
	char *_command = _command_buffer;
	if(strlen(command_to_parse1) > 0)
	{
		memcpy(_command, command_to_parse1, CAT_BUFFER_SIZE);
		memset(command_to_parse1, 0, CAT_BUFFER_SIZE);
	}
	else if(strlen(command_to_parse2) > 0)
	{
		memcpy(_command, command_to_parse2, CAT_BUFFER_SIZE);
		memset(command_to_parse2, 0, CAT_BUFFER_SIZE);
	}
	
	while (*_command == '\r' || *_command == '\n' || *_command == ' ') //trim
		_command++;
	if (strlen(_command) < 2)
		return;
	//sendToDebug_str3("New CAT command: |",_command,"|\r\n");

	char command[3] = {0};
	strncpy(command, _command, 2);
	bool has_args = false;
	char arguments[32] = {0};
	char ctmp[30] = {0};
	if (strlen(_command) > 2)
	{
		strncpy(arguments, _command + 2, strlen(_command) - 2);
		has_args = true;
	}
	//sendToDebug_str3("Arguments: |",arguments,"|\r\n");

	if (strcmp(command, "AI") == 0) // AUTO INFORMATION
	{
		if (!has_args)
		{
			CAT_Transmit("AI0;");
		}
		else
		{
			//ничего не делаем, автоуведомления и так не работают
		}
		return;
	}

	if (strcmp(command, "ID") == 0) // IDENTIFICATION
	{
		if (!has_args)
		{
			CAT_Transmit("ID0241;");
		}
		else
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "FT") == 0) // FUNCTION TX
	{
		if (!has_args)
		{
			if(!TRX.CLAR)
				CAT_Transmit("FT0;");
			else
				CAT_Transmit("FT1;");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				TRX.CLAR = false;
			}
			else if (strcmp(arguments, "1") == 0)
			{
				TRX.CLAR = true;
			}
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "VS") == 0) // VFO SELECT
	{
		if (!has_args)
		{
			if (!TRX.current_vfo)
				CAT_Transmit("VS0;");
			else
				CAT_Transmit("VS1;");
		}
		else
		{
			uint8_t new_vfo = 0;
			if (strcmp(arguments, "0") == 0)
				new_vfo = 0;
			else if (strcmp(arguments, "1") == 0)
				new_vfo = 1;
			if(TRX.current_vfo != new_vfo)
			{
				TRX.current_vfo = new_vfo;
				/*if(!TRX.current_vfo)
				{
					CurrentVFO = &TRX.VFO_A;
					SecondaryVFO = &TRX.VFO_B;
				}
				else
				{
					CurrentVFO = &TRX.VFO_B;
					SecondaryVFO = &TRX.VFO_A;
				}*/
				LCD_UpdateQuery.TopButtons = true;
				LCD_UpdateQuery.BottomButtons = true;
				LCD_UpdateQuery.FreqInfoRedraw = true;
				LCD_UpdateQuery.StatusInfoGUI = true;
				LCD_UpdateQuery.StatusInfoBarRedraw = true;
				NeedSaveSettings = true;
				NeedReinitAudioFilters = true;
				FFT_Init();
			}
			sendToDebug_str3("CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "IF") == 0) // INFORMATION
	{
		if (!has_args)
		{
			char answer[30] = {0};
			strcat(answer, "IF001"); //memory channel
			if (CurrentVFO()->Freq < 10000000)
				strcat(answer, "0");
			sprintf(ctmp, "%u", CurrentVFO()->Freq);
			strcat(answer, ctmp);	 //freq
			strcat(answer, "+0000"); //clirifier offset
			strcat(answer, "0");	 //RX clar off
			strcat(answer, "0");	 //TX clar off
			char mode[3] = {0};
			getFT450Mode((uint8_t)CurrentVFO()->Mode, mode);
			strcat(answer, mode); //mode
			strcat(answer, "0");  //VFO Memory
			strcat(answer, "0");  //CTCSS OFF
			strcat(answer, "00"); //TONE NUMBER
			strcat(answer, "0;"); //Simplex
			CAT_Transmit(answer);
		}
		else
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "OI") == 0) // OPPOSITE BAND INFORMATION
	{
		if (!has_args)
		{
			char answer[30] = {0};
			strcat(answer, "OI001"); //memory channel
			if (SecondaryVFO()->Freq < 10000000)
				strcat(answer, "0");
			sprintf(ctmp, "%u", SecondaryVFO()->Freq);
			strcat(answer, ctmp);	 //freq
			strcat(answer, "+0000"); //clirifier offset
			strcat(answer, "0");	 //RX clar off
			strcat(answer, "0");	 //TX clar off
			char mode[3] = {0};
			getFT450Mode((uint8_t)SecondaryVFO()->Mode, mode);
			strcat(answer, mode); //mode
			strcat(answer, "0");  //VFO Memory
			strcat(answer, "0");  //CTCSS OFF
			strcat(answer, "00"); //TONE NUMBER
			strcat(answer, "0;"); //Simplex
			CAT_Transmit(answer);
		}
		else
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}
	
	if (strcmp(command, "FA") == 0) // FREQUENCY VFO-A
	{
		if (!has_args)
		{
			char answer[30] = {0};
			strcat(answer, "FA");
			if (TRX.VFO_A.Freq < 10000000)
				strcat(answer, "0");
			sprintf(ctmp, "%u", TRX.VFO_A.Freq);
			strcat(answer, ctmp); //freq
			strcat(answer, ";");
			CAT_Transmit(answer);
		}
		else
		{
			TRX_setFrequency((uint32_t)atoi(arguments), CurrentVFO());
			LCD_UpdateQuery.FreqInfo = true;
			LCD_UpdateQuery.TopButtons = true;
		}
		return;
	}

	if (strcmp(command, "FB") == 0) // FREQUENCY VFO-B
	{
		if (!has_args)
		{
			char answer[30] = {0};
			strcat(answer, "FB");
			if (TRX.VFO_B.Freq < 10000000)
				strcat(answer, "0");
			sprintf(ctmp, "%u", TRX.VFO_B.Freq);
			strcat(answer, ctmp); //freq
			strcat(answer, ";");
			CAT_Transmit(answer);
		}
		else
		{
			TRX_setFrequency((uint32_t)atoi(arguments), SecondaryVFO());
			LCD_UpdateQuery.FreqInfo = true;
			LCD_UpdateQuery.TopButtons = true;
		}
		return;
	}

	if (strcmp(command, "RA") == 0) // RF ATTENUATOR
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
				CAT_Transmit("RA00;");
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "PA") == 0) // PRE-AMP
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				if (TRX.ADC_Driver)
					CAT_Transmit("PA01;");
				else
					CAT_Transmit("PA00;");
			}
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "PS") == 0) // POWER-SWITCH
	{
		if (!has_args)
		{
			CAT_Transmit("PS1;");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				//power off
			}
			else if (strcmp(arguments, "1") == 0)
			{
				//power on
			}
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "GT") == 0) // AGC FUNCTION
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				if (TRX.RX_AGC_SSB_speed == 0 || !CurrentVFO()->AGC)
					CAT_Transmit("GT00;");
				else if (TRX.RX_AGC_SSB_speed == 1)
					CAT_Transmit("GT04;");
				else if (TRX.RX_AGC_SSB_speed == 2)
					CAT_Transmit("GT03;");
				else if (TRX.RX_AGC_SSB_speed == 3)
					CAT_Transmit("GT02;");
				else
					CAT_Transmit("GT01;");
			}
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "MD") == 0) // MODE
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				char answer[30] = {0};
				strcat(answer, "MD0");
				char mode[3] = {0};
				getFT450Mode((uint8_t)CurrentVFO()->Mode, mode);
				strcat(answer, mode); //mode
				strcat(answer, ";");
				CAT_Transmit(answer);
			}
			else
			{
				if (CurrentVFO()->Mode != setFT450Mode(arguments))
				{
					TRX_setMode(setFT450Mode(arguments), CurrentVFO());
					LCD_UpdateQuery.TopButtons = true;
				}
			}
		}
		return;
	}

	if (strcmp(command, "PC") == 0) // POWER CONTROL
	{
		if (!has_args)
		{
			char answer[30] = {0};
			strcat(answer, "PC");
			sprintf(ctmp, "%d", TRX.RF_Power);
			strcat(answer, ctmp);
			strcat(answer, ";");
			CAT_Transmit(answer);
		}
		else
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "SH") == 0) // WIDTH
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				CAT_Transmit("SH031;");
			}
		}
		return;
	}

	if (strcmp(command, "NB") == 0) // NOISE BLANKER
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				CAT_Transmit("NB00;");
			}
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "NR") == 0) // NOISE REDUCTION
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				CAT_Transmit("NR00;");
			}
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "VX") == 0) // VOX STATUS
	{
		if (!has_args)
		{
			CAT_Transmit("VX0;");
		}
		else
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "CT") == 0) // CTCSS
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				CAT_Transmit("CT00;");
			}
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "ML") == 0) // MONITOR LEVEL
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				CAT_Transmit("ML00;");
			}
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "BP") == 0) // MANUAL NOTCH
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "00") == 0)
			{
				CAT_Transmit("BP00000;");
			}
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "BI") == 0) // BREAK IN
	{
		if (!has_args)
		{
			CAT_Transmit("BI0;");
		}
		else
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "OS") == 0) // OFFSET
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				CAT_Transmit("OS00;");
			}
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}
	
	if (strcmp(command, "BS") == 0) // BAND SELECT
	{
		if (!has_args)
		{
			
		}
		else
		{
			int8_t band = -1;
			if (strcmp(arguments, "00") == 0)
				band = 1;
			else if (strcmp(arguments, "01") == 0)
				band = 2;
			else if (strcmp(arguments, "03") == 0)
				band = 4;
			else if (strcmp(arguments, "04") == 0)
				band = 5;
			else if (strcmp(arguments, "05") == 0)
				band = 6;
			else if (strcmp(arguments, "06") == 0)
				band = 7;
			else if (strcmp(arguments, "07") == 0)
				band = 8;
			else if (strcmp(arguments, "08") == 0)
				band = 9;
			else if (strcmp(arguments, "09") == 0)
				band = 10;
			else if (strcmp(arguments, "10") == 0)
				band = 11;
			else
				sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
			
			if(band > -1)
			{
				TRX_setFrequency(TRX.BANDS_SAVED_SETTINGS[band].Freq, CurrentVFO());
				TRX_setMode(TRX.BANDS_SAVED_SETTINGS[band].Mode, CurrentVFO());
				TRX.ATT = TRX.BANDS_SAVED_SETTINGS[band].ATT;
				TRX.ATT_DB = TRX.BANDS_SAVED_SETTINGS[band].ATT_DB;
				TRX.ADC_Driver = TRX.BANDS_SAVED_SETTINGS[band].ADC_Driver;
				TRX.IF_Gain = TRX.BANDS_SAVED_SETTINGS[band].IF_Gain;
				CurrentVFO()->AGC = TRX.BANDS_SAVED_SETTINGS[band].AGC;
				TRX_Temporary_Stop_BandMap = false;
				
				LCD_UpdateQuery.TopButtons = true;
				LCD_UpdateQuery.FreqInfoRedraw = true;
				LCD_UpdateQuery.StatusInfoBarRedraw = true;
				LCD_UpdateQuery.StatusInfoGUI = true;
			}
		}
		return;
	}

	if (strcmp(command, "NA") == 0) // NARROW
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
				CAT_Transmit("NA00;");
		}
		return;
	}

	if (strcmp(command, "SM") == 0) // READ S-METER
	{
		if (!has_args)
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
				CAT_Transmit("SM0100;");
		}
		return;
	}
	
	if (strcmp(command, "KP") == 0) // READ KEY PITCH
	{
		if (!has_args)
		{
			CAT_Transmit("KP04;");
		}
		else
		{
			sendToDebug_str3("Unknown CAT arguments: ", _command, "\r\n");
		}
		return;
	}

	if (strcmp(command, "TX") == 0) // TX SET
	{
		if (!has_args)
		{
			if (TRX_ptt_soft)
				CAT_Transmit("TX1;");
			else if (TRX_ptt_hard)
				CAT_Transmit("TX2;");
			else
				CAT_Transmit("TX0;");
		}
		else
		{
			if (strcmp(arguments, "0") == 0)
			{
				TRX_ptt_soft = false;
			}
			if (strcmp(arguments, "1") == 0)
			{
				TRX_ptt_soft = true;
			}
		}
		return;
	}

	sendToDebug_str3("Unknown CAT command: ", _command, "\r\n");
	//sendToDebug_str2(command,"|\r\n");
	//sendToDebug_str2(arguments,"|\r\n");
}

static void getFT450Mode(uint8_t VFO_Mode, char* out)
{
	if (VFO_Mode == TRX_MODE_LSB)
		strcpy(out, "1");
	if (VFO_Mode == TRX_MODE_USB)
		strcpy(out, "2");
	if (VFO_Mode == TRX_MODE_IQ)
		strcpy(out, "8");
	if (VFO_Mode == TRX_MODE_CW_L || VFO_Mode == TRX_MODE_CW_U)
		strcpy(out, "3");
	if (VFO_Mode == TRX_MODE_DIGI_L)
		strcpy(out, "8");
	if (VFO_Mode == TRX_MODE_DIGI_U)
		strcpy(out, "C");
	if (VFO_Mode == TRX_MODE_NFM)
		strcpy(out, "4");
	if (VFO_Mode == TRX_MODE_WFM)
		strcpy(out, "4");
	if (VFO_Mode == TRX_MODE_AM)
		strcpy(out, "5");
	if (VFO_Mode == TRX_MODE_LOOPBACK)
		strcpy(out, "8");
}

	static uint8_t setFT450Mode(char *FT450_Mode)
{
	if (strcmp(FT450_Mode, "01") == 0 || strcmp(FT450_Mode, "1") == 0)
		return TRX_MODE_LSB;
	if (strcmp(FT450_Mode, "02") == 0 || strcmp(FT450_Mode, "2") == 0)
		return TRX_MODE_USB;
	if (strcmp(FT450_Mode, "08") == 0 || strcmp(FT450_Mode, "8") == 0)
		return TRX_MODE_IQ;
	if (strcmp(FT450_Mode, "03") == 0 || strcmp(FT450_Mode, "3") == 0)
		return TRX_MODE_CW_L;
	if (strcmp(FT450_Mode, "06") == 0 || strcmp(FT450_Mode, "6") == 0)
		return TRX_MODE_DIGI_L;
	if (strcmp(FT450_Mode, "09") == 0 || strcmp(FT450_Mode, "9") == 0)
		return TRX_MODE_DIGI_U;
	if (strcmp(FT450_Mode, "0C") == 0 || strcmp(FT450_Mode, "C") == 0)
		return TRX_MODE_DIGI_U;
	if (strcmp(FT450_Mode, "04") == 0 || strcmp(FT450_Mode, "4") == 0)
		return TRX_MODE_NFM;
	if (strcmp(FT450_Mode, "05") == 0 || strcmp(FT450_Mode, "5") == 0)
		return TRX_MODE_AM;
	sendToDebug_str3("Unknown mode ", FT450_Mode, "\r\n");
	return TRX_MODE_USB;
}