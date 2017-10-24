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
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t hour24;
}RtcTime;

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
static void ConvertHour(uint8_t data, uint8_t ctrl_reg);

#endif /*_RTC_DRIVER_IN_H_*/