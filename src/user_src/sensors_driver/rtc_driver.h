/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： rtc_driver.h
＊　責務　　　：
＊　作成日　　： 2017/10/11
＊　作成者　　：　kern-gt

＊　備考　  ：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
#ifndef _RTC_DRIVER_H_
#define _RTC_DRIVER_H_

/**----------------------------------------------------------------------------
<<外部公開マクロ定義>>
-----------------------------------------------------------------------------**/


/**----------------------------------------------------------------------------
<<外部公開型定義>>
-----------------------------------------------------------------------------**/
//時間管理定義
typedef enum{
	kSun = 0,
	kMon,
	kTues,
	kWed,
	kThurs,
	kFri,
	kSat,
	kAM,
	kPM
}RtcTimeEnum;

/**----------------------------------------------------------------------------
<<外部公開プロトタイプ定義>>
-----------------------------------------------------------------------------**/
/*extern*/
extern void RtcDriverTask(void);
extern uint8_t GetCurrentTimeSec(void);
extern uint8_t GetCurrentTimeMin(void);
extern uint8_t GetCurrentTimeHour12(void);
extern uint8_t GetCurrentTimeHour24(void);
RtcTimeEnum GetCurrentTimeAmPm(void);
uint8_t GetCurrentTimeMon(void);
uint8_t GetCurrentTimeDay(void);
RtcTimeEnum GetCurrentTimeWeekDay(void);
uint16_t GetCurrentTimeYear(void);
uint8_t GetRtcLowVolState(void);
uint8_t GetRtcStopOscState(void);
uint8_t GetRtcResetState(void);

extern void PresetSeconds(uint8_t sec);
extern void PresetMinites(uint8_t min);
extern void PresetHours24(uint8_t hour);
extern void PresetMonths(uint8_t mon);
extern void PresetDays(uint8_t day);
extern void PresetWeekdays(RtcTimeEnum weekday);
extern void PresetYears(uint16_t year);
extern void ApplyRtcConfig(void);

#endif /* _RTC_DRIVER_H_ */