#include <project.h>
#include "spi.h"
#include "display.h"

int Background_cursor,py,px;

//--------------------------------------------------------------------------------------------------
// Private function prototypes
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Name : LcdInit
// Description : Performs SPI & LCD controller initialization.
// Argument(s) : None if you don't use DRAW_OVER_BACKGROUND
//               background -> Pointer to background in another case
// Return value : None.
//--------------------------------------------------------------------------------------------------
#ifdef DRAW_OVER_BACKGROUND
void LcdInit (const char * background)
#else  //DRAW_OVER_BACKGROUND
void LcdInit ( void )
#endif //DRAW_OVER_BACKGROUND
{
  int i;
  //wait before clear reset
  RST_Write(1);

//  PRT0DR = LCD_RST_PIN;
  // Enable SPI 
  SPI_Start();
  LcdSendCommand(0x21); // LCD Extended Commands.
  LcdSendCommand(0xC0); // Set LCD Vop (Contrast).
  LcdSendCommand(0x06); // Set Temp coefficent.
  LcdSendCommand(0x13); // LCD bias mode 1:48.
  LcdSendCommand(0x20); // LCD Standard Commands, Horizontal addressing mode.
  LcdSendCommand(0x0C); // LCD in normal mode.
  //clear display
  LcdSendCommand(0x40);
  LcdSendCommand(0x80);

#ifdef DRAW_OVER_BACKGROUND //DRAW_OVER_BACKGROUND
    if (background == NULL){
   int16 n;
   LcdGoto(0,0);
   for (n=0;n<504;n++) {
background = 0x00;
LcdSendData(0x00);
}//84 pixel x 6 lineas=504 
}
    else{
  Background = background;
  for(i=0; i<504; i++) LcdSendData(Background[i]);  
  Background_cursor = 0;}
#else  						//DRAW_OVER_BACKGROUND
  for(i=0; i<504; i++) LcdSendData(0x00);
#endif 

}

//--------------------------------------------------------------------------------------------------
// Name : SetBackground
// Description : SetPointer to dispaly background.
// Argument(s) : dataPtr -> Pointer on aray that contains background.
// Return value : None.
// Note : Awailale only with DRAW_OVER_BACKGROUND directive
//--------------------------------------------------------------------------------------------------
#ifdef DRAW_OVER_BACKGROUND
void LcdSetBackground ( const char *dataPtr )
{
  Background = dataPtr;
}
#endif //DRAW_OVER_BACKGROUND

//--------------------------------------------------------------------------------------------------
// Name : LcdContrast
// Description : Set display contrast.
// Argument(s) : contrast -> Contrast value from 0x00 to 0x7F.
// Return value : None.
// Notes : No change visible at ambient temperature.
//--------------------------------------------------------------------------------------------------
void LcdContrast ( char contrast )
{
   if (contrast > 0x7f) {
    contrast = 0x7f;
     }
  // LCD Extended Commands.
  LcdSendCommand(0x20|0x01);
  // Set LCD Vop (Contrast).
  LcdSendCommand(0x80|contrast);
  // LCD Standard Commands, horizontal addressing mode.
  LcdSendCommand(0x20);
}

//--------------------------------------------------------------------------------------------------
// Name : LcdClear
// Description : Clears the display.
// Argument(s) : None.
// Return value : None.
//--------------------------------------------------------------------------------------------------
void LcdClear ( void )
{

  int i;
#ifdef DRAW_OVER_BACKGROUND
  LcdSendCommand(0x40);
  LcdSendCommand(0x80);
  for(i=0; i<504; i++) LcdSendData(Background[i]);  
#else  //DRAW_OVER_BACKGROUND
  for(i=0; i<504; i++) LcdSendData(0x00);
#endif //DRAW_OVER_BACKGROUND
  LcdSendCommand(0x40);
  LcdSendCommand(0x80);
}

//--------------------------------------------------------------------------------------------------
// Name : LcdGotoXY
// Description : Sets cursor location to xy location corresponding to basic font size.
// Argument(s) : x, y -> Coordinate for new cursor position. x range 0..83, y - 0..5
// Return value : None.
//--------------------------------------------------------------------------------------------------
void LcdGoto ( char x, char y )
{   px=x;
    py=y;
    LcdSendCommand(0x40|(y & 0x07));
    LcdSendCommand(0x80|(x & 0x7f));
#ifdef DRAW_OVER_BACKGROUND
  Background_cursor = y*84+x;
#endif //DRAW_OVER_BACKGROUND
}

//--------------------------------------------------------------------------------------------------
// Name : LcdChr
// Description : Displays a character at current cursor location and increment cursor location.
// Argument(s) : ch -> Character to write.
//               dt (available only with DRAW_OVER_BACKGROUND directive) -> drawing type  
// Return value : None.
//--------------------------------------------------------------------------------------------------
#ifdef DRAW_OVER_BACKGROUND
void LcdChr ( char ch, draw_type dtype, front_type ft )
{
    
  int i;
    switch(ft){
    case 0: // Small Front
  if (dtype == DRAW_OR)  
    for(i=0; i<3; i++, Background_cursor++) LcdSendData(SmallFront[ch-32][i] | Background[Background_cursor]);
  else if (dtype == DRAW_XOR)    
    for(i=0; i<3; i++, Background_cursor++) LcdSendData(SmallFront[ch-32][i] ^ Background[Background_cursor]);
  else // DRAW_CLEAR
    for(i=0; i<3; i++ ) LcdSendData(Background[Background_cursor]);
  LcdSendData(Background[Background_cursor]); // empty space after character 
    break;
   default: // Default Middle Front
  if (dtype == DRAW_OR)  
    for(i=0; i<5; i++, Background_cursor++) LcdSendData(MiddleFont[ch-32][i] | Background[Background_cursor]);
  else if (dtype == DRAW_XOR)    
    for(i=0; i<5; i++, Background_cursor++) LcdSendData(MiddleFont[ch-32][i] ^ Background[Background_cursor]);
  else // DRAW_CLEAR
    for(i=0; i<5; i++ ) LcdSendData(Background[Background_cursor]);
  LcdSendData(Background[Background_cursor]); // empty space after character 
    break;
    }
}
#else //DRAW_OVER_BACKGROUND
void LcdChr ( char ch , front_type ft )
{
  int i;
   switch(ft){
    case 0: // Small Front
  for(i=0; i<3; i++) LcdSendData(SmallFront[ch-32][i]);// empty space after character 
  LcdSendData(0x00);
    break;
default: // Default Middle Front
  for(i=0; i<5; i++) LcdSendData(MiddleFont[ch-32][i]);// empty space after character 
  LcdSendData(0x00);
    break;

}
}
#endif //DRAW_OVER_BACKGROUND

//--------------------------------------------------------------------------------------------------
// Name : LcdStr
// Description : Displays a character at current cursor location and increment cursor location
// according to font size.
// Argument(s) : dataPtr -> Pointer to null terminated ASCII string to display.
//               dt (available only with DRAW_OVER_BACKGROUND directive) -> drawing type  
// Return value : None.
//--------------------------------------------------------------------------------------------------
#ifdef DRAW_OVER_BACKGROUND
void LcdStr ( char *dataPtr, draw_type dtype, front_type ft )
{
  int i;
  while ( *dataPtr ) 
  {
    char ch = *dataPtr++; 
    switch(ft){
        case 0: // Small Front
    if (dtype == DRAW_OR)
      for(i=0; i<3; i++, Background_cursor++) LcdSendData(SmallFront[ch-32][i] | Background[Background_cursor]);
    else if (dtype == DRAW_XOR)
      for(i=0; i<3; i++, Background_cursor++) LcdSendData(SmallFront[ch-32][i] ^ Background[Background_cursor]);
    else
      for(i=0; i<3; i++, Background_cursor++) LcdSendData(Background[Background_cursor]);// empty space after character 
    LcdSendData(Background[Background_cursor++]);
    break;
    default: //Default front Middle
    if (dtype == DRAW_OR)
      for(i=0; i<5; i++, Background_cursor++) LcdSendData(MiddleFont[ch-32][i] | Background[Background_cursor]);
    else if (dtype == DRAW_XOR)
      for(i=0; i<5; i++, Background_cursor++) LcdSendData(MiddleFont[ch-32][i] ^ Background[Background_cursor]);
    else
      for(i=0; i<5; i++, Background_cursor++) LcdSendData(Background[Background_cursor]);// empty space after character 
    LcdSendData(Background[Background_cursor++]);
    break;
  }
}
}
#else //DRAW_OVER_BACKGROUND
void LcdStr ( char *dataPtr , front_type ft  )
{
  int i;
  while ( *dataPtr ) 
  {
    char ch = *dataPtr++; 
    switch(ft){
        case 0: // Small Front
    char ch = *dataPtr++; 
    for(i=0; i<5; i++) LcdSendData(FontLookup[ch-32][i]);
    // empty space after character 
    LcdSendData(0x00);
    default: // Default Middle Front
    char ch = *dataPtr++; 
    for(i=0; i<5; i++) LcdSendData(FontLookup[ch-32][i]);
    // empty space after character 
    LcdSendData(0x00);
    break;
    }
  }
}
#endif //DRAW_OVER_BACKGROUND
//--------------------------------------------------------------------------------------------------
// Name : LcdBigStr
// Description : Displays a character at current cursor location and increment cursor location
// according to font size.
// Argument(s) : dataPtr -> Pointer to null terminated ASCII string to display.
//               dt (available only with DRAW_OVER_BACKGROUND directive) -> drawing type  
// Return value : None.
//--------------------------------------------------------------------------------------------------
#ifdef DRAW_OVER_BACKGROUND
void LcdBigStr(char *dataPtr, draw_type dtype)
{
  char *dataPtrVar;
  int i;
  char ch;
  //draw upper half of text
  dataPtrVar = dataPtr;
  LcdSendCommand(0x40 | (py & 0x07));
  LcdSendCommand(0x80 | (px & 0x7f));
  Background_cursor = py*84+px;
  while ( *dataPtrVar ) 
  {
    ch = *dataPtrVar++; 
    if (dtype == DRAW_OR)
      for(i=0; i<8; i++,Background_cursor++) LcdSendData(BigFont[ch-32][i][0] | Background[Background_cursor]); 
    else if (dtype == DRAW_XOR)
      for(i=0; i<8; i++,Background_cursor++ ) LcdSendData(BigFont[ch-32][i][0] ^ Background[Background_cursor]); 
    else 
      for(i=0; i<8; i++,Background_cursor++ ) LcdSendData(Background[Background_cursor]); 
    LcdSendData(Background[Background_cursor]);
  };
  //draw lover half of text
  dataPtrVar = dataPtr;
  LcdSendCommand(0x40 | ((py+1)& 0x07));
  LcdSendCommand(0x80 | ( px& 0x7f));
  Background_cursor = (py+1)*84+px;
  while ( *dataPtrVar ) 
  {
    char ch = *dataPtrVar++; 
    if (dtype == DRAW_OR)
      for(i=0; i<8; i++,Background_cursor++) LcdSendData(BigFont[ch-32][i][1] | Background[Background_cursor]); 
    else if (dtype == DRAW_XOR)
      for(i=0; i<8; i++,Background_cursor++) LcdSendData(BigFont[ch-32][i][1] ^ Background[Background_cursor]); 
    else
      for(i=0; i<8; i++,Background_cursor++) LcdSendData(Background[Background_cursor]); 
    LcdSendData(Background[Background_cursor]);
  }
}
#else //DRAW_OVER_BACKGROUND
void LcdBigStr(char x, char y, char *dataPtr )
{
  char *dataPtrVar;
  int i;
  char ch;
  //draw upper half of text
  dataPtrVar = dataPtr;
  LcdSendCommand(0x40 | y);
  LcdSendCommand(0x80 | x);
  while ( *dataPtrVar ) 
  {
    ch = *dataPtrVar++; 
    for(i=0; i<8; i++) LcdSendData(BigFontLookup[ch-32][i][0]); 
    LcdSendData(0x00);
  };
  //draw lover half of text
  dataPtrVar = dataPtr;
  LcdSendCommand(0x40 | (y+1));
  LcdSendCommand(0x80 | x);
  while ( *dataPtrVar ) 
  {
    char ch = *dataPtrVar++; 
    for(i=0; i<8; i++) LcdSendData(BigFontLookup[ch-32][i][1]); 
    LcdSendData(0x00);
  }
}
#endif //DRAW_OVER_BACKGROUND

//--------------------------------------------------------------------------------------------------
// Name : LcdImage
// Description : DrawBitmap.
// Argument(s) : x -> Start x coord
//               y -> Start y coord
//               xsize -> Image x size
//               ysize -> Image y size
//               dataPrt -> pointer to linear array that contains bitmap	
// Return value : None.
//--------------------------------------------------------------------------------------------------
void LcdImage (char x, char y, char xsize, char ysize, const char * dataPtr)
{
  int i, j;
  for (j=0; j<ysize; j++)
  {
    // set start address
    LcdSendCommand(0x40 | (y+j));
    LcdSendCommand(0x80 | x);
    for (i=0; i<xsize; i++) LcdSendData(dataPtr[j*xsize+i]); 
  } 
#ifdef DRAW_OVER_BACKGROUND
  Background_cursor = (y+ysize)*84+x+xsize;
#else //DRAW_OVER_BACKGROUND
#endif //DRAW_OVER_BACKGROUND
}

//--------------------------------------------------------------------------------------------------
// Name : LcdVBargraph
// Description : Draw vertical Bargraph.
// Argument(s) : x -> bargraph's left coord
//               ystart -> Start y coord (by bank)
//               yend -> End y size (by bank, yend must be > ystart)
//               yposition -> bargraph position (by pixel, must  be <= (yend-ystart+1)*8)
//               dt (available only with DRAW_OVER_BACKGROUND directive) -> drawing type  
// Return value : None.
//--------------------------------------------------------------------------------------------------
#ifdef DRAW_OVER_BACKGROUND
void LcdVBargraph (char x, char ystart, char yend, char yposition, draw_type dt)
{
  char i,j,ch;
  LcdSendCommand(0x40 | yend);
  LcdSendCommand(0x80 | x);
  Background_cursor = yend*84+x;
  if (dt == DRAW_OR) 
    LcdSendData(0xFF);
  else if (dt == DRAW_XOR) 
    LcdSendData(0xFF ^ Background[Background_cursor++]);
  else   
    LcdSendData(Background[Background_cursor++]);
  for (i=0; i<6 ; i++, Background_cursor++) 
  { switch (yposition) 
    {
      case 0 : ch=0x80; break;
      case 1 : ch=0x80; break;
      case 2 : ch=0xC0; break;
      case 3 : ch=0xE0; break;
      case 4 : ch=0xF0; break;
      case 5 : ch=0xF8; break;
      case 6 : ch=0xFC; break;
      case 7 : ch=0xFE; break;
      default: ch=0xFF; break;
    };
    if (dt == DRAW_OR) 
      LcdSendData(ch | Background[Background_cursor]);
    else if (dt == DRAW_XOR) 
      LcdSendData(ch ^ Background[Background_cursor]);
    else   
      LcdSendData(Background[Background_cursor]);
  }
  if (dt == DRAW_OR) 
    LcdSendData(0xFF);
  else if (dt == DRAW_XOR) 
    LcdSendData(0xFF ^ Background[Background_cursor++]);
  else   
    LcdSendData(Background[Background_cursor++]);

  for (j=yend-1; j>ystart; j--)
  {
    if (yposition > 8) yposition -=8; else yposition=0;  
    LcdSendCommand(0x40 | j);
    LcdSendCommand(0x80 | x);
    Background_cursor = j*84+x;

    if (dt == DRAW_OR) 
      LcdSendData(0xFF);
    else if (dt == DRAW_XOR) 
      LcdSendData(0xFF ^ Background[Background_cursor++]);
    else   
      LcdSendData(Background[Background_cursor++]);
    for (i=0; i<6 ; i++, Background_cursor++) 
    { switch (yposition) 
      {
        case 0 : ch=0x00; break;
        case 1 : ch=0x80; break;
        case 2 : ch=0xC0; break;
        case 3 : ch=0xE0; break;
        case 4 : ch=0xF0; break;
        case 5 : ch=0xF8; break;
        case 6 : ch=0xFC; break;
        case 7 : ch=0xFE; break;
        default: ch=0xFF; break;
      };
      if (dt == DRAW_OR) 
        LcdSendData(ch | Background[Background_cursor]);
      else if (dt == DRAW_XOR) 
        LcdSendData(ch ^ Background[Background_cursor]);
      else   
        LcdSendData(Background[Background_cursor]);
    };
    if (dt == DRAW_OR) 
      LcdSendData(0xFF);
    else if (dt == DRAW_XOR) 
      LcdSendData(0xFF ^ Background[Background_cursor++]);
    else   
      LcdSendData(Background[Background_cursor++]);
  };
  if (yposition > 8) yposition -=8; else yposition=0;  
  LcdSendCommand(0x40 | ystart);
  LcdSendCommand(0x80 | x);
  Background_cursor = ystart*84+x;
  if (dt == DRAW_OR) 
    LcdSendData(0xFF);
  else if (dt == DRAW_XOR) 
    LcdSendData(0xFF ^ Background[Background_cursor++]);
  else   
    LcdSendData(Background[Background_cursor++]);
  for (i=0; i<6 ; i++, Background_cursor++) 
  { switch (yposition) 
    {
      case 0 : ch=0x01; break;
      case 1 : ch=0x81; break;
      case 2 : ch=0xC1; break;
      case 3 : ch=0xE1; break;
      case 4 : ch=0xF1; break;
      case 5 : ch=0xF9; break;
      case 6 : ch=0xFD; break;
      case 7 : ch=0xFF; break;
      default: ch=0xFF; break;
    };
    if (dt == DRAW_OR) 
      LcdSendData(ch | Background[Background_cursor]);
    else if (dt == DRAW_XOR) 
      LcdSendData(ch ^ Background[Background_cursor]);
    else   
      LcdSendData(Background[Background_cursor]);
  }
  if (dt == DRAW_OR) 
    LcdSendData(0xFF);
  else if (dt == DRAW_XOR) 
    LcdSendData(0xFF ^ Background[Background_cursor++]);
  else   
    LcdSendData(Background[Background_cursor++]);
}

#else //DRAW_OVER_BACKGROUND
void LcdVBargraph (char x, char ystart, char yend, char yposition)
{
  char i,j,ch;
  LcdSendCommand(0x40 | yend);
  LcdSendCommand(0x80 | x);
  LcdSendData(0xFF);
  for (i=0; i<6 ; i++, Background_cursor++) 
  { switch (yposition) 
    {
      case 0 : ch=0x80; break;
      case 1 : ch=0x80; break;
      case 2 : ch=0xC0; break;
      case 3 : ch=0xE0; break;
      case 4 : ch=0xF0; break;
      case 5 : ch=0xF8; break;
      case 6 : ch=0xFC; break;
      case 7 : ch=0xFE; break;
      default: ch=0xFF; break;
    };
    LcdSendData(ch);
  }
  LcdSendData(0xFF);
  for (j=yend-1; j>ystart; j--)
  {
    if (yposition > 8) yposition -=8; else yposition=0;  
    LcdSendCommand(0x40 | j);
    LcdSendCommand(0x80 | x);
    LcdSendData(0xFF);
    for (i=0; i<6 ; i++, Background_cursor++) 
    { switch (yposition) 
      {
        case 0 : ch=0x00; break;
        case 1 : ch=0x80; break;
        case 2 : ch=0xC0; break;
        case 3 : ch=0xE0; break;
        case 4 : ch=0xF0; break;
        case 5 : ch=0xF8; break;
        case 6 : ch=0xFC; break;
        case 7 : ch=0xFE; break;
        default: ch=0xFF; break;
      };
      LcdSendData(ch);
    };
    LcdSendData(0xFF);
  };
  if (yposition > 8) yposition -=8; else yposition=0;  
  LcdSendCommand(0x40 | ystart);
  LcdSendCommand(0x80 | x);
  LcdSendData(0xFF);
  for (i=0; i<6 ; i++, Background_cursor++) 
  { switch (yposition) 
    {
      case 0 : ch=0x01; break;
      case 1 : ch=0x81; break;
      case 2 : ch=0xC1; break;
      case 3 : ch=0xE1; break;
      case 4 : ch=0xF1; break;
      case 5 : ch=0xF9; break;
      case 6 : ch=0xFD; break;
      case 7 : ch=0xFF; break;
      default: ch=0xFF; break;
    };
    LcdSendData(ch);
  }
  LcdSendData(0xFF);
}
#endif //DRAW_OVER_BACKGROUND

/*--------------------------------------------------------------------------------------------------
// Name : LcdHBargraph
// Description : Draw vertical Bargraph.
// Argument(s) : y -> bargraph's top coord
//               xstart -> Start x coord 
//		         xend -> End x coord (xend must be > xstart)
//               xposition -> bargraph position (must  be xstart <= xposition <= xend)
//               dt (available only with DRAW_OVER_BACKGROUND directive) -> drawing type  
// Return value : None.
//--------------------------------------------------------------------------------------------------*/
#ifdef DRAW_OVER_BACKGROUND
void LcdHBargraph (char y, char xstart, char xend, char xposition, draw_type dt)
{
  int i;
  LcdSendCommand(0x40 | y);
  LcdSendCommand(0x80 | xstart);
  Background_cursor = y*84+xstart;
  if (dt == DRAW_OR) 
    LcdSendData(0x7E | Background[Background_cursor++]);
  else if (dt == DRAW_XOR) 
    LcdSendData(0x7E ^ Background[Background_cursor++]);
  else   
    LcdSendData(Background[Background_cursor++]);
  for (i=1; i<xposition; i++,Background_cursor++) 
    if (dt == DRAW_OR) 
      LcdSendData(0x7E | Background[Background_cursor]);
    else if (dt == DRAW_XOR) 
      LcdSendData(0x7E ^ Background[Background_cursor]);
    else   
      LcdSendData(Background[Background_cursor]);
  for (i=xposition+1; i<xend-xstart; i++,Background_cursor++) 
    if (dt == DRAW_OR) 
      LcdSendData(0x42 | Background[Background_cursor]);
    else if (dt == DRAW_XOR) 
      LcdSendData(0x42 ^ Background[Background_cursor]);
    else   
      LcdSendData(Background[Background_cursor]);
  if (dt == DRAW_OR) 
    LcdSendData(0x7E | Background[Background_cursor++]);
  else if (dt == DRAW_XOR) 
    LcdSendData(0x7E ^ Background[Background_cursor++]);
  else   
    LcdSendData(Background[Background_cursor++]);
}
#else //DRAW_OVER_BACKGROUND
void LcdHBargraph (char y, char xstart, char xend, char xposition)
{
  int i;
  LcdSendCommand(0x40 | y);
  LcdSendCommand(0x80 | xstart);
  LcdSendData(0x7E);
  for (i=1; i<=xposition; i++) LcdSendData(0x7E);
  for (i=xposition+1; i<xend-xstart; i++) LcdSendData(0x42);
  LcdSendData(0x7E);
}
#endif //DRAW_OVER_BACKGROUND


#ifdef DRAW_OVER_BACKGROUND
void LcdLine (char xb, char yb, char xe, char ye, draw_type dt)
#else //DRAW_OVER_BACKGROUND
void LcdLine (char xb, char yb, char xe, char ye)
#endif //DRAW_OVER_BACKGROUND
{
  char posx, posy, drwdata;
  int stepx, stepy, dx, dy, fraction;
  dy = ye - yb;
  dx = xe - xb;
  if (dy < 0)
  {
    dy = -dy;
    stepy = -1;
  }
  else stepy = 1; 
  if (dx < 0)
  {
    dx = -dx;
    stepx = -1;
  }
  else stepx = 1;
  dx <<= 1;
  dy <<= 1;

  posx = xb;
  posy = yb >> 3;
  switch(yb & 0x07)
  {
    case 0x00 : drwdata = 0x01; break;
    case 0x01 : drwdata = 0x02; break;
    case 0x02 : drwdata = 0x04; break;
    case 0x03 : drwdata = 0x08; break;
    case 0x04 : drwdata = 0x10; break;
    case 0x05 : drwdata = 0x20; break;
    case 0x06 : drwdata = 0x40; break;
    default : drwdata = 0x80;
  } 

  if (dx > dy)
  { 
    fraction = dy - (dx >> 1);
    while (xb != xe)
    {
      if (fraction >=0 )
      { 
        yb += stepy;
        fraction -= dx;
      }
      xb += stepx;
      fraction += dy;

      if (posx != xb || posy != yb >> 3)
      {
        LcdSendCommand(0x40 | posy);
        LcdSendCommand(0x80 | posx); 
#ifdef DRAW_OVER_BACKGROUND
        Background_cursor = posy*84+posx;
        if (dt == DRAW_OR) LcdSendData(drwdata | Background[Background_cursor++]);
        else if (dt == DRAW_XOR) LcdSendData(drwdata ^ Background[Background_cursor++]); 
        else LcdSendData(Background[Background_cursor++]); 
#else //DRAW_OVER_BACKGROUND
        LcdSendData(drwdata); 
#endif //DRAW_OVER_BACKGROUND
        posx = xb;
        posy = yb >> 3;
        switch(yb & 0x07)
        {
          case 0x00 : drwdata = 0x01; break;
          case 0x01 : drwdata = 0x02; break;
          case 0x02 : drwdata = 0x04; break;
          case 0x03 : drwdata = 0x08; break;
          case 0x04 : drwdata = 0x10; break;
          case 0x05 : drwdata = 0x20; break;
          case 0x06 : drwdata = 0x40; break;
         default : drwdata = 0x80;
        }
      }
      else
      {
        switch(yb & 0x07)
        {
          case 0x00 : drwdata |= 0x01; break;
          case 0x01 : drwdata |= 0x02; break;
          case 0x02 : drwdata |= 0x04; break;
          case 0x03 : drwdata |= 0x08; break;
          case 0x04 : drwdata |= 0x10; break;
          case 0x05 : drwdata |= 0x20; break;
          case 0x06 : drwdata |= 0x40; break;
         default : drwdata |= 0x80; break;
        }
      }
    }
  } 
  else 
  {
    fraction = dx - (dy >> 1);
    while (yb != ye)
    {
      if (fraction >=0 )
      {
        xb += stepx;
        fraction -= dy;
      }
      yb += stepy;
      fraction += dx;
      if (posx != xb || posy != yb >> 3)
      {
        LcdSendCommand(0x40 | posy);
        LcdSendCommand(0x80 | posx); 
#ifdef DRAW_OVER_BACKGROUND
        Background_cursor = posy*84+posx;
        if (dt == DRAW_OR) LcdSendData(drwdata | Background[Background_cursor++]);
        else if (dt == DRAW_XOR) LcdSendData(drwdata ^ Background[Background_cursor++]); 
        else LcdSendData(Background[Background_cursor++]); 
#else //DRAW_OVER_BACKGROUND
        LcdSendData(drwdata); 
#endif //DRAW_OVER_BACKGROUND
        posx = xb;
        posy = yb >> 3;
        switch(yb & 0x07)
        {
          case 0x00: drwdata = 0x01; break;
          case 0x01 : drwdata = 0x02; break;
          case 0x02 : drwdata = 0x04; break;
          case 0x03 : drwdata = 0x08; break;
          case 0x04 : drwdata = 0x10; break;
          case 0x05 : drwdata = 0x20; break;
          case 0x06 : drwdata = 0x40; break;
         default : drwdata = 0x80;
        }
      }
      else
      {
        switch(yb & 0x07)
        {
          case 0x00: drwdata |= 0x01; break;
          case 0x01 : drwdata |= 0x02; break;
          case 0x02 : drwdata |= 0x04; break;
          case 0x03 : drwdata |= 0x08; break;
          case 0x04 : drwdata |= 0x10; break;
          case 0x05 : drwdata |= 0x20; break;
          case 0x06 : drwdata |= 0x40; break;
         default : drwdata |= 0x80;
        }
      }
    }
  }
  LcdSendCommand(0x40 | posy);
  LcdSendCommand(0x80 | posx); 
#ifdef DRAW_OVER_BACKGROUND
  Background_cursor = posy*84+posx;
  if (dt == DRAW_OR) LcdSendData(drwdata | Background[Background_cursor++]);
  else if (dt == DRAW_XOR) LcdSendData(drwdata ^ Background[Background_cursor++]); 
  else LcdSendData(Background[Background_cursor++]); 
#else //DRAW_OVER_BACKGROUND
  LcdSendData(drwdata); 
#endif //DRAW_OVER_BACKGROUND
}

/*--------------------------------------------------------------------------------------------------
// Name : LcdSendData
// Description : Sends data to display controller.
// Argument(s) : data -> Data to be sent
// Return value : None.
//--------------------------------------------------------------------------------------------------*/
void LcdSendData ( char data )
{
    DC_Write(1);
    while ( ! (SPI_ReadTxStatus() & SPI_STS_SPI_IDLE) );
  // Send data to display controller.
  SPI_WriteTxData(data);

  // Wait until SPI cycle complete.
  while ( ! (SPI_ReadTxStatus() & SPI_STS_SPI_IDLE) );
    
}

/*--------------------------------------------------------------------------------------------------
// Name : LcdSendCommand
// Description : Sends command to display controller.
// Argument(s) : command -> Command to be sent
// Return value : None.
//--------------------------------------------------------------------------------------------------*/
void LcdSendCommand ( char command )
{
    DC_Write(0);
//  PRT0DR &= ~LCD_DC_PIN;
    while ( ! (SPI_ReadTxStatus() & SPI_STS_SPI_IDLE) );
  // Send command to display controller.
  SPI_WriteTxData(command);
  // Wait until SPI cycle complete.
  while ( ! (SPI_ReadTxStatus() & SPI_STS_SPI_IDLE) );

}
//-EOF-----------------------------------------------------------------------------------------------
