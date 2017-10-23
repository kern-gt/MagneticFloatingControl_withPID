/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： draw_data.c
＊　責務　　　：
＊　作成日　　： 2017/10/8
＊　作成者　　：　kern-gt

＊　コンパイラ　：CC-RX

＊　備考　　　：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/

/**----------------------------------------------------------------------------
<<利用ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
/*#include "r_cg_macrodriver.h"*/
/*#include "r_cg_userdefine.h"*/
#include <stdint.h>
/**----------------------------------------------------------------------------
<<自ファイルのヘッダ>>
-----------------------------------------------------------------------------**/
#include "draw_data.h"

/**----------------------------------------------------------------------------
<<変数>>
-----------------------------------------------------------------------------**/
const uint8_t test_font[32] =		{0x07,0x01,0x01,0x18,0x08,0x00,0x00,0x80,0x80,0x00,0x00,0x08,0x18,0x01,0x01,0x07,0xE0,0x80,0x80,0x18,0x10,0x00,0x01,0x03,0x03,0x01,0x00,0x10,0x18,0x80,0x80,0xE0};
const uint8_t test2_font[32] =		{0x00,0x00,0x00,0x03,0x01,0x00,0x00,0x80,0x80,0x00,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x80,0x00,0x01,0x03,0x03,0x01,0x00,0x80,0xC0,0x00,0x00,0x00};
const uint8_t number_font[10][32] =	{
									{0x00,0x00,0x00,0xE0,0x38,0x04,0x02,0x02,0x02,0x02,0x04,0x38,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x1C,0x20,0x40,0x40,0x40,0x40,0x20,0x1C,0x07,0x00,0x00,0x00},
									{0x00,0x00,0x00,0x00,0x10,0x08,0x04,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x7F,0x40,0x40,0x00,0x00,0x00,0x00,0x00,0x00},
									{0x00,0x00,0x00,0x18,0x0C,0x06,0x02,0x02,0x02,0x02,0x82,0xC4,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x60,0x70,0x58,0x4C,0x46,0x43,0x41,0x40,0x40,0x00,0x00,0x00},
									{0x00,0x00,0x00,0x08,0x0C,0x84,0x86,0x82,0x82,0x82,0x82,0x44,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x60,0x20,0x31,0x1E,0x00,0x00,0x00},
									{0x00,0x00,0x00,0x80,0xC0,0x60,0x30,0x18,0x0C,0x06,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x05,0x04,0x04,0x04,0x04,0x44,0x44,0x7F,0x44,0x44,0x04,0x00,0x00},
									{0x00,0x00,0x00,0xFE,0x82,0x42,0x42,0x42,0x42,0x42,0x42,0x82,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x41,0x40,0x40,0x40,0x40,0x60,0x20,0x30,0x18,0x0F,0x00,0x00,0x00},
									{0x00,0x00,0x00,0xC0,0x70,0x98,0x4C,0x44,0x46,0x42,0x82,0x82,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x39,0x20,0x40,0x40,0x40,0x40,0x20,0x31,0x1F,0x00,0x00,0x00},
									{0x00,0x00,0x00,0x1E,0x02,0x02,0x02,0x02,0x02,0x82,0xE2,0x3A,0x0E,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x1E,0x03,0x00,0x00,0x00,0x00,0x00,0x00},
									{0x00,0x00,0x00,0x38,0x4C,0x84,0x82,0x82,0x82,0x82,0x84,0x4C,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x31,0x20,0x40,0x40,0x40,0x40,0x20,0x31,0x1E,0x00,0x00,0x00},
									{0x00,0x00,0x00,0xF8,0x8C,0x04,0x02,0x02,0x02,0x02,0x04,0x9C,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x41,0x41,0x42,0x62,0x22,0x32,0x19,0x0E,0x03,0x00,0x00,0x00}
									};
const uint8_t colon_font[32] =  	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t percent_font[32] =	{0x00,0x7C,0x82,0x82,0x82,0x7C,0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x00,0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x01,0x00,0x00,0x3E,0x41,0x41,0x41,0x3E,0x00};
const uint8_t celsius_font[32] = 	{0x0C,0x12,0x12,0x0C,0xE0,0x38,0x0C,0x04,0x02,0x02,0x02,0x02,0x04,0x08,0x3E,0x00,0x00,0x00,0x00,0x00,0x03,0x0E,0x18,0x10,0x20,0x20,0x20,0x20,0x10,0x18,0x0C,0x00};
const uint8_t dot_font[32] = 		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};