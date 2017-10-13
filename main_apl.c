/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名： main_apl.c
＊　責務　　　： ユーザmain関数の定義
＊　作成日　　： 2017/10/1
＊　作成者　　： kern-gt

＊　コンパイラ：CC-RX

＊　備考：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
/**----------------------------------------------------------------------------
<<利用ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
/*#include "r_cg_macrodriver.h"*/
/*#include "r_cg_userdefine.h"*/
#include <stdio.h>
#include "iodefine.h"

/* Kernel includes. */
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/queue.h"

/* アプリケーション */
#include "oled.h"
#include "rtc.h"

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "main_apl.h"

/***タスク定義*******************************************************************/
void LedBlink2Hz(void *pvParameters)
{
	while(1) {
		PORTD.PODR.BIT.B0 = ~PORTD.PODR.BIT.B0;
		vTaskDelay(250/portTICK_PERIOD_MS);
	}
}

void LedBlink1Hz(void *pvParameters)
{
	while(1) {
		PORTD.PODR.BIT.B1 = ~PORTD.PODR.BIT.B1;
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}

void vTask3(void *pvParameters){
	DrawTask();
}

void vTask4(void *pvParameters){
	RtcDriverTask();
}

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　： MainApplication
＊　機能　　： ユーザmain関数
＊　引数　　： 
＊　戻り値　： 
＊　備考　　： 呼び出し元は生成コードのr_cg_main.c/void main(void);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void MainApplication(void)
{
	/*タスク生成*/
	//xTaskCreate(LedBlink2Hz,"LedBlink2Hz",100,NULL,2,NULL);
	//xTaskCreate(LedBlink1Hz,"LedBlink1Hz",100,NULL,2,NULL);
	xTaskCreate(vTask3,"Task3",100,NULL,1,NULL);
	//xTaskCreate(vTask4,"Task4",100,NULL,1,NULL);
	
	/* タスクスケジューラ起動*/
	vTaskStartScheduler();

	/*ここには戻ってこないはず*/
	while(1);
}