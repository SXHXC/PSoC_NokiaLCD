/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include "string.h"
#include "display.h"
#include "bitmap.h"

#define HBAR_V_LIMIT 48
#define HBAR_I_LIMIT 74

#define VBAR_V_LIMIT 20
#define VBAR_I_LIMIT 740


int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
int i,i1,i2;
	
  LcdInit(back1);
  LcdContrast(0x39);
  /* first screen */
  LcdGoto(5,1);
  LcdStr("Cypress",DRAW_OR,Middle_Front);
  LcdGoto(5,2);
  LcdStr("PSoC5LP & LCD",DRAW_XOR,Middle_Front);
  LcdGoto(8,3);
  LcdStr("interfacing",DRAW_OR,Middle_Front);
  LcdGoto(20,4);
  LcdStr("example",DRAW_XOR,Middle_Front);
 
  CyDelay(2000);
  /* Fonts Demo Screen */
  LcdClear();
  LcdGoto(2,1);
  LcdBigStr("Big Front" , DRAW_XOR);
  LcdGoto(2,3);
  LcdStr("Middle Font",DRAW_XOR,Middle_Front);
  LcdGoto(2,4);
  LcdStr("Small Font",DRAW_XOR,Small_Front);

  
CyDelay(2000);
  /* XOR draw Over Background example screen */
  LcdSetBackground(back2);
  LcdClear();
  //LcdBigCStr(5,1,"XOR DRAW",DRAW_XOR);  
  LcdGoto(10,1);
  LcdStr("OVER",DRAW_XOR,Middle_Front);  
  LcdGoto(10,2);
  LcdStr("BACKGROUND",DRAW_XOR,Middle_Front);  

CyDelay(2000);
  /* Horizontal bargraphs example screen */
  LcdSetBackground(back1);
  LcdClear();
  LcdGoto(4,1);
  LcdChr('V',DRAW_OR,Middle_Front);
  LcdGoto(4,3);
  LcdChr('I',DRAW_OR,Middle_Front);
for (i=0;i<16;i++)
  { 
    LcdHBargraph(1,15,79,i,DRAW_OR);
    for (i1=0;i1<HBAR_V_LIMIT;i1++)
    {   
      LcdHBargraph(3,15,79,i1,DRAW_OR);
      for (i2=0;i2<HBAR_I_LIMIT;i2++);
    }
  };      

  /* Vertical bargraphs example screen */
  LcdClear();
  LcdGoto(10,1);
  LcdChr('V',DRAW_OR,Middle_Front);
  LcdGoto(30,1);
  LcdChr('I',DRAW_OR,Middle_Front);
  for (i=0;i<10;i++)
 { 
    LcdVBargraph(10,2,4,i,DRAW_OR);
    for (i1=0;i1<VBAR_V_LIMIT;i1++)
    {   
      LcdVBargraph(30,2,4,i1,DRAW_OR);
      for (i2=0;i2<VBAR_I_LIMIT;i2++);
    }
  };      



  /* Bitmap draw example screen */
  LcdClear();
  LcdImage(11,1,62,4,logo);
  CyDelay(2000);
  /* Line draw example screen */
  LcdSetBackground(gauge);
  LcdClear();
    

    for(;;)
    {    
    LcdLine (6,15,42,45,DRAW_OR);
    CyDelay(500); // delay
    LcdLine (6,15,42,45,DRAW_CLEAR);
    LcdLine (7,14,42,45,DRAW_OR);
    CyDelay(500); // delay
    LcdLine (7,14,42,45,DRAW_CLEAR);
    LcdLine (9,14,42,45,DRAW_OR);
    CyDelay(500); // delay
    LcdLine (9,14,42,45,DRAW_CLEAR);
    LcdLine (10,12,42,45,DRAW_OR);
    CyDelay(500); // delay
    LcdLine (10,12,42,45,DRAW_CLEAR);
    LcdLine (26,6,42,45,DRAW_OR);
    CyDelay(500); // delay
    LcdLine (26,6,42,45,DRAW_CLEAR);
    LcdLine (43,4,42,45,DRAW_OR);
    CyDelay(500); // delay
    LcdLine (43,4,42,45,DRAW_CLEAR);/* Place your application code here. */
    }
}

/* [] END OF FILE */
