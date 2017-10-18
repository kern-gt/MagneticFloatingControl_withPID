/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： rtc_driver.c
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
#include "i2c_driver.h"
#include "rx-8025.h"

/* Kernel includes. */
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/queue.h"

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "rtc_driver_in.h"

/**----------------------------------------------------------------------------
<<変数>>
-----------------------------------------------------------------------------**/
/* kernel */
extern xQueueHandle sci0_iic_queue;
extern xQueueHandle rtc_que;

/**/
#define RTC_ADDR (RX_8025_ADDRESS)	//I2CAddress

static RtcRegisters rtc_mem;

/***公開関数*******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void RtcDriverTask(void){

	//vTaskDelay(3000/portTICK_PERIOD_MS);
	InitRtc();

	while(1){
		vTaskDelay(1000/portTICK_PERIOD_MS);
		RefreshRtcState();
		//vTaskDelay(500/portTICK_PERIOD_MS);
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
	Rx8025TransferByte mode;  //RTC転送モード
	uint8_t init_data;        //RTC書き込みデータ
	I2cData_t i2c_data;
	I2cRetData_t i2c_ret;

	//RTCのレジスタアドレス、転送モード、設定データ初期化
	mode.bit.address = 0xE;
	mode.bit.transmode = kTransStandardMode;
	init_data = 0x03;

	i2c_data.que_id = (QueueID)rtc_que;
	i2c_data.p_data1st = &mode.byte;
	i2c_data.p_data2nd = &init_data;
	i2c_data.cnt1st    = sizeof(mode.byte);
	i2c_data.cnt2nd    = sizeof(init_data);
	i2c_data.slave_addr= RTC_ADDR;
	i2c_data.mast_slv_r_w = kI2cMasterSend;

	xQueueSend(sci0_iic_queue, &i2c_data, portMAX_DELAY);

	xQueueReceive(rtc_que, &i2c_ret, portMAX_DELAY);

	switch(i2c_ret.ret){
		case kI2cSuccess:
			break;

		case kI2cError:
			break;

		default:
			break;
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：RefreshRtcState
＊　機能　　： RTCの全レジスタ読み込み
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void RefreshRtcState(void){
	Rx8025TransferByte mode;
	I2cData_t i2c_data;
	I2cRetData_t i2c_ret;

	//RTCのレジスタアドレス、転送モード、設定データ初期化
	mode.bit.address = 0x0;
	mode.bit.transmode = kTransStandardMode;

	i2c_data.que_id = (QueueID)rtc_que;
	i2c_data.p_data1st = &mode.byte;
	i2c_data.p_data2nd = rtc_mem.read_data;
	i2c_data.cnt1st    = sizeof(mode.byte);
	i2c_data.cnt2nd    = sizeof(rtc_mem.read_data);
	i2c_data.slave_addr= RTC_ADDR;
	i2c_data.mast_slv_r_w = kI2cMasterReceive;

	xQueueSend(sci0_iic_queue, &i2c_data, portMAX_DELAY);

	xQueueReceive(rtc_que, &i2c_ret, portMAX_DELAY);

	switch(i2c_ret.ret){
		case kI2cSuccess:
			break;

		case kI2cError:
			break;

		default:
			break;
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