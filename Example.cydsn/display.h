#ifndef _DISPLAY_H_
#define _DISPLAY_H_

//-------------------------------------------------------------------------------------------------- 
// Compiler directive that allow using backgroung under painting
// To disable this feature comment it
//--------------------------------------------------------------------------------------------------
#define DRAW_OVER_BACKGROUND

//--------------------------------------------------------------------------------------------------
// Port 0 pinout for LCD.
//--------------------------------------------------------------------------------------------------
#define LCD_DC_PIN 0x08 // PRT0_3
#define LCD_RST_PIN 0x02 // PRT0_1

//--------------------------------------------------------------------------------------------------
// Include font files
//--------------------------------------------------------------------------------------------------
#include "fonts.h"
//--------------------------------------------------------------------------------------------------
// Background variable
//--------------------------------------------------------------------------------------------------
#ifdef DRAW_OVER_BACKGROUND
 const char *Background;
typedef enum
{
  Small_Front = 0,
  Middle_Front = 1
} front_type;

typedef enum
{
  DRAW_OR = 0,
  DRAW_XOR = 1,
  DRAW_CLEAR = 2
} draw_type;

typedef enum {
	Invert_Yes,
	Invert_No
} Invert_t;


#endif // DRAW_OVER_BACKGROUND


//--------------------------------------------------------------------------------------------------
// Public function prototypes
//--------------------------------------------------------------------------------------------------
void LcdClear (void);
void LcdContrast (char contrast);
void LcdGoto (char x, char y);
void LcdImage (char x, char y, char xsize, char ysize, const char * dataPtr);
void LcdSendData (char data);
void LcdSendCommand (char command);

#ifdef DRAW_OVER_BACKGROUND
void LcdInit (const char * dataPtr);
void LcdSetBackground (const char * dataPtr);
void LcdChr (char ch, draw_type dt, front_type ft);
void LcdStr (char *dataPtr, draw_type dt, front_type ft);
void LcdBigStr(char *dataPtr, draw_type dtype);
void LcdVBargraph (char x, char ystart, char yend, char yposition, draw_type dt);
void LcdHBargraph (char y, char xstart, char xend, char xposition, draw_type dt);
void LcdLine (char xb, char yb, char xe, char ye, draw_type dt);
#else  // DRAW_OVER_BACKGROUND
void LcdInit (void);
void LcdChr (char ch, front_type ft);
void LcdStr (char *dataPtr, front_type ft);
void LcdVBargraph (char x, char ystart, char yend, char yposition);
void LcdHBargraph (char y, char xstart, char xend, char xposition);
void LcdLine (char xb, char yb, char xe, char ye);

#endif // DRAW_OVER_BACKGROUND

#endif // _DISPLAY_H_