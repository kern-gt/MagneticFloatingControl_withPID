/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： rtc_driver_in.h
＊　責務　　　：
＊　作成日　　： 2017/10/11
＊　作成者　　：　kern-gt

＊　備考　　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
#ifndef _RTC_DRIVER_IN_H_
#define _RTC_DRIVER_IN_H_

#include "rtc_driver.h"

/**----------------------------------------------------------------------------
<<非公開マクロ定義>>
-----------------------------------------------------------------------------**/


/**----------------------------------------------------------------------------
<<非公開型定義>>
-----------------------------------------------------------------------------**/
//時間管理構造体
typedef struct{
	uint8_t  sec;
	uint8_t  min;
	uint8_t  hour12;
	uint8_t  hour24;

	uint8_t  ampm;
	uint8_t  day;
	uint8_t  weekday;
	uint8_t  month;

	uint16_t year;
	uint16_t padding1;
}RtcTime;

//RTC動作状態構造体
typedef union{
	uint8_t byte;
	struct{
		uint8_t low_voltage:1;
		uint8_t stop_osc   :1;
		uint8_t reset      :1;
		uint8_t :5;
	}bit;
}RtcRunStates;


//エラー定義
typedef enum{
	kOk = 0,
	kErrorAddress,
	kErrorUnknown
}RtcErrorEnum;

/**----------------------------------------------------------------------------
<<非公開プロトタイプ定義>>
-----------------------------------------------------------------------------**/
/*static */
static void InitRtc(void);
static void RtcCallBackMaster(void);
static void RefreshRtcState(void);
static void RtcDeviceCallBack(uint8_t ret);

static void ConvertSec(uint8_t data);
static void ConvertMin(uint8_t data);
static void ConvertHour24(uint8_t data);
static void ConvertWeekday(uint8_t data);
static void ConvertDay(uint8_t data);
static void ConvertMonth(uint8_t data);
static void ConvertYears(uint8_t data);
static void ConvertRtcRunStates(uint8_t data);

static uint8_t DecodeBCD8(uint8_t bcd_data);
static uint8_t EncodeBCD8(uint8_t int_data);

static void SetRtcConfig(void);
#endif /*_RTC_DRIVER_IN_H_*/