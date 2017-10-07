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

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "main_apl.h"

/***タスク定義*******************************************************************/
void vTask1(void *pvParameters)
{
	while(1) {
		PORTD.PODR.BIT.B0 = ~PORTD.PODR.BIT.B0;
		vTaskDelay(125/portTICK_PERIOD_MS);
	}
}

void vTask2(void *pvParameters)
{
	while(1) {
		PORTD.PODR.BIT.B1 = ~PORTD.PODR.BIT.B1;
		vTaskDelay(250/portTICK_PERIOD_MS);
	}
}

void vTask3(void *pvParameters)
{
	while(1) {
		PORTD.PODR.BIT.B2 = ~PORTD.PODR.BIT.B2;
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}

void vTask4(void *pvParameters)
{
	while(1) {
		PORTD.PODR.BIT.B3 = ~PORTD.PODR.BIT.B3;
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
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
	xTaskCreate(vTask1,"Task1",100,NULL,1,NULL);
	xTaskCreate(vTask2,"Task2",100,NULL,1,NULL);
	xTaskCreate(vTask3,"Task3",100,NULL,1,NULL);
	xTaskCreate(vTask4,"Task4",100,NULL,1,NULL);
	
	/* タスクスケジューラ起動*/
	vTaskStartScheduler();

	/*ここには戻ってこないはず*/
	while(1);
}