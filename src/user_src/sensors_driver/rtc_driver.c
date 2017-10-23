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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

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

static RtcTime current_time;

/***公開関数*******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void RtcDriverTask(void){

	InitRtc();

	while(1){
		vTaskDelay(500/portTICK_PERIOD_MS);
		RefreshRtcState();
		//vTaskDelay(500/portTICK_PERIOD_MS);
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetCurrentTimeSec(void){

	return(current_time.sec);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetCurrentTimeMin(void){

	return(current_time.min);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetCurrentTimeHour(void){

	return(current_time.hour);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetCurrentTimeHour24(void){

	return(current_time.hour24);
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

	//時間管理構造体の初期化
	current_time.sec    = 33;
	current_time.min    = 10;
	current_time.hour   = 10;
	current_time.hour24 = 10;

	//RTCの発振安定時間待機
	vTaskDelay(2000/portTICK_PERIOD_MS);

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

	ConvertSec(rtc_mem.registers.seconds.byte);
	ConvertMin(rtc_mem.registers.minutes.byte);
	ConvertHour(rtc_mem.registers.hours.byte, rtc_mem.registers.control1.byte);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void ConvertSec(uint8_t data){
	Rx8025Seconds sec;

	sec.byte = data;
	current_time.sec = sec.bit.s1;
	current_time.sec += sec.bit.s2 * 2;
	current_time.sec += sec.bit.s4 * 4;
	current_time.sec += sec.bit.s8 * 8;
	current_time.sec += sec.bit.s10 * 10;
	current_time.sec += sec.bit.s20 * 20;
	current_time.sec += sec.bit.s40 * 40;
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void ConvertMin(uint8_t data){
	Rx8025Minutes min;

	min.byte = data;
	current_time.min = min.bit.m1;
	current_time.min += min.bit.m2 * 2;
	current_time.min += min.bit.m4 * 4;
	current_time.min += min.bit.m8 * 8;
	current_time.min += min.bit.m10 * 10;
	current_time.min += min.bit.m20 * 20;
	current_time.min += min.bit.m40 * 40;
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void ConvertHour(uint8_t data, uint8_t ctrl_reg){
	Rx8025Hours hour;
	Rx8025Control1 reg;

	reg.byte = ctrl_reg;

	if(reg.bit.h12_24 == TRUE)
	{
		//24時間制
		hour.byte = data;
		current_time.hour = hour.bit.h1;
		current_time.hour += hour.bit.h2 * 2;
		current_time.hour += hour.bit.h4 * 4;
		current_time.hour += hour.bit.h8 * 8;
		current_time.hour += hour.bit.h10 * 10;
		current_time.hour24 = current_time.hour;
		current_time.hour24 += hour.bit.h20_pa * 20;
	}else{
		//12時間制
		hour.byte = data;
		current_time.hour = hour.bit.h1;
		current_time.hour += hour.bit.h2 * 2;
		current_time.hour += hour.bit.h4 * 4;
		current_time.hour += hour.bit.h8 * 8;
		current_time.hour += hour.bit.h10 * 10;
		current_time.hour24 = current_time.hour;
		if(hour.bit.h20_pa)
		{
			//PM
			current_time.hour24 += 12;
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