#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);    // I2C

void displayString(String secondsRemaining)
{
  u8g.firstPage();

  do
  {
    // show digital value and align its position
    u8g.setFont(u8g_font_profont22);           
    u8g.setPrintPos(16,38);
    u8g.print(secondsRemaining);
  }
  while( u8g.nextPage());
  
  /*
  do 
  {
    rendersomething();
  }
  while( u8g.nextPage() );
  */
}

void setupDisplay(void)
{
  u8g.setFont(u8g_font_chikita);

  // Instructs the display to draw with a pixel on.
  u8g.setColorIndex(1);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {

    u8g.setColorIndex(255);                     // white

  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {

    u8g.setColorIndex(3);                       // max intensity

  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {

    u8g.setColorIndex(1);                       // pixel on

  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {

    u8g.setHiColorByRGB(255,255,255);

  }
}
