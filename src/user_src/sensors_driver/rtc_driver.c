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
//#include "queue.h"

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "rtc_driver_in.h"

/**----------------------------------------------------------------------------
<<変数>>
-----------------------------------------------------------------------------**/
/**/
static xTaskHandle mytask_id;

static RtcRegisters rtc_mem;
static RtcTime current_times, preset_times;
static RtcRunStates current_states;
static uint8_t set_config_flag;

#define RTC_ADDR (RX_8025_ADDRESS)	//I2CAddress
static I2cRetData_t i2c_ret;

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

	return(current_times.sec);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetCurrentTimeMin(void){

	return(current_times.min);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetCurrentTimeHour12(void){

	return(current_times.hour12);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetCurrentTimeHour24(void){

	return(current_times.hour24);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
RtcTimeEnum GetCurrentTimeAmPm(void){
	return((RtcTimeEnum)current_times.ampm);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetCurrentTimeMon(void){
	return(current_times.month);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetCurrentTimeDay(void){
	return(current_times.day);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
RtcTimeEnum GetCurrentTimeWeekDay(void){
	return((RtcTimeEnum)current_times.weekday);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint16_t GetCurrentTimeYear(void){
	return(current_times.year);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetRtcLowVolState(void){
	uint8_t ret;

	ret = current_states.bit.low_voltage;
	current_states.bit.low_voltage = FALSE;
	return(ret);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetRtcStopOscState(void){
	uint8_t ret;

	ret = current_states.bit.stop_osc;
	current_states.bit.stop_osc = FALSE;
	return(ret);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t GetRtcResetState(void){
	uint8_t ret;

	ret = current_states.bit.reset;
	current_states.bit.reset = FALSE;
	return(ret);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void PresetSeconds(uint8_t sec){
	if(sec < 60){
		preset_times.sec = sec;
	}else if(sec == 60){
		preset_times.sec = 0;
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void PresetMinites(uint8_t min){
	if(min < 60)
	{
		preset_times.min = min;
	}else if(min == 60){
		preset_times.min = 0;
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void PresetHours24(uint8_t hour){
	if(hour < 24){
		preset_times.hour24 = hour;
		if(hour < 13){
			if(hour == 0){
				preset_times.hour12 = 12;
			}else{
				preset_times.hour12 = hour;
			}
			preset_times.ampm = (uint8_t)kAM;
		}else{
			preset_times.hour12 = hour - 12;
			preset_times.ampm = (uint8_t)kPM;
		}
	}else if(hour == 24){
		preset_times.hour24 = 12;
		preset_times.ampm = (uint8_t)kAM;
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void PresetDays(uint8_t day){
	if(day < 32)
	{
		preset_times.day = day;
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void PresetMonths(uint8_t mon){
	if(mon < 13)
	{
		preset_times.month = mon;
	}
}

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void PresetWeekdays(RtcTimeEnum weekday){
	preset_times.weekday = (uint8_t)weekday;
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void PresetYears(uint16_t year){
	if(year < 2100)
	{
		preset_times.year = year;
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void ApplyRtcConfig(void){
	set_config_flag = TRUE;
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
	uint8_t init_data[2];        //RTC書き込みデータ
	Rx8025Control1 init_data1;
	Rx8025Control2 init_data2;
	I2cData_t i2c_data;

	//時間管理構造体の初期化
	current_times.sec    = 5;
	current_times.min    = 10;
	current_times.hour12 = 10;
	current_times.hour24 = 10;

	current_times.ampm   = kAM;
	current_times.day    = 1;
	current_times.weekday= kSun;
	current_times.month  = 1;
	current_times.year   = 2017;

	//設定用時間管理構造体の初期化
	preset_times.sec    = 5;
	preset_times.min    = 10;
	preset_times.hour12 = 10;
	preset_times.hour24 = 10;

	preset_times.ampm   = kAM;
	preset_times.day    = 1;
	preset_times.weekday= kSun;
	preset_times.month  = 1;
	preset_times.year   = 2017;

	//RTC動作状態構造体初期化
	current_states.byte = 0U;

	//設定更新フラグ初期化
	set_config_flag = FALSE;

	mytask_id = xTaskGetCurrentTaskHandle();

	//RTCの発振安定時間待機
	vTaskDelay(2000/portTICK_PERIOD_MS);

	//RTCのレジスタアドレス、転送モード、設定データ初期化
	mode.bit.address      = kAddControl1;
	mode.bit.transmode    = kTransStandardMode;
	init_data1.bit.ct     = 0x03U; //1Hz定周期割込み
	init_data1.bit.h12_24 = 1U;    //24時間制
	init_data2.bit.xst    = 1U;
	init_data2.bit.pon    = 0U;
	init_data2.bit.vdsl   = 0U;

	init_data[0] = init_data1.byte;
	init_data[1] = init_data2.byte;

	i2c_data.callbackfunc = RtcDeviceCallBack;
	i2c_data.p_data1st    = &mode.byte;
	i2c_data.p_data2nd    = init_data;
	i2c_data.cnt1st       = sizeof(mode.byte);
	i2c_data.cnt2nd       = sizeof(init_data);
	i2c_data.slave_addr   = RTC_ADDR;
	i2c_data.mast_slv_r_w = kI2cMasterSend;

	I2cWrite(&i2c_data);
	vTaskSuspend(mytask_id);

	switch(i2c_ret.ret){
		case kI2cSuccess:
			break;

		case kI2cError:
			break;

		default:
			break;
	}

	SetRtcConfig();
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

	//RTC時刻設定
	if(set_config_flag == TRUE){
		SetRtcConfig();
		set_config_flag = FALSE;
	}

	//RTCのレジスタアドレス、転送モード、設定データ初期化
	mode.bit.address = 0x0;
	mode.bit.transmode = kTransStandardMode;

	i2c_data.callbackfunc = RtcDeviceCallBack;
	i2c_data.p_data1st = &mode.byte;
	i2c_data.p_data2nd = rtc_mem.read_data;
	i2c_data.cnt1st    = sizeof(mode.byte);
	i2c_data.cnt2nd    = sizeof(rtc_mem.read_data);
	i2c_data.slave_addr= RTC_ADDR;
	i2c_data.mast_slv_r_w = kI2cMasterReceive;

	I2cWrite(&i2c_data);
	vTaskSuspend(mytask_id);

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
	ConvertHour24(rtc_mem.registers.hours.byte);
	ConvertWeekday(rtc_mem.registers.weekdays.byte);
	ConvertDay(rtc_mem.registers.days.byte);
	ConvertMonth(rtc_mem.registers.months.byte);
	ConvertYears(rtc_mem.registers.years.byte);
	ConvertRtcRunStates(rtc_mem.registers.control2.byte);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void SetRtcConfig(void){
	Rx8025TransferByte mode;  //RTC転送モード
	RtcRegisters config_data; //RTC書き込みデータ
	I2cData_t i2c_data;
	uint8_t tmp;

	//設定データ作成

	config_data.registers.seconds.byte  = EncodeBCD8(preset_times.sec);
	config_data.registers.minutes.byte  = EncodeBCD8(preset_times.min);
	config_data.registers.hours.byte    = EncodeBCD8(preset_times.hour24);
	config_data.registers.weekdays.byte = (uint8_t)preset_times.weekday;
	config_data.registers.months.byte   = EncodeBCD8(preset_times.month);
	tmp = preset_times.year - 2000;
	config_data.registers.years.byte    = EncodeBCD8(tmp);
	config_data.registers.days.byte     = EncodeBCD8(preset_times.day);

	//RTCのレジスタアドレス、転送モード、設定データ初期化
	mode.bit.address   = kAddSeconds;
	mode.bit.transmode = kTransStandardMode;

	i2c_data.callbackfunc = RtcDeviceCallBack;
	i2c_data.p_data1st    = &mode.byte;
	i2c_data.p_data2nd    = config_data.read_data;
	i2c_data.cnt1st       = sizeof(mode.byte);
	i2c_data.cnt2nd       = 7U;
	i2c_data.slave_addr   = RTC_ADDR;
	i2c_data.mast_slv_r_w = kI2cMasterSend;

	I2cWrite(&i2c_data);
	vTaskSuspend(mytask_id);

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
static void ConvertSec(uint8_t data){
	current_times.sec = DecodeBCD8(data);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void ConvertMin(uint8_t data){
	current_times.min = DecodeBCD8(data);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void ConvertHour24(uint8_t data){
	//24時間制モード専用
	current_times.hour24 = DecodeBCD8(data);
	if(current_times.hour24 < 12){
		if(current_times.hour24 == 0){
			current_times.hour12 = 12;
		}else{
			current_times.hour12 = current_times.hour24;
		}
		current_times.ampm = kAM;
	}else{
		if((current_times.hour24 - 12) == 0){
			current_times.hour12 = 12;
		}else{
			current_times.hour12 = current_times.hour24 - 12;
		}
		current_times.ampm = kPM;
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void ConvertWeekday(uint8_t data){
	current_times.weekday = (RtcTimeEnum)DecodeBCD8(data);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void ConvertDay(uint8_t data){
	current_times.day = DecodeBCD8(data);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void ConvertMonth(uint8_t data){
	current_times.month = DecodeBCD8(data);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void ConvertYears(uint8_t data){
	current_times.year = 2000 + DecodeBCD8(data);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void ConvertRtcRunStates(uint8_t data){
	Rx8025Control2 reg;

	reg.byte = data;

	current_states.bit.low_voltage = reg.bit.vdsl;
	current_states.bit.stop_osc    = ~reg.bit.xst;
	current_states.bit.reset       = reg.bit.pon;
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static uint8_t DecodeBCD8(uint8_t bcd_data){
	uint8_t ret, high, low;

	high = bcd_data>>4;
	if(high < 10){
		low  = (0x0F & bcd_data);
		if(low < 10){
			ret  = low;
			ret += high*10;	
		}else{
			ret = NULL;
		}
	}else{
		ret = NULL;
	}
	return(ret);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static uint8_t EncodeBCD8(uint8_t int_data){
	uint8_t ret, high, low;

	if(int_data < 100){
		low  = int_data%10;
		high = int_data/10;
		ret = high<<4;
		ret += low;
	}else{
		ret = NULL;
	}

	return(ret);	
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void RtcDeviceCallBack(uint8_t ret){

	i2c_ret.ret = ret;
	vTaskResume(mytask_id);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/