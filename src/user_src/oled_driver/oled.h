/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
＊　ファイル名　： oled.h
＊　責務　　　：
＊　作成日　　： 2017/10/7
＊　作成者　　：　kern-gt

＊　備考　  ：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
#ifndef _OLED_H_
#define _OLED_H_

/**----------------------------------------------------------------------------
<<外部公開マクロ定義>>
-----------------------------------------------------------------------------**/


/**----------------------------------------------------------------------------
<<外部公開型定義>>
-----------------------------------------------------------------------------**/


/**----------------------------------------------------------------------------
<<外部公開プロトタイプ定義>>
-----------------------------------------------------------------------------**/
/*extern*/
extern void InitOled(void);
extern void DrawTask(void);
extern void DisplayOn(void);
extern void DisplayOff(void);

extern void DrawClear(void);
extern void DrawWhite(void);
extern void Bitblt(void);

extern void DrawDot(uint16_t x, uint16_t y, uint8_t color);
extern void DrawLine(uint16_t x_1, uint16_t y_1, uint16_t x_2, uint16_t y_2, uint8_t color);
extern void DrawRectangle(uint16_t x_rup, uint16_t y_rup, uint16_t x_ldwn, uint16_t y_ldwn, uint8_t color, uint8_t fill);
extern void DrawCircle(int16_t x, int16_t y, uint16_t r, uint8_t line_color, uint8_t fill);
extern void PasteImage1616(uint16_t x, uint16_t page_num, const uint8_t *data);
extern void PrintNumber(uint16_t x, uint8_t page_num, uint8_t num);
#endif /* _OLED_H_ */