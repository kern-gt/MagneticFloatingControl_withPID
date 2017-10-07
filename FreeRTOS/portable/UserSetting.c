/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名： UserSetting.c
＊　責務　　　： ユーザ依存関数定義
＊　作成日　　： 2017/10/4
＊　作成者　　： kern-gt

＊　コンパイラ：CC-RX

＊　備考：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/

/**----------------------------------------------------------------------------
<<利用ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
/* Kernel includes. */
#include "../FreeRTOS.h"

/***configASSERT()定義*******************************************************************/
void vAssertCalled( void )
{
	volatile unsigned long ul = 0;

	taskENTER_CRITICAL();
	{
		/* Use the debugger to set ul to a non-zero value in order to step out
		of this function to determine why it was called. */
		while( ul == 0 )
		{
			portNOP();
		}
	}
	taskEXIT_CRITICAL();
}

/***割込み優先度の注意事項*******************************************************************/
/*
	 割込み処理の中でシステムコール、つまりRTOSの機能を使用する場合は、その割込みの優先度に
	気をつけなければならない。ただし、ここで言う割り込み処理とはユーザが用いるものであり、
	下記に示すICU_SWINT、CMT0_CMI0のようなFreeRTOS側が使用するものではない。

	  configMAX_SYSCALL_INTERRUPT_PRIORITYがシステムコールを利用できる割込みハンドラの最大
	優先度で、これ以上はOS管理外割込みとなる。管理外とはFreeRTOSが全く干渉しない優先度で、
	最も高速に処理できるが、システムコールが使用できない。
*/
/***ディスパッチャ割込みの注意事項*******************************************************************/
/*
	 ディスパッチャ割込み（ソフトウェア割込み）にはICU_SWINT（VECT=27）
	を使用している。割込み優先度はconfigKERNEL_INTERRUPT_PRIORITYである。

	 この割込みはタスク切り替え処理を行う。割込み処理のほうがタスクよりも優先度が高い。
	すべての割込み処理を終えてからタスク切り替えを行う遅延ディスパッチを実現するべく、
	この割込みの優先度（configKERNEL_INTERRUPT_PRIORITY）はもっとも低く設定しなければならない。
	また、この割込みハンドラの設定はFreeRTOS/portable/port.c、port_asm.srcにあるため、
	コード生成ツールを使用する際はICU_SWINTを使用しないようにする。

*/
/***カーネルタイマ初期設定*******************************************************************/
/*
	 カーネルタイマ（OSタイマ）にはCMT0_CMI0を使用している。
	割込み優先度はconfigKERNEL_INTERRUPT_PRIORITYである。

	 スケジューラ起動時にこのコールバック関数が呼ばれタイマの周期設定後、タイマを起動する。
	また、FreeRTOS/portable/port.cにOStickハンドラ(vTickISR())としてCMT0_CMI0の割込みハンドラを
	定義しているので、コード生成ツールを使用する場合はCMT0_CMI0を使用しないようにする。
	以下の関数はデモ用のサンプルを参考にOStickの周波数領域を拡張
	する改良を行った。
	 具体的にはデモコードはconfigTICK_RATE_HZ=114Hz以下の設定はCMT0_CMI0の16bitタイマを
	オーバーフローするのでCMT0の分周器を変更するようにしてある。
	これにより、configTICK_RATE_HZ＝30Hzまでの設定が可能である。
	ただし、configPERIPHERAL_CLOCK_HZ=PCLKBクロック=60MHzであること
	を前提としている。
	
	 The RX port uses this callback function to configure its tick interrupt.
	This allows the application to choose the tick interrupt source. 
*/
void vApplicationSetupTimerInterrupt( void )
{
	const uint32_t ulEnableRegisterWrite = 0xA50BUL, ulDisableRegisterWrite = 0xA500UL;

	/* Disable register write protection. */
	SYSTEM.PRCR.WORD = ulEnableRegisterWrite;

	/* Enable compare match timer 0. */
	MSTP( CMT0 ) = 0;

	/* Interrupt on compare match. */
	CMT0.CMCR.BIT.CMIE = 1;

	/* Set the compare match value. */
	if(configTICK_RATE_HZ > 114){
		/* Divide the PCLK by 8. */
		CMT0.CMCR.BIT.CKS = 0;
		CMT0.CMCOR = ( unsigned short ) ( ( ( configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ ) -1 ) / 8 );
	}else  if(configTICK_RATE_HZ > 29){
		/* Divide the PCLK by 32. */
		CMT0.CMCR.BIT.CKS = 1;
		CMT0.CMCOR = ( unsigned short ) ( ( ( configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ ) -1 ) / 32 );
	}else{
		/*configTICK_RATE_HZ is very small.*/
		while(1);
	}

	/* Enable the interrupt... */
	_IEN( _CMT0_CMI0 ) = 1;

	/* ...and set its priority to the application defined kernel priority. */
	_IPR( _CMT0_CMI0 ) = configKERNEL_INTERRUPT_PRIORITY;

	/* Start the timer. */
	CMT.CMSTR0.BIT.STR0 = 1;

    /* Reneable register protection. */
    SYSTEM.PRCR.WORD = ulDisableRegisterWrite;
}