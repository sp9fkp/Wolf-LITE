#ifndef LCD_Layout_h
#define LCD_Layout_h

#include "lcd_driver.h"
#include "color_themes.h"
#include "fonts.h"
	
//Top row of status buttons
#define LAY_TOPBUTTONS_X1 0
#define LAY_TOPBUTTONS_X2 (LCD_WIDTH - 1)
#define LAY_TOPBUTTONS_Y1 1
#define LAY_TOPBUTTONS_Y2 50
#define LAY_TOPBUTTONS_WIDTH 62
#define LAY_TOPBUTTONS_HEIGHT 40
#define LAY_TOPBUTTONS_TB_MARGIN 2
#define LAY_TOPBUTTONS_LR_MARGIN 2
#define LAY_TOPBUTTONS_PRE_X (uint16_t)(LAY_TOPBUTTONS_X1 + LAY_TOPBUTTONS_LR_MARGIN)
#define LAY_TOPBUTTONS_PRE_Y LAY_TOPBUTTONS_Y1
#define LAY_TOPBUTTONS_ATT_X (uint16_t)(LAY_TOPBUTTONS_PRE_X + LAY_TOPBUTTONS_WIDTH + LAY_TOPBUTTONS_LR_MARGIN)
#define LAY_TOPBUTTONS_ATT_Y LAY_TOPBUTTONS_Y1
#define LAY_TOPBUTTONS_AGC_X (uint16_t)(LAY_TOPBUTTONS_ATT_X + LAY_TOPBUTTONS_WIDTH + LAY_TOPBUTTONS_LR_MARGIN)
#define LAY_TOPBUTTONS_AGC_Y LAY_TOPBUTTONS_Y1
#define LAY_TOPBUTTONS_FAST_X (uint16_t)(LAY_TOPBUTTONS_AGC_X + LAY_TOPBUTTONS_WIDTH + LAY_TOPBUTTONS_LR_MARGIN)
#define LAY_TOPBUTTONS_FAST_Y LAY_TOPBUTTONS_Y1
#define LAY_TOPBUTTONS_MUTE_X (uint16_t)(LAY_TOPBUTTONS_FAST_X + LAY_TOPBUTTONS_WIDTH + LAY_TOPBUTTONS_LR_MARGIN)
#define LAY_TOPBUTTONS_MUTE_Y LAY_TOPBUTTONS_Y1
#define LAY_TOPBUTTONS_LOCK_X (uint16_t)(LAY_TOPBUTTONS_MUTE_X + LAY_TOPBUTTONS_WIDTH + LAY_TOPBUTTONS_LR_MARGIN)
#define LAY_TOPBUTTONS_LOCK_Y LAY_TOPBUTTONS_Y1
//Clock
#define LAY_CLOCK_POS_Y 17
#define LAY_CLOCK_POS_HRS_X (LCD_WIDTH - 75)
#define LAY_CLOCK_POS_MIN_X (uint16_t)(LAY_CLOCK_POS_HRS_X + 25)
#define LAY_CLOCK_POS_SEC_X (uint16_t)(LAY_CLOCK_POS_MIN_X + 25)
#define LAY_CLOCK_FONT &FreeSans9pt7b
//Frequency output
#define LAY_FREQ_X_OFFSET_100 37
#define LAY_FREQ_X_OFFSET_10 73
#define LAY_FREQ_X_OFFSET_1 113
#define LAY_FREQ_X_OFFSET_KHZ 170
#define LAY_FREQ_X_OFFSET_HZ 307
#define LAY_FREQ_HEIGHT 51
#define LAY_FREQ_WIDTH 370
#define LAY_FREQ_TOP_OFFSET 15
#define LAY_FREQ_LEFT_MARGIN 37
#define LAY_FREQ_RIGHT_MARGIN (uint16_t)(LCD_WIDTH - LAY_FREQ_LEFT_MARGIN - LAY_FREQ_WIDTH)
#define LAY_FREQ_BOTTOM_OFFSET 8
#define LAY_FREQ_BLOCK_HEIGHT (uint16_t)(LAY_FREQ_HEIGHT + LAY_FREQ_TOP_OFFSET + LAY_FREQ_BOTTOM_OFFSET)
#define LAY_FREQ_Y_TOP LAY_TOPBUTTONS_Y2
#define LAY_FREQ_Y_BASELINE (uint16_t)(LAY_TOPBUTTONS_Y2 + LAY_FREQ_HEIGHT + LAY_FREQ_TOP_OFFSET)
#define LAY_FREQ_Y_BASELINE_SMALL (uint16_t)(LAY_FREQ_Y_BASELINE - 2)
#define LAY_FREQ_FONT &FreeSans36pt7b
#define LAY_FREQ_SMALL_FONT &Quito32pt7b
#define LAY_FREQ_DELIMITER_Y_OFFSET 0
#define LAY_FREQ_DELIMITER_X1_OFFSET 151
#define LAY_FREQ_DELIMITER_X2_OFFSET 285
//Display statuses under frequency
#define LAY_STATUS_Y_OFFSET (uint16_t)(LAY_FREQ_Y_TOP + LAY_FREQ_BLOCK_HEIGHT + 1)
#define LAY_STATUS_HEIGHT 44
#define LAY_STATUS_BAR_X_OFFSET 60
#define LAY_STATUS_BAR_Y_OFFSET 16
#define LAY_STATUS_BAR_HEIGHT 10
#define LAY_STATUS_TXRX_X_OFFSET 3
#define LAY_STATUS_TXRX_Y_OFFSET -50
#define LAY_STATUS_TXRX_FONT &FreeSans9pt7b
#define LAY_STATUS_VFO_X_OFFSET 0
#define LAY_STATUS_VFO_Y_OFFSET -43
#define LAY_STATUS_VFO_BLOCK_WIDTH 37
#define LAY_STATUS_VFO_BLOCK_HEIGHT 22
#define LAY_STATUS_ANT_X_OFFSET 0
#define LAY_STATUS_ANT_Y_OFFSET -23
#define LAY_STATUS_ANT_BLOCK_WIDTH 37
#define LAY_STATUS_ANT_BLOCK_HEIGHT 22
#define LAY_STATUS_TX_LABELS_OFFSET_X 5
#define LAY_STATUS_TX_LABELS_MARGIN_X 55
#define LAY_STATUS_SMETER_WIDTH 400
#define LAY_STATUS_SMETER_MARKER_HEIGHT 30
#define LAY_STATUS_PMETER_WIDTH 300
#define LAY_STATUS_AMETER_WIDTH 90
#define LAY_STATUS_ALC_BAR_X_OFFSET 10
#define LAY_STATUS_LABELS_OFFSET_Y 0
#define LAY_STATUS_LABELS_FONT_SIZE 1
#define LAY_STATUS_LABEL_S_VAL_X_OFFSET 12
#define LAY_STATUS_LABEL_S_VAL_Y_OFFSET 25
#define LAY_STATUS_LABEL_S_VAL_FONT &FreeSans7pt7b
#define LAY_STATUS_LABEL_DBM_X_OFFSET 5
#define LAY_STATUS_LABEL_DBM_Y_OFFSET 36
#define LAY_STATUS_LABEL_BW_X_OFFSET 400
#define LAY_STATUS_LABEL_BW_Y_OFFSET -70
#define LAY_STATUS_LABEL_RIT_X_OFFSET 140
#define LAY_STATUS_LABEL_RIT_Y_OFFSET 36
#define LAY_STATUS_LABEL_VLT_X_OFFSET 200
#define LAY_STATUS_LABEL_VLT_Y_OFFSET 36
#define LAY_STATUS_LABEL_CPU_X_OFFSET 280
#define LAY_STATUS_LABEL_CPU_Y_OFFSET 36
#define LAY_STATUS_LABEL_NOTCH_X_OFFSET 335
#define LAY_STATUS_LABEL_NOTCH_Y_OFFSET 36
#define LAY_STATUS_LABEL_FFT_BW_X_OFFSET 420
#define LAY_STATUS_LABEL_FFT_BW_Y_OFFSET 36
#define LAY_STATUS_SMETER_PEAK_HOLDTIME 1000
#define LAY_STATUS_SMETER_TXLABELS_MARGIN 55
#define LAY_STATUS_SMETER_TXLABELS_PADDING 23
#define LAY_STATUS_TX_LABELS_VAL_WIDTH 25
#define LAY_STATUS_TX_LABELS_VAL_HEIGHT 8
#define LAY_STATUS_TX_ALC_X_OFFSET 40
#define LAY_STATUS_MODE_X_OFFSET (uint16_t)(LCD_WIDTH - LAY_FREQ_RIGHT_MARGIN + 10)
#define LAY_STATUS_MODE_Y_OFFSET -42
#define LAY_STATUS_MODE_BLOCK_WIDTH 48
#define LAY_STATUS_MODE_BLOCK_HEIGHT 22
#define LAY_STATUS_ERR_OFFSET_X 435
#define LAY_STATUS_ERR_OFFSET_Y 65
#define LAY_STATUS_ERR_WIDTH 20
#define LAY_STATUS_ERR_HEIGHT 8
#define LAY_TEXTBAR_FONT 2
#define LAY_TEXTBAR_TEXT_T_BOTTOM_OFFESET 30
#define LAY_TEXTBAR_TEXT_X_OFFSET 85
//FFT and waterfall
#define LAY_FFT_HEIGHT 50
#define LAY_WTF_HEIGHT 55
#define LAY_FFT_PRINT_SIZE LCD_WIDTH
#define LAY_FFT_CWDECODER_OFFSET 17
#define LAY_FFT_FFTWTF_POS_Y (uint16_t)(LCD_HEIGHT - LAY_FFT_HEIGHT - LAY_WTF_HEIGHT - LAY_TEXTBAR_TEXT_T_BOTTOM_OFFESET) 
#define LAY_FFT_FFTWTF_HEIGHT (uint16_t)(LAY_FFT_FFTWTF_POS_Y + LAY_FFT_HEIGHT + LAY_WTF_HEIGHT)
//System menu
#define LAY_SYSMENU_X1 5
#define LAY_SYSMENU_X2 400
#define LAY_SYSMENU_X2_BIGINT 350
#define LAY_SYSMENU_X2R_BIGINT 400
#define LAY_SYSMENU_W 458
#define LAY_SYSMENU_ITEM_HEIGHT 18
#define LAY_SYSMENU_MAX_ITEMS_ON_PAGE (uint16_t)(LCD_HEIGHT / LAY_SYSMENU_ITEM_HEIGHT)
//Stuff
#define LAY_GREETINGS_X (LCD_WIDTH / 2 - 5)
#define LAY_GREETINGS_Y 25
//Tooltip
#define LAY_TOOLTIP_TIMEOUT 1000
#define LAY_TOOLTIP_MARGIN 5
#define LAY_TOOLTIP_POS_X (LCD_WIDTH / 2)
#define LAY_TOOLTIP_POS_Y 70
//bottom buttons
#define LAY_BOTTOMBUTTONS_X 0
#define LAY_BOTTOMBUTTONS_Y (LAY_FFT_FFTWTF_HEIGHT + 1)
#define LAY_BOTTOMBUTTONS_MARGIN 1
#define LAY_BOTTOMBUTTONS_WIDTH (LCD_WIDTH / 4 - LAY_BOTTOMBUTTONS_MARGIN * 2)
#define LAY_BOTTOMBUTTONS_HEIGHT LAY_TEXTBAR_TEXT_T_BOTTOM_OFFESET
#define LAY_BOTTOMBUTTONS_X1 LAY_BOTTOMBUTTONS_X
#define LAY_BOTTOMBUTTONS_X2 (LAY_BOTTOMBUTTONS_X1 + LAY_BOTTOMBUTTONS_WIDTH + LAY_BOTTOMBUTTONS_MARGIN * 2)
#define LAY_BOTTOMBUTTONS_X3 (LAY_BOTTOMBUTTONS_X2 + LAY_BOTTOMBUTTONS_WIDTH + LAY_BOTTOMBUTTONS_MARGIN * 2)
#define LAY_BOTTOMBUTTONS_X4 (LAY_BOTTOMBUTTONS_X3 + LAY_BOTTOMBUTTONS_WIDTH + LAY_BOTTOMBUTTONS_MARGIN * 2)
//BW Trapezoid
#define LAY_BW_TRAPEZ_POS_X 385
#define LAY_BW_TRAPEZ_POS_Y 25
#define LAY_BW_TRAPEZ_HEIGHT 25
#define LAY_BW_TRAPEZ_WIDTH 90

#endif
