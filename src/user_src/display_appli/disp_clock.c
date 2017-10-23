/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： disp_clock.c
＊　責務　　　：
＊　作成日　　： 2017/10/19
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

/* 数学ライブラリ */
//#include <mathf.h>
#include "sin_cos.h"

/* 描画データ */
#include "draw_data.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ドライバ */
#include "oled.h"
#include "temp_sens_driver.h"
#include "rtc_driver.h"

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "disp_clock_in.h"

/**----------------------------------------------------------------------------
<<変数>>
-----------------------------------------------------------------------------**/
#define PI (3.1415927f)
const uint16_t clock_x0 = 0;
const uint16_t clock_y0 = 0;
const uint16_t clock_centerx = 31;
const uint16_t clock_centery = 31;
const uint16_t temp_x0  = 64;
const uint16_t temp_y0  = 0;
const uint16_t humi_x0  = 64;
const uint16_t humi_y0  = 33;

const float sec_hand_length = 22.0f;
const float min_hand_length = 19.0f;
const float hour_hand_length = 13.0f;

/***公開関数*******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
uint8_t DispClock(uint8_t changeflag){
	uint8_t bitblt_flag = FALSE;
	uint16_t x, y, angle, sec=0, min=0, hour=0;
	uint8_t num_i[4];
	uint32_t num_l;
	float hour_angle;

	if(changeflag){
		InitDispClock();
	}

	//描画エリアクリア
	DrawCircle(clock_x0+clock_centerx, clock_y0+clock_centery,24,0,1); //文字盤クリア
	DrawRectangle(temp_x0+1, temp_y0+1, temp_x0+63-1, temp_y0+31-1, 0, 1);
	DrawRectangle(humi_x0+1, humi_y0+1, humi_x0+63-1, humi_y0+30-1, 0, 1);

	//時計文字盤描画
	sec = (uint16_t)GetCurrentTimeSec();
	min = (uint16_t)GetCurrentTimeMin();
	hour= (uint16_t)GetCurrentTimeHour();

	//秒針描画
	if(sec < 15)
	{
		angle = (uint16_t)((float)sec + 45.0f)*360.0f/60.0f;
	}else if(sec < 360)
	{
		angle = (uint16_t)((float)sec - 15.0f)*360.0f/60.0f;
	}else{
		angle = 0;
	}
	x = (uint16_t)(((sec_hand_length) * CosDeg(angle)) + (float)clock_centerx);
	y = (uint16_t)((sec_hand_length * SinDeg(angle)) + (float)clock_centery);
	DrawLine(clock_centerx, clock_centery, x, y,1);

	//分針描画
	if(min < 15)
	{
		angle = (uint16_t)((float)min + 45.0f)*360.0f/60.0f;
	}else if(min < 360)
	{
		angle = (uint16_t)((float)min - 15.0f)*360.0f/60.0f;
	}else{
		angle = 0;
	}
	x = (uint16_t)(((min_hand_length) * CosDeg(angle)) + (float)clock_centerx);
	y = (uint16_t)((min_hand_length * SinDeg(angle)) + (float)clock_centery);
	DrawLine(clock_centerx, clock_centery, x, y,1);

	//時針描画
	hour_angle = (float)(hour*5) + (float)min/60.0f*5.0f;
	if(hour_angle < 15.0f)
	{
		angle = (uint16_t)(hour_angle + 45.0f)*360.0f/60.0f;
	}else if(hour_angle < 360.0f)
	{
		angle = (uint16_t)(hour_angle - 15.0f)*360.0f/60.0f;
	}else{
		angle = 0;
	}
	x = (uint16_t)(((hour_hand_length) * CosDeg(angle)) + (float)clock_centerx);
	y = (uint16_t)((hour_hand_length * SinDeg(angle)) + (float)clock_centery);
	DrawLine(clock_centerx, clock_centery, x, y,1);

	//上側湿度
	//PasteImage1616(65+13*0, 1,test2_font);		//時計"00:00"
	//PasteImage1616(65+13*1, 1,test2_font);
	//PasteImage1616(65+(13*2)-3, 1,colon_font);
	//PasteImage1616(65+(13*3)-5, 1,test2_font);
	//PasteImage1616(65+(13*4)-5, 1,test2_font);

	num_l = (uint32_t)(10.0f * ReadHumidity());
	num_i[2] = num_l%10;
	num_l /= 10;
	num_i[1] = num_l%10;
	num_l /= 10;
	num_i[0] = num_l%10;

	PrintNumber(65+(14*0), 1, num_i[0]);
	PrintNumber(65+(14*1), 1, num_i[1]);
	PasteImage1616(65+(14*2), 1,dot_font);
	PrintNumber(65+(14*2), 1, num_i[2]);
	PasteImage1616(65+(14*3), 1,percent_font);

	//下側温度
	num_l = (uint32_t)(10.0f * ReadTemperature());
	num_i[2] = num_l%10;
	num_l /= 10;
	num_i[1] = num_l%10;
	num_l /= 10;
	num_i[0] = num_l%10;

	PrintNumber(65+(14*0), 5, num_i[0]);
	PrintNumber(65+(14*1), 5, num_i[1]);
	PasteImage1616(65+(14*2), 5,dot_font);
	PrintNumber(65+(14*2), 5, num_i[2]);
	PasteImage1616(65+(14*3), 5,celsius_font);
	
	//画面更新
	bitblt_flag = TRUE;

	return(bitblt_flag);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/

/***非公開関数******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void InitDispClock(void){
	DrawRectangle(clock_x0, clock_y0, clock_x0+62, clock_y0+63, 1, 0);
	DrawRectangle(temp_x0, temp_y0, temp_x0+63, temp_y0+31, 1, 0);
	DrawRectangle(humi_x0, humi_y0, humi_x0+63, humi_y0+30, 1, 0);
	DrawDot(clock_x0+2, clock_y0+2, 1);
	DrawDot(clock_x0+2, clock_y0+61, 1);

	//時計文字盤描画
	DrawCircle(clock_x0+clock_centerx, clock_y0+clock_centery, 30, 1, 0); //外枠
	DrawLine(clock_centerx, clock_centery, clock_centerx+26, clock_centery-15,1);
	DrawLine(clock_centerx, clock_centery, clock_centerx+15, clock_centery-26,1);
	DrawLine(clock_centerx, clock_centery, clock_centerx+26, clock_centery+15,1);
	DrawLine(clock_centerx, clock_centery, clock_centerx+15, clock_centery+26,1);
	DrawLine(clock_centerx, clock_centery, clock_centerx-26, clock_centery-15,1);
	DrawLine(clock_centerx, clock_centery, clock_centerx-15, clock_centery-26,1);
	DrawLine(clock_centerx, clock_centery, clock_centerx-26, clock_centery+15,1);
	DrawLine(clock_centerx, clock_centery, clock_centerx-15, clock_centery+26,1);
	DrawLine(clock_centerx, clock_centery, clock_centerx, clock_centery-30,1);
	DrawLine(clock_centerx, clock_centery, clock_centerx+30, clock_centery,1);
	DrawLine(clock_centerx, clock_centery, clock_centerx, clock_centery+30,1);
	DrawLine(clock_centerx, clock_centery, clock_centerx-30, clock_centery,1);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/