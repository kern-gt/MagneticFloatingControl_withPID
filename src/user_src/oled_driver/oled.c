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

/* デバイス */
#include "r_riic_rx_if.h"
#include "ssd1306.h"

/* データ定義 */
#include "draw_data.h"

/* Kernel includes. */
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/queue.h"

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "oled_in.h"

/**----------------------------------------------------------------------------
<<変数>>
-----------------------------------------------------------------------------**/
#define OLED_ADDR (SSD1306_ADDRESS)	//I2CAddress(コード生成APIはビットシフトが必要)
static volatile int send_busy = FALSE; //I2C送信中フラグ

static riic_info_t iic_info_0;  //i2c通信構造体

//1バイトコマンド
//static uint8_t disp_bytecommand[2] = {kCtrlCommands, kCommandDispOn};

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
void DrawTask(void){

	//vTaskDelay(200/portTICK_PERIOD_MS);
	InitOled();
	//DrawWhite();
	//RefreshDisplay();
	
	while(1){

		DrawWhite();
		RefreshDisplay();
		//DisplayOn();
		vTaskDelay(500/portTICK_PERIOD_MS);

		DrawClear();
		RefreshDisplay();
		//DisplayOff();
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
	
	send_busy = TRUE;
    ret = R_RIIC_MasterSend(&iic_info_0);
    while(send_busy == TRUE);
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

	send_busy = TRUE;
    ret = R_RIIC_MasterSend(&iic_info_0);
    while(send_busy == TRUE);
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
	send_busy = TRUE;	//送信終了時にFalseになる。
	ret = R_RIIC_MasterSend(&iic_info_0);
	while(send_busy == TRUE);	//送信終了するまでコールバックを待つ

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

	send_busy = TRUE;	//送信終了時にFalseになる。
	ret = R_RIIC_MasterSend(&iic_info_0);
	while(send_busy == TRUE);	//送信終了までコールバックを待つ


	//描画データ送信
	iic_info_0.cnt1st = sizeof(control_byte);
	iic_info_0.cnt2nd = sizeof(gram.buf_byte);
	iic_info_0.p_data1st = control_byte;
	iic_info_0.p_data2nd = gram.buf_byte;
	iic_info_0.p_slv_adr = oled_address;

	control_byte[0] = kCtrlData;

	send_busy = TRUE;
	ret = R_RIIC_MasterSend(&iic_info_0);
	while(send_busy == TRUE);
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
static void DrawWhite(void){
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
＊　関数名　：OledDataSendFinish
＊　機能　　： i2c送信終了コールバックハンドラ
          ここでは送信ビジーフラグをクリアする。
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void OledSendCallBack(void){
	send_busy = FALSE;
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