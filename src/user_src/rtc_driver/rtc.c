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

/* デバイス */
#include "r_sci_iic_rx_if.h"
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
#define RTC_ADDR (RX_8025_ADDRESS)	//I2CAddress(FITドライバAPIはビットシフト無し)
static volatile int i2c_runninng = FALSE; //I2C送受信中フラグ

static sci_iic_info_t siic_info_0;

//static uint8_t write_command[2] = {0, 0};
static RtcRegisters rtc_mem;

//static uint8_t * const write_command_p = write_command;
//static uint8_t * const rtc_mem_p = rtc_mem.read_data;

/***公開関数*******************************************************************/
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
		RefreshRtcState();
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
	volatile sci_iic_return_t ret;           //i2cAPI戻り値
	Rx8025TransferByte mode;              //RTC転送モード
	uint8_t write_command;                //RTC書き込みデータ
	uint8_t rtc_address[1] = {RTC_ADDR};  //スレーブアドレス

	/*I2C初期化*****************************************/
	siic_info_0.dev_sts = SCI_IIC_NO_INIT;
	siic_info_0.ch_no   = 0;

	ret = R_SCI_IIC_Open(&siic_info_0);

	/*INTAの1Hz周期割込み設定*****************************/
	//通信構造体を設定
	siic_info_0.callbackfunc = &RtcCallBackMaster;
	siic_info_0.cnt1st = 1;
	siic_info_0.cnt2nd = 1;
	siic_info_0.p_data1st = &mode.byte;
	siic_info_0.p_data2nd = &write_command;
	siic_info_0.p_slv_adr = rtc_address;

	//RTCのレジスタアドレス、転送モード、設定データ初期化
	mode.bit.address = 0xE;
	mode.bit.transmode = kTransStandardMode;
	write_command = 0x03;

	i2c_runninng = TRUE;	//送信終了時にFalseになる。
	ret = R_SCI_IIC_MasterSend(&siic_info_0);
	while(i2c_runninng == TRUE);	//送信終了までコールバックを待つ
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：RefreshRtcState
＊　機能　　： RTCの全レジスタ読み込み
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void RefreshRtcState(void){
	volatile sci_iic_return_t ret;
	Rx8025TransferByte mode;
	uint8_t rtc_address[1] = {RTC_ADDR};

	/*アドレス0x0~0xfまで順次読出し*/
	siic_info_0.cnt1st = 1;
	siic_info_0.cnt2nd = sizeof(rtc_mem.read_data);
	siic_info_0.p_data1st = &mode.byte;
	siic_info_0.p_data2nd = rtc_mem.read_data;
	siic_info_0.p_slv_adr = rtc_address;

	mode.bit.address = 0x0;
	mode.bit.transmode = kTransStandardMode;

	i2c_runninng = TRUE;	//送信終了時にFalseになる。
	ret = R_SCI_IIC_MasterReceive(&siic_info_0);
	while(i2c_runninng == TRUE);	//送信終了コールバックを待つ
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