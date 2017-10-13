/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： rtc.c
＊　責務　　　：
＊　作成日　　： 2017/10/11
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
#include "r_riic_rx_if.h"
#include "rx-8025.h"

/* Kernel includes. */
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/queue.h"

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "rtc_in.h"

/**----------------------------------------------------------------------------
<<変数>>
-----------------------------------------------------------------------------**/
#define RTC_ADDR (0x32)	//I2CAddress
static volatile int i2c_runninng = FALSE; //I2C送受信中フラグ

static riic_info_t iic_info_0;

//static uint8_t write_command[2] = {0, 0};
static RtcRegisters rtc_mem;

//static uint8_t * const write_command_p = write_command;
//static uint8_t * const rtc_mem_p = rtc_mem.read_data;

/***公開関数*******************************************************************/

void Rtctest2(void){
	volatile riic_return_t ret;
	Rx8025TransferByte mode;
	uint8_t rtc_address[1] = {RTC_ADDR};

	//
	iic_info_0.cnt1st = 1;
	iic_info_0.cnt2nd = sizeof(rtc_mem.read_data);
	iic_info_0.p_data1st = &mode.byte;
	iic_info_0.p_data2nd = rtc_mem.read_data;
	iic_info_0.p_slv_adr = rtc_address;

	mode.elements.address = 0x0;
	mode.elements.transmode = kTransStandardMode;

	i2c_runninng = TRUE;	//送信終了時にFalseになる。
	ret = R_RIIC_MasterReceive(&iic_info_0);
	while(i2c_runninng == TRUE);	//送信終了まで待つ
}	
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void RtcDriverTask(void){

	//
	//vTaskDelay(3000/portTICK_PERIOD_MS);
	InitRtc();


	while(1){
		//vTaskDelay(500/portTICK_PERIOD_MS);
		//Rtctest();
		Rtctest2();
		vTaskDelay(500/portTICK_PERIOD_MS);
		//Rtctest1();
	}
}


/***非公開関数******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void InitRtc(void){
	volatile riic_return_t ret;
	Rx8025TransferByte mode;
	uint8_t write_command;
	uint8_t rtc_address[1] = {RTC_ADDR};

	iic_info_0.dev_sts = RIIC_NO_INIT;
	iic_info_0.ch_no   = 0;

	ret = R_RIIC_Open(&iic_info_0);
	//
	iic_info_0.callbackfunc = &RtcCallBackMaster;
	iic_info_0.cnt1st = 1;
	iic_info_0.cnt2nd = 1;
	iic_info_0.p_data1st = &mode.byte;
	iic_info_0.p_data2nd = &write_command;
	iic_info_0.p_slv_adr = rtc_address;

	mode.elements.address = 0xE;
	mode.elements.transmode = kTransStandardMode;
	write_command = 0x03;

	i2c_runninng = TRUE;	//送信終了時にFalseになる。
	ret = R_RIIC_MasterSend(&iic_info_0);
	while(i2c_runninng == TRUE);	//送信終了まで待つ
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：RtcCallBackMaster
＊　機能　　： i2c通信終了コールバックハンドラ
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void RtcCallBackMaster(void){
	i2c_runninng = FALSE;
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