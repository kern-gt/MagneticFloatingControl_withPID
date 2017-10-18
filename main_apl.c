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
#include "rtc_driver.h"
#include "temp_sens_driver.h"
#include "i2c_driver.h"

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "main_apl.h"

/**----------------------------------------------------------------------------
<<変数>>
-----------------------------------------------------------------------------**/
xQueueHandle sci0_iic_queue, rtc_que, temp_sens_que;

//static xTaskHandle task1_id, task2_id;

/***タスク定義*******************************************************************/
void LedBlink2Hz(void *pvParameters)
{
	//task1_id = xTaskGetCurrentTaskHandle();
	
	vTaskDelay(1000/portTICK_PERIOD_MS);
	//vTaskSuspend(NULL);
	//vTaskSuspend(task2_id);
	while(1) {
		PORTD.PODR.BIT.B0 = ~PORTD.PODR.BIT.B0;
		vTaskDelay(500/portTICK_PERIOD_MS);
		//vTaskResume(task2_id);
		vTaskDelay(100/portTICK_PERIOD_MS);
		vTaskSuspend(NULL);
	}
}

void LedBlink1Hz(void *pvParameters)
{
	//task2_id = xTaskGetCurrentTaskHandle();
	//vTaskDelay(250/portTICK_PERIOD_MS);
	vTaskSuspend(NULL);
	while(1) {
		PORTD.PODR.BIT.B1 = ~PORTD.PODR.BIT.B1;
		vTaskDelay(500/portTICK_PERIOD_MS);
		//vTaskResume(task1_id);
		vTaskSuspend(NULL);
	}
}

void vTask3(void *pvParameters){
	DrawTask();
}

void vTask4(void *pvParameters){
	RtcDriverTask();
}

void vTask5(void *pvParameters){
	I2cDrivTask();
}

void vTask6(void *pvParameters){
	TempSensTask();
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
	/*キュー生成*/
	sci0_iic_queue = xQueueCreate(4, 24);
	rtc_que        = xQueueCreate(1,4);
	temp_sens_que  = xQueueCreate(1,4);


	/*タスク生成*/
	//xTaskCreate(LedBlink2Hz,"LedBlink2Hz",100,NULL,1,NULL);
	//xTaskCreate(LedBlink1Hz,"LedBlink1Hz",100,NULL,4,NULL);
	xTaskCreate(vTask3,"Task3",100,NULL,1,NULL);
	xTaskCreate(vTask4,"Task4",100,NULL,1,NULL);
	xTaskCreate(vTask5,"Task5",100,NULL,1,NULL);
	xTaskCreate(vTask6,"Task6",100,NULL,1,NULL);
	
	/* タスクスケジューラ起動*/
	vTaskStartScheduler();

	/*ここには戻ってこないはず*/
	while(1);
}