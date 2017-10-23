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
#define OLED_PAGE_NUM    (OLED_COLUMN_SIZE/8) //8[bit/page]

/**----------------------------------------------------------------------------
<<非公開型定義>>
-----------------------------------------------------------------------------**/
//グラフィックRAM構造体
typedef union{
	uint8_t buf_byte[(OLED_PAGE_NUM * OLED_ROW_SIZE)];
	uint8_t disp_buf[OLED_PAGE_NUM][OLED_ROW_SIZE];
}OledScreenBuf;

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
static void SendImageData(void);
static void SendCommand(void);
static void OledSendCallBack(void);

static void CrossLine(uint16_t x_l, uint16_t y_l, uint16_t x_h, uint16_t y_h, uint8_t color);
static void SlopeLine(uint16_t x_l, uint16_t y_l, uint16_t x_h, uint16_t y_h, uint8_t color);


#endif /*_OLED_IN_H_*/