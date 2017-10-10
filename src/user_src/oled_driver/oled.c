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
#include "Config_RIIC0.h"
#include "draw_data.h"

/* Kernel includes. */
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "oled_in.h"

/**----------------------------------------------------------------------------
<<変数>>
-----------------------------------------------------------------------------**/
#define OLED_ADDR (0x3C)	//I2CAddress
volatile int send_busy = FALSE; //I2C送信中フラグ

/* SSD1306 I2C初期設定データ */
static uint8_t setup_data[] = {0,     //control byte, Co bit = 0 (continue), D/C# = 0 (command)
								0xAE, //display off
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
//1バイトコマンド
static uint8_t disp_bytecommand[2] = {kCtrlCommands, kCommandDispOn};
//描画RAMカーソル設定
static uint8_t draw_set[] = {0x00, 0xb0, 0x21, 0x00, 0x7f};
//グラフィックRAM構造体
static OledScreenBuf gram;

//i2c送信API用配列ポインタ
static uint8_t * const disp_bytecommand_p = disp_bytecommand;
static uint8_t * const setup_data_p = setup_data;
static uint8_t * const draw_set_p = draw_set;
static uint8_t * const gram_p = gram.buf_byte;

//static uint8_t * const draw_black_p = draw_black;
//static uint8_t * const draw_blue_p  = draw_blue;

//uint8_t * const anime_0_p = anime_0;
//uint8_t * const anime_1_p = anime_1;
//uint8_t * const anime_2_p = anime_2;
//uint8_t * const anime_3_p = anime_3;
/***公開関数*******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void DrawTask(void){

	//vTaskDelay(200/portTICK_PERIOD_MS);
	InitOled();
	DrawClear();
	
	while(1){

		DrawWhite();
		RefreshDisplay();
		vTaskDelay(500/portTICK_PERIOD_MS);

		DrawClear();
		RefreshDisplay();
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void DisplayOn(void){

	disp_bytecommand[0] = kCtrlCommands;
	disp_bytecommand[1] = kCommandDispOn;

	send_busy = TRUE;
    R_Config_RIIC0_Master_Send(OLED_ADDR, disp_bytecommand_p, (uint16_t)sizeof(disp_bytecommand)); //control byte, Co bit = 0 (continue), D/C# = 1 (data)
    while(send_busy == TRUE);
    R_Config_RIIC0_IIC_StopCondition();
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void DisplayOff(void){

	disp_bytecommand[0] = kCtrlCommands;
	disp_bytecommand[1] = kCommandDispOff;

	send_busy = TRUE;
    R_Config_RIIC0_Master_Send(OLED_ADDR, disp_bytecommand_p, (uint16_t)sizeof(disp_bytecommand)); //control byte, Co bit = 0 (continue), D/C# = 1 (data)
    while(send_busy == TRUE);
    R_Config_RIIC0_IIC_StopCondition();
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void I2cSendBufEnpty(void){
	send_busy = FALSE;
}


/***非公開関数******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void InitOled(void){
	MD_STATUS id = MD_OK;

	R_Config_RIIC0_Start();

	//OLED(SSD1306)を初期化
	send_busy = TRUE;	//送信終了時にFalseになる。
	do{
		id = R_Config_RIIC0_Master_Send(OLED_ADDR, setup_data_p, (uint16_t)sizeof(setup_data));
	}while(id != MD_OK);
	while(send_busy == TRUE);	//送信終了したら、ストップコンディション発行
	R_Config_RIIC0_IIC_StopCondition();

	//グラフィックRAM構造体初期化
	gram.Elements.control_byte = kCtrlData;
	DrawClear();
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void RefreshDisplay(void){

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
static void SendImageData(void){
	MD_STATUS id;

	//グラフィックRAMのカーソル位置を画面左上にセット
	send_busy = TRUE;	//送信終了時にFalseになる。
	draw_set[1] = kCtrlCommands;
	do{
		id = R_Config_RIIC0_Master_Send(OLED_ADDR, draw_set_p, (uint16_t)sizeof(draw_set));
	}while(id != MD_OK);

	while(send_busy == TRUE);	//送信終了したら、ストップコンディション発行
	R_Config_RIIC0_IIC_StopCondition();

	//描画データ送信
	send_busy = TRUE;
	do{
		id = R_Config_RIIC0_Master_Send(OLED_ADDR, gram_p, (uint16_t)(sizeof(gram.buf_byte) - SCREENBUF_PADDING));
	}while(id != MD_OK);

	while(send_busy == TRUE);
	R_Config_RIIC0_IIC_StopCondition();
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
static void DrawClear(void){
	int i, j;

	for (i = 0; i < (OLED_PAGE_NUM); ++i)
	{
		for (j = 0; j < (OLED_ROW_SIZE); ++j)
		{
			gram.Elements.disp_buf[i][j] = 0x00;
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
static void DrawWhite(void){
	int i, j;

	for (i = 0; i < (OLED_PAGE_NUM); ++i)
	{
		for (j = 0; j < (OLED_ROW_SIZE); ++j)
		{
			gram.Elements.disp_buf[i][j] = 0xff;
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