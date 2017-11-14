/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： i2c_driver.c
＊　責務　　　：
＊　作成日　　： 2017/10/14
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
#include "r_sci_iic_rx_if.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "i2c_driver_in.h"

/**----------------------------------------------------------------------------
<<変数>>
-----------------------------------------------------------------------------**/
static sci_iic_info_t siic_info_0;        //sci0_iic
static xTaskHandle i2c_driv_tsk_id;
extern xQueueHandle sci0_iic_queue;
/***公開関数*******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void I2cDrivTask(void){
	I2cData_t i2c_data;

	InitI2c();

	while(1){
		xQueueReceive(sci0_iic_queue, &i2c_data, portMAX_DELAY);

		I2cCommunication(&i2c_data);

		I2cCommuniReturn(&i2c_data);
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void I2cWrite(I2cData_t *i2c_data){
	xQueueSend(sci0_iic_queue, i2c_data, portMAX_DELAY);
}


/***非公開関数******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void InitI2c(void){
	volatile sci_iic_return_t ret;           //i2cAPI戻り値

	//自タスクID取得
	i2c_driv_tsk_id = xTaskGetCurrentTaskHandle();

	/*I2C初期化*****************************************/
	siic_info_0.callbackfunc = &CallBackMaster;
	siic_info_0.dev_sts = SCI_IIC_NO_INIT;
	siic_info_0.ch_no   = 0;

	ret = R_SCI_IIC_Open(&siic_info_0);

	switch(ret){
		case SCI_IIC_SUCCESS:
			break;

		case SCI_IIC_ERR_LOCK_FUNC:
			break;

		case SCI_IIC_ERR_INVALID_CHAN:
			break;

		case SCI_IIC_ERR_INVALID_ARG:
			break;

		case SCI_IIC_ERR_NO_INIT:
			break;

		case SCI_IIC_ERR_BUS_BUSY:
			break;

		case SCI_IIC_ERR_OTHER:
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
static void CallBackMaster(void){

	//xTaskResumeFromISR(i2c_driv_tsk_id);
	vTaskResume(i2c_driv_tsk_id);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void I2cCommunication(I2cData_t *i2c_data){

	siic_info_0.p_slv_adr = &i2c_data->slave_addr;

	siic_info_0.p_data1st = i2c_data->p_data1st;
	siic_info_0.p_data2nd = i2c_data->p_data2nd;
	siic_info_0.cnt1st    = i2c_data->cnt1st;
	siic_info_0.cnt2nd    = i2c_data->cnt2nd;

	switch(i2c_data->mast_slv_r_w){
		case kI2cMasterSend:
			i2cMasterSend();
			break;

		case kI2cMasterReceive:
			i2cMasterReceive();
			break;
		
		case kI2cSlaveSend:
			while(1);
			break;

		case kI2cSlaveReceive:
			while(1);
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
static void I2cCommuniReturn(I2cData_t *data){
	i2c_callback CallBack;

	CallBack = data->callbackfunc;
	(*CallBack)((uint8_t)kI2cSuccess);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void i2cMasterSend(void)
{
	volatile sci_iic_return_t ret;           //i2cAPI戻り値

	ret = R_SCI_IIC_MasterSend(&siic_info_0);
	vTaskSuspend(i2c_driv_tsk_id);

	switch(ret){
		case SCI_IIC_SUCCESS:
			break;

		case SCI_IIC_ERR_LOCK_FUNC:
			break;

		case SCI_IIC_ERR_INVALID_CHAN:
			break;

		case SCI_IIC_ERR_INVALID_ARG:
			break;

		case SCI_IIC_ERR_NO_INIT:
			break;

		case SCI_IIC_ERR_BUS_BUSY:
			break;

		case SCI_IIC_ERR_OTHER:
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
static void i2cMasterReceive(void)
{
	volatile sci_iic_return_t ret;	//i2cAPI戻り値

	ret = R_SCI_IIC_MasterReceive(&siic_info_0);
	vTaskSuspend(NULL);

	switch(ret){
		case SCI_IIC_SUCCESS:
			break;

		case SCI_IIC_ERR_LOCK_FUNC:
			break;

		case SCI_IIC_ERR_INVALID_CHAN:
			break;

		case SCI_IIC_ERR_INVALID_ARG:
			break;

		case SCI_IIC_ERR_NO_INIT:
			break;

		case SCI_IIC_ERR_BUS_BUSY:
			break;

		case SCI_IIC_ERR_OTHER:
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

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/