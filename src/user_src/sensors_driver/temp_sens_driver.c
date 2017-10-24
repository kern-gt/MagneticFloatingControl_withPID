/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： temp_sens_driver.c
＊　責務　　　：
＊　作成日　　： 2017/10/13
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
#include "i2c_driver.h"
#include "hdc1000.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "temp_sens_driver_in.h"

/**----------------------------------------------------------------------------
<<変数>>
-----------------------------------------------------------------------------**/
/**/
#define TEMP_SENSOR_ADDR (HDC1000_ADDR)
static xTaskHandle mytask_id;
static TempHumiData temp_humi_data;
static I2cRetData_t i2c_ret;

/***公開関数*******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void TempSensTask(void){
	InitTempSens();

	while(1){
		vTaskDelay(10000/portTICK_PERIOD_MS);
		TempSensRead();
	}
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
float ReadTemperature(void){
	return(temp_humi_data.temp);
}

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
float ReadHumidity(void){
	return(temp_humi_data.humi);
}


/***非公開関数******************************************************************/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void InitTempSens(void){
	uint8_t regi_addr;
	Hdc1000Config init_data;        //
	I2cData_t i2c_data;

	temp_humi_data.temp = 25.678f;
	temp_humi_data.humi = 0.0f;
	temp_humi_data.count = 0U;

	//
	mytask_id = xTaskGetCurrentTaskHandle();

	//初期化
	regi_addr = kConfiguration;
	init_data.word[0] = 0x10;
	init_data.word[1] = 0x00;

	i2c_data.callbackfunc = TempDeviceCallBack;
	i2c_data.p_data1st = &regi_addr;
	i2c_data.p_data2nd = init_data.word;
	i2c_data.cnt1st    = sizeof(regi_addr);
	i2c_data.cnt2nd    = sizeof(init_data.word);
	i2c_data.slave_addr= TEMP_SENSOR_ADDR;
	i2c_data.mast_slv_r_w = kI2cMasterSend;

	I2cWrite(&i2c_data);
	vTaskSuspend(mytask_id);

	switch(i2c_ret.ret){
		case kI2cSuccess:
			break;

		case kI2cError:
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
static void TempSensRead(void){
	uint8_t regi_addr;
	uint8_t data[4];        //
	I2cData_t i2c_data;

	//センサ値起動トリガ
	regi_addr = kTemperature;

	i2c_data.callbackfunc = TempDeviceCallBack;
	i2c_data.p_data1st = FIT_NO_PTR;
	i2c_data.p_data2nd = &regi_addr;
	i2c_data.cnt1st    = 0;
	i2c_data.cnt2nd    = sizeof(regi_addr);
	i2c_data.slave_addr= TEMP_SENSOR_ADDR;
	i2c_data.mast_slv_r_w = kI2cMasterSend;

	I2cWrite(&i2c_data);
	vTaskSuspend(mytask_id);

	switch(i2c_ret.ret){
		case kI2cSuccess:
			break;

		case kI2cError:
			break;

		default:
			break;
	}

	vTaskDelay(30/portTICK_PERIOD_MS);

	//温度・湿度を読み込み
	i2c_data.callbackfunc = TempDeviceCallBack;
	i2c_data.p_data1st = FIT_NO_PTR;
	i2c_data.p_data2nd = data;
	i2c_data.cnt1st    = 0;
	i2c_data.cnt2nd    = sizeof(data);
	i2c_data.slave_addr= TEMP_SENSOR_ADDR;
	i2c_data.mast_slv_r_w = kI2cMasterReceive;

	I2cWrite(&i2c_data);
	vTaskSuspend(mytask_id);

	switch(i2c_ret.ret){
		case kI2cSuccess:
			break;

		case kI2cError:
			break;

		default:
			break;
	}

	ConvertSensorVal(&temp_humi_data, data);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void ConvertSensorVal(TempHumiData *temp_humi_p, uint8_t *data){
	uint16_t puretemp = 0, purehumi = 0;
	float temp_f, humi_f;

	//エンディアンを反転
	puretemp = (uint16_t)data[0] <<8;
	puretemp = puretemp | (uint16_t)data[1];
	purehumi = (uint16_t)data[2] <<8;
	purehumi = purehumi | (uint16_t)data[3];

	temp_f = ((float)puretemp)/((float)0x10000);
	temp_humi_p->temp = (temp_f * 165.0f) - 40.0f;
	humi_f = (float)purehumi;
	temp_humi_p->humi = humi_f * 100.0f/65536.0f;
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
static void TempDeviceCallBack(uint8_t ret){
	i2c_ret.ret = ret;
	vTaskResume(mytask_id);
}


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　関数名　：
＊　機能　　：
＊　引数　　：
＊　戻り値　：
＊　備考　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/