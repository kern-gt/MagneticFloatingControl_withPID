/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： RX-8025.h
＊　責務　　　：
＊　作成日　　： 2017/10/12
＊　作成者　　：　kern-gt

＊　備考　  ：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
#ifndef _RX_8025_H_
#define _RX_8025_H_

/**----------------------------------------------------------------------------
<<外部公開マクロ定義>>
-----------------------------------------------------------------------------**/


/**----------------------------------------------------------------------------
<<外部公開型定義>>
-----------------------------------------------------------------------------**/
//RX-8025用アドレス+転送モード指定バイト定義************************
	//転送モード定義
	typedef enum{
		kTransStandardMode = 0x0, //標準Read,Writeモード
		kTransRapidMode    = 0x4  //短縮Readモード
	}Rx8025TransferModeEnum;

	//アドレス+転送モード指定バイト構造体
	typedef union{
		uint8_t byte;
		struct
		{
			uint8_t transmode:4;
			uint8_t address:4;
		}elements;
	}Rx8025TransferByte;

//RX-8025用レジスタアドレス定義*********************
typedef enum{
	kAddSeconds = 0x0,
	kAddMinutes,
	kAddHours,
	kAddWeekdays,
	kAddDays,
	kAddMonths,
	kAddYears,
	kAddDigitalOffsets,
	kAddAlarmWMinute,
	kAddAlarmWHour,
	kAddAlarmWWeekday,
	kAddAlarmDMinute,
	kAddAlarmDHour,
	kAddReserved,
	kAddControl1,
	kAddControl2
}Rx8025RegisterEnum;

//RX-8025NBレジスタ定義**************************
typedef union{
	uint8_t byte;
	struct{
		uint8_t s1:1;
		uint8_t s2:1;
		uint8_t s4:1;
		uint8_t s8:1;
		uint8_t s10:1;
		uint8_t s20:1;
		uint8_t s40:1;
		uint8_t :1;
	}bit;
}Seconds;

typedef union{
	uint8_t byte;
	struct{
		uint8_t m1:1;
		uint8_t m2:1;
		uint8_t m4:1;
		uint8_t m8:1;
		uint8_t m10:1;
		uint8_t m20:1;
		uint8_t m40:1;
		uint8_t :1;		
	}bit;
}Minutes;

typedef union{
	uint8_t byte;
	struct{
		uint8_t h1:1;
		uint8_t h2:1;
		uint8_t h4:1;
		uint8_t h8:1;
		uint8_t h10:1;
		uint8_t h20:1;
		uint8_t :2;
	}bit;
}Hours;

typedef union{
	uint8_t byte;
	struct{
		uint8_t w1:1;
		uint8_t w2:1;
		uint8_t w4:1;
		uint8_t :5;
	}bit;
}Weekdays;

typedef union{
	uint8_t byte;
	struct{
		uint8_t d1:1;
		uint8_t d2:1;
		uint8_t d4:1;
		uint8_t d8:1;
		uint8_t d10:1;
		uint8_t d20:1;
		uint8_t :2;
	}bit;
}Days;

typedef union{
	uint8_t byte;
	struct
	{
		uint8_t mo1:1;
		uint8_t mo2:1;
		uint8_t mo4:1;
		uint8_t mo8:1;
		uint8_t mo10:1;
		uint8_t :2;
		uint8_t c:1;
	}bit;
}Months;

typedef union{
	uint8_t byte;
	struct{
		uint8_t y1:1;
		uint8_t y2:1;
		uint8_t y4:1;
		uint8_t y8:1;
		uint8_t y10:1;
		uint8_t y20:1;
		uint8_t y40:1;
		uint8_t y80:1;
	}bit;
}Years;

typedef union{
	uint8_t byte;
	struct{
		uint8_t f0:1;
		uint8_t f1:1;
		uint8_t f2:1;
		uint8_t f3:1;
		uint8_t f4:1;
		uint8_t f5:1;
		uint8_t f6:1;
		uint8_t test:1;
	}bit;
}Digital_offsets;

typedef union{
	uint8_t byte;
	struct{
		uint8_t wm1:1;
		uint8_t wm2:1;
		uint8_t wm4:1;
		uint8_t wm8:1;
		uint8_t wm10:1;
		uint8_t wm20:1;
		uint8_t wm40:1;
		uint8_t :1;
	}bit;
}AlarmWMinute;

typedef union{
	uint8_t byte;
	struct{
		uint8_t wh1:1;
		uint8_t wh2:1;
		uint8_t wh4:1;
		uint8_t wh8:1;
		uint8_t wh10:1;
		uint8_t wh20:1;
		uint8_t :2;
	}bit;
}AlarmWHour;

typedef union{
	uint8_t byte;
	struct{
		uint8_t ww0:1;
		uint8_t ww1:1;
		uint8_t ww2:1;
		uint8_t ww3:1;
		uint8_t ww4:1;
		uint8_t ww5:1;
		uint8_t ww6:1;
		uint8_t :1;
	}bit;
}AlarmWWeekday;

typedef union{
	uint8_t byte;
	struct{
		uint8_t dm1:1;
		uint8_t dm2:1;
		uint8_t dm4:1;
		uint8_t dm8:1;
		uint8_t dm10:1;
		uint8_t dm20:1;
		uint8_t dm40:1;
		uint8_t :1;
	}bit;
}AlarmDMinute;

typedef union{
	uint8_t byte;
	struct{
		uint8_t dh1:1;
		uint8_t dh2:1;
		uint8_t dh4:1;
		uint8_t dh8:1;
		uint8_t dh10:1;
		uint8_t dh20:1;
		uint8_t :2;
	}bit;
}AlarmDHour;

typedef union{
	uint8_t byte;
	struct{
		uint8_t ct0:1;
		uint8_t ct1:1;
		uint8_t ct2:1;
		uint8_t test:1;
		uint8_t clen2:1;
		uint8_t h12_24:1;
		uint8_t dale:1;
		uint8_t wale:1;
	}bit;
}Control1;

typedef union{
	uint8_t byte;
	struct{
		uint8_t dafg:1;
		uint8_t wafg:1;
		uint8_t ctfg:1;
		uint8_t clen1:1;
		uint8_t pon:1;
		uint8_t xst:1;
		uint8_t vdet:1;
		uint8_t vdsl:1;
	}bit;
}Control2;

//RX-8025NB全レジスタ構造体*************************
typedef union{
	uint8_t  read_data[16];
	struct
	{
		Seconds         seconds;
		Minutes         minutes;
		Hours           hours;
		Weekdays        weekdays;
		Days            days;
		Months          months;
		Years           years;
		Digital_offsets digital_offsets;
		AlarmWMinute    alarm_w_minute;
		AlarmWHour      alarm_w_hour;
		AlarmWWeekday   alarm_w_weekday;
		AlarmDMinute    alarm_d_minute;
		AlarmDHour      alarm_d_hour;
		uint8_t         reserved;
		Control1        control1;
		Control2        control2;
	}registers;
}RtcRegisters;


/**----------------------------------------------------------------------------
<<外部公開プロトタイプ定義>>
-----------------------------------------------------------------------------**/
/*extern*/

#endif /* _RX_8025_H_ */