/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： i2c_driver.h
＊　責務　　　：
＊　作成日　　： 2017/10/14
＊　作成者　　：　kern-gt

＊　備考　  ：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
#ifndef _I2C_DRIVER_H_
#define _I2C_DRIVER_H_
/**----------------------------------------------------------------------------
<<外部公開マクロ定義>>
-----------------------------------------------------------------------------**/


/**----------------------------------------------------------------------------
<<外部公開型定義>>
-----------------------------------------------------------------------------**/
typedef void* QueueID;

typedef enum
{
	kI2cMasterSend = 0,
	kI2cMasterReceive,
	kI2cSlaveSend,
	kI2cSlaveReceive,
}I2cCommuniTypeEnum;

typedef struct
{
	QueueID   que_id;
	uint8_t*  p_data1st;
	uint8_t*  p_data2nd;
	uint32_t  cnt1st;
	uint32_t  cnt2nd;
	uint8_t   slave_addr;
	uint8_t   mast_slv_r_w;	//I2cCommuniTypeEnum
	uint8_t   dummy[2];     //アライメント調整
}I2cData_t;

typedef enum
{
	kI2cSuccess = 0,
	kI2cError
}I2cErrorEnum;

typedef struct
{
	uint8_t ret;
	uint8_t dummy[3];
}I2cRetData_t;
/**----------------------------------------------------------------------------
<<外部公開プロトタイプ定義>>
-----------------------------------------------------------------------------**/
/*extern*/
extern void I2cDrivTask(void);

#endif /* _I2C_DRIVER_H_ */