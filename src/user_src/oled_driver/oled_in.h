/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： oled_in.h
＊　責務　　　：
＊　作成日　　： 2017/10/7
＊　作成者　　：　kern-gt

＊　備考　　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
#ifndef _OLED_IN_H_
#define _OLED_IN_H_

#include "oled.h"

/**----------------------------------------------------------------------------
<<非公開マクロ定義>>
-----------------------------------------------------------------------------**/
#define OLED_ROW_SIZE    (128U)
#define OLED_COLUMN_SIZE (64U)
#define OLED_PAGE_NUM    (8U)
#define SSD1306_CTRLBYTE_SIZE (1U)

#define SCREENBUF_PADDING (sizeof(unsigned int) - SSD1306_CTRLBYTE_SIZE)

/**----------------------------------------------------------------------------
<<非公開型定義>>
-----------------------------------------------------------------------------**/
//グラフィックRAM構造体
typedef union{
	uint8_t  buf_byte[SSD1306_CTRLBYTE_SIZE + (OLED_PAGE_NUM * OLED_ROW_SIZE) + SCREENBUF_PADDING];
	struct
	{
		uint8_t control_byte;
		uint8_t disp_buf[OLED_PAGE_NUM][OLED_ROW_SIZE];
		uint8_t dummy[SCREENBUF_PADDING]; //control_byteが1バイトなのでアライメントをとる
	}Elements;
}OledScreenBuf;

//SSD1306用コントロールバイト定義
typedef enum{
	kCtrlCommands = 0x00,
	kCtrlData     = 0x40
}OledControlByteEnum;

//SSD1306用コマンド定義
typedef enum{
	kCommandDispOff = 0xAE,
	kCommandDispOn  = 0xAF
}OledCommandTypeEnum;

//エラー定義
typedef enum{
	kOk = 0,
	kErrorAddress,
	kErrorUnknown
}OledErrorEnum;

/**----------------------------------------------------------------------------
<<非公開プロトタイプ定義>>
-----------------------------------------------------------------------------**/
/*static */
static void InitOled(void);
static void SendImageData(void);
static void DrawClear(void);
static void DrawWhite(void);
static void RefreshDisplay(void);
static void SendCommand(void);

#endif /*_OLED_IN_H_*/