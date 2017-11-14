/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： oled.c
＊　責務　　　：
＊　作成日　　： 2017/10/7
＊　作成者　　：　kern-gt

＊　コンパイラ　：CC-RX

＊　備考　　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/

/**----------------------------------------------------------------------------
<<利用ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include <stdint.h>
/* 数学ライブラリ */
#include <mathf.h>

/* デバイス */
#include "r_riic_rx_if.h"
#include "ssd1306.h"

/* データ定義 */
#include "draw_data.h"

/* GUIアプリケーション */
#include "disp_main.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
//#include "queue.h"

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "oled_in.h"

/**----------------------------------------------------------------------------
<<変数>>
-----------------------------------------------------------------------------**/
#define OLED_ADDR (SSD1306_ADDRESS)	//I2CAddress
extern xTaskHandle oled_tsk_id;

static riic_info_t iic_info_0;  //i2c通信構造体

//グラフィックRAM構造体
static OledScreenBuf gram;

/***公開関数*******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　： DrawTask
＊　機能　　： 描画タスク
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
#if 0
void DrawTask(void){

	oled_tsk_id = xTaskGetCurrentTaskHandle();
	InitOled();
	RefreshDisplay();
	
	while(1){
		vTaskDelay((1000/DISP_FLAME_RATE_HZ)/portTICK_PERIOD_MS);
		if(DisplayApli())
		{
			RefreshDisplay();
		}
	}
}
#endif

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void DisplayOn(void){
	volatile riic_return_t ret;
	uint8_t oled_address[1] = {OLED_ADDR};  //スレーブアドレス
	uint8_t control_byte[1] = {0};    //control byte, Co bit = 0 (continue), D/C# = 0 (command)
	uint8_t oled_command[1] = {0};

	//通信構造体を設定
	iic_info_0.cnt1st = sizeof(control_byte);
	iic_info_0.cnt2nd = sizeof(oled_command);
	iic_info_0.p_data1st = control_byte;
	iic_info_0.p_data2nd = oled_command;
	iic_info_0.p_slv_adr = oled_address;

	control_byte[0] = kCtrlCommands;
	oled_command[0] = kCommandDispOn;
	
    ret = R_RIIC_MasterSend(&iic_info_0);
    vTaskSuspend(oled_tsk_id);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void DisplayOff(void){
	volatile riic_return_t ret;
	uint8_t oled_address[1] = {OLED_ADDR};  //スレーブアドレス
	uint8_t control_byte[1] = {0};    //control byte, Co bit = 0 (continue), D/C# = 0 (command)
	uint8_t oled_command[1] = {0};

	//通信構造体を設定
	iic_info_0.cnt1st = sizeof(control_byte);
	iic_info_0.cnt2nd = sizeof(oled_command);
	iic_info_0.p_data1st = control_byte;
	iic_info_0.p_data2nd = oled_command;
	iic_info_0.p_slv_adr = oled_address;

	control_byte[0] = kCtrlCommands;
	oled_command[0] = kCommandDispOff;

    ret = R_RIIC_MasterSend(&iic_info_0);
    vTaskSuspend(oled_tsk_id);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void Bitblt(void){

	SendImageData();
	SendCommand();
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void DrawClear(void){
	int i, j;

	for (i = 0; i < (OLED_PAGE_NUM); ++i)
	{
		for (j = 0; j < (OLED_ROW_SIZE); ++j)
		{
			gram.disp_buf[i][j] = 0x00;
		}
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void DrawWhite(void){
	int i, j;

	for (i = 0; i < (OLED_PAGE_NUM); ++i)
	{
		for (j = 0; j < (OLED_ROW_SIZE); ++j)
		{
			gram.disp_buf[i][j] = 0xff;
		}
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void DrawDot(uint16_t x, uint16_t y, uint8_t color){
	uint8_t page_num, y_offset;

	if(x < (OLED_ROW_SIZE) && y < (OLED_COLUMN_SIZE))
	{
		page_num = y / OLED_PAGE_NUM;
		y_offset = y - (page_num * 8);

		if(color)
		{
			gram.disp_buf[page_num][x] = gram.disp_buf[page_num][x] | (0x01 << y_offset);
		}else{
			gram.disp_buf[page_num][x] = gram.disp_buf[page_num][x] & ~(0x01 << y_offset);
		}
		
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void DrawLine(uint16_t x_1, uint16_t y_1, uint16_t x_2, uint16_t y_2, uint8_t color){

	if(x_1 < (OLED_ROW_SIZE) && x_2 < (OLED_ROW_SIZE))
	{
		if(y_1 < (OLED_COLUMN_SIZE) && y_2 < (OLED_COLUMN_SIZE))
		{
			if((x_1 != x_2) && (y_1 != y_2))
			{
				SlopeLine(x_1, y_1, x_2, y_2, color);
			}else{
				CrossLine(x_1, y_1, x_2, y_2, color);
			}
		}
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void DrawRectangle(uint16_t x_rup, uint16_t y_rup, uint16_t x_ldwn, uint16_t y_ldwn, uint8_t color, uint8_t fill){
	int16_t i;

	if(x_rup < (OLED_ROW_SIZE - 1) && x_ldwn < (OLED_ROW_SIZE))
	{
		if(y_rup < (OLED_COLUMN_SIZE -1) && y_ldwn < (OLED_COLUMN_SIZE))
		{
			if((x_rup != x_ldwn) && (y_rup != y_ldwn))
			{
				if(fill == FALSE)
				{
					//上側
					CrossLine(x_rup, y_rup, x_ldwn, y_rup, color);
					//下側
					CrossLine(x_rup, y_ldwn, x_ldwn, y_ldwn, color);
					//右側
					CrossLine(x_ldwn, y_rup, x_ldwn, y_ldwn, color);
					//左側
					CrossLine(x_rup, y_rup, x_rup, y_ldwn, color);
				}else{
					for(i = y_rup; i < y_ldwn; ++i)
					{
						CrossLine(x_rup, i, x_ldwn, i, color);
					}
				}
			}
		}
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void DrawCircle(int16_t x, int16_t y, uint16_t r, uint8_t line_color, uint8_t fill){
	uint16_t x_rup, y_rup, x_ldwn, y_ldwn, i, j;

	if(r != 0)
	{
		x_rup  = ((x - r) < 0)? 0: x - r;
		x_ldwn = (OLED_ROW_SIZE < (x + r))? OLED_ROW_SIZE: x + r;
		y_rup  = ((y - r) < 0)? 0: y - r;
		y_ldwn = (OLED_COLUMN_SIZE < (y + r))? OLED_COLUMN_SIZE: y + r;

		if(fill == FALSE)
		{
			for(i = y_rup; i <= y_ldwn; ++i)
			{
				for(j = x_rup; j <= x_ldwn; ++j)
				{
					if(((int16_t)r - (int16_t)sqrtf((j-x)*(j-x) + (i-y)*(i-y))) == 0)
					{
						DrawDot(j, i, line_color);
					}
				}
			}
		}else{
			for(i = y_rup; i <= y_ldwn; ++i)
			{
				for(j = x_rup; j <= x_ldwn; ++j)
				{
					if(((int16_t)r - (int16_t)sqrtf((j-x)*(j-x) + (i-y)*(i-y))) >= 0)
					{
						DrawDot(j, i, line_color);
					}
				}
			}
		}
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void PasteImage1616(uint16_t x, uint16_t page_num, const uint8_t *data){
	uint16_t i;

	if(x < (OLED_ROW_SIZE) && page_num < (OLED_PAGE_NUM -1))
	{
		for(i = 0; i < 16; ++i)
		{
			if((x+i) > (OLED_ROW_SIZE-1))	break;
			gram.disp_buf[page_num][x+i] |= data[i];
		}

		for(i = 0; i < 16; ++i)
		{
			if((x+i) > (OLED_ROW_SIZE-1)) break;
			gram.disp_buf[page_num+1][x+i] |= data[16+i];
		}
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void PrintNumber(uint16_t x, uint8_t page_num, uint8_t num){
	
	if(num < 10)
	{
		PasteImage1616(x,page_num,&number_font[num][0]);
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void InitOled(void){
	volatile riic_return_t ret;
	uint8_t oled_address[1] = {OLED_ADDR};  //スレーブアドレス
	/* SSD1306 I2C初期設定データ */
	uint8_t control_byte[1] = {0};    //control byte, Co bit = 0 (continue), D/C# = 0 (command)
	const uint8_t setup_data[] = {0xAE, //display off
								0xA8, //Set Multiplex Ratio  0xA8, 0x3F
								0x3F, //64MUX
								0xD3, //Set Display Offset 0xD3, 0x00
								0x00,
								0x40, //Set Display Start Line 0x40
								0xA0, //Set Segment re-map 0xA0/0xA1
								0xC0, //Set COM Output Scan Direction 0xC0,0xC8
								0xDA, //Set COM Pins hardware configuration 0xDA, 0x02
								0x12,
								0x81, //Set Contrast Control 0x81, 0x7F（明るさ設定）
								0xff, //0-255
								0xA4, //Disable Entire Display On（ディスプレイ全体ＯＮ）
								0xA6, //Set Normal Display 0xA6, Inverse display 0xA7
								0xD5, //Set Display Clock Divide Ratio/Oscillator Frequency 0xD5, 0x80
								0x80,
								0x2E, //Deactivate scrollスクロール表示解除
								0x20, //Set Memory Addressing Mode
								0x10, //Page addressing mode
								0x21, //set Column Address
								0,    //Column Start Address←水平開始位置はここで決める(0～127)
								127,  //Column Stop Address　画面をフルに使う
								0x22, //Set Page Address
								0,    //垂直開始位置（ページ）
								7,    //垂直終了位置（ページ）
								0x8D, //Set Enable charge pump regulator 0x8D, 0x14
								0x14,
								0xAF};//Display On 0xAF

	/*I2C初期化*******************************************/
	iic_info_0.dev_sts = RIIC_NO_INIT;
	iic_info_0.ch_no   = 0;
	ret = R_RIIC_Open(&iic_info_0);

	/*OLED(SSD1306)を初期化********************************/
	//通信構造体を設定
	iic_info_0.callbackfunc = &OledSendCallBack;
	iic_info_0.cnt1st = sizeof(control_byte);
	iic_info_0.cnt2nd = sizeof(setup_data);
	iic_info_0.p_data1st = control_byte;
	iic_info_0.p_data2nd = (uint8_t*)setup_data;
	iic_info_0.p_slv_adr = oled_address;
	//送信
	ret = R_RIIC_MasterSend(&iic_info_0);
	vTaskSuspend(oled_tsk_id);

	DrawClear();
}
/***非公開関数******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：SendImageData
＊　機能　　： ディスプレイを更新する。
          OLEDにグラフィックRAMのデータを転送する。
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void SendImageData(void){
	volatile riic_return_t ret;
	uint8_t oled_address[1] = {OLED_ADDR};  //スレーブアドレス
	uint8_t control_byte[1] = {0};                  //control byte, Co bit = 0 (continue), D/C# = 0 (command)
	uint8_t draw_set[] = {(kCommandSetPageNum | 0), //Set PageNumber 0
						kCommandSetColumnAddress,   //Set Column Address
						0x00,                       //Column Start Address
						0x7F};                      //Column Stop  Address


	//スクリーンバッファポインタセット
	iic_info_0.cnt1st = sizeof(control_byte);
	iic_info_0.cnt2nd = sizeof(draw_set);
	iic_info_0.p_data1st = control_byte;
	iic_info_0.p_data2nd = draw_set;
	iic_info_0.p_slv_adr = oled_address;

	control_byte[0] = kCtrlCommands;

	ret = R_RIIC_MasterSend(&iic_info_0);
	vTaskSuspend(oled_tsk_id);


	//描画データ送信
	iic_info_0.cnt1st = sizeof(control_byte);
	iic_info_0.cnt2nd = sizeof(gram.buf_byte);
	iic_info_0.p_data1st = control_byte;
	iic_info_0.p_data2nd = gram.buf_byte;
	iic_info_0.p_slv_adr = oled_address;

	control_byte[0] = kCtrlData;

	ret = R_RIIC_MasterSend(&iic_info_0);
	vTaskSuspend(oled_tsk_id);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void SendCommand(void){

}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void CrossLine(uint16_t x_l, uint16_t y_l, uint16_t x_h, uint16_t y_h, uint8_t color){
	uint16_t i;

	if(x_l == x_h && y_l == y_h)
	{
		DrawDot(x_l, y_l, color);
	}else if(x_l == x_h)
	{
		if(y_h > y_l)
		{
			for(i = y_l; i <= y_h; ++i)
			{
				DrawDot(x_l, i, color);
			}
		}else
		{
			for(i = y_l; i >= y_h; --i)
			{
				DrawDot(x_l, i, color);
			}
		}
	}else if(y_l == y_h)
	{
		if(x_h > x_l)
		{
			for(i = x_l; i <= x_h; ++i)
			{
				DrawDot(i, y_l, color);
			}
		}else
		{
			for(i = x_l; i >= x_h; --i)
			{
				DrawDot(i, y_l, color);
			}
		}
	}else{
		while(1);
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void SlopeLine(uint16_t x_l, uint16_t y_l, uint16_t x_h, uint16_t y_h, uint8_t color){
	uint16_t i ,x, y;
	float slope_gain;

	slope_gain = ((float)(y_h - y_l))/((float)(x_h - x_l));

	if(slope_gain > 0.0f)
	{
		if(slope_gain > 1.0f)
		{
			slope_gain = ((float)(x_h - x_l))/((float)(y_h - y_l));
			if(y_h > y_l)
			{
				for (i = y_l; i <= y_h; ++i)
				{
					x = (uint16_t)(slope_gain * (float)(i - y_l) + (float)x_l);
					DrawDot(x, i, color);
				}
			}else{
				for (i = y_l; i >= y_h; --i)
				{
					x = (uint16_t)(slope_gain * (float)(i - y_l) + (float)x_l);
					DrawDot(x, i, color);
				}
			}
		}else{
			if(x_h > x_l)
			{
				for(i = x_l; i <= x_h; ++i)
				{
					y = (uint16_t)(slope_gain * (float)(i - x_l) + (float)y_l);
					DrawDot(i, y, color);
				}
			}else{
				for(i = x_l; i >= x_h; --i)
				{
					y = (uint16_t)(slope_gain * (float)(i - x_l) + (float)y_l);
					DrawDot(i, y, color);
				}
			}
		}
	}else if(slope_gain < 0.0f)
	{
		if (-1.0f <= slope_gain)
		{
			if(x_h > x_l)
			{
				for(i = x_l; i <= x_h; ++i)
				{
					y = (uint16_t)(slope_gain * (float)(i - x_l) + (float)y_l);
					DrawDot(i, y, color);
				}
			}else{
				for(i = x_l; i >= x_h; --i)
				{
					y = (uint16_t)(slope_gain * (float)(i - x_l) + (float)y_l);
					DrawDot(i, y, color);
				}
			}
		}else{
			slope_gain = ((float)(x_h - x_l))/((float)(y_h - y_l));
			if(y_h > y_l)
			{
				for (i = y_l; i <= y_h; ++i)
				{
					x = (uint16_t)(slope_gain * (float)(i - y_l) + (float)x_l);
					DrawDot(x, i, color);
				}
			}else{
				for (i = y_l; i >= y_h; --i)
				{
					x = (uint16_t)(slope_gain * (float)(i - y_l) + (float)x_l);
					DrawDot(x, i, color);
				}
			}
		}
	}else{
		while(1);
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：OledDataSendFinish
＊　機能　　： i2c送信終了コールバックハンドラ
          ここでは送信ビジーフラグをクリアする。
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void OledSendCallBack(void){
	//xTaskResumeFromISR(oled_tsk_id);
	vTaskResume(oled_tsk_id);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/