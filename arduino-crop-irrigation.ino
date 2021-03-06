#include <Wire.h>
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);    // I2C
#include "Wire.h"
#include "RTClib.h"
RTC_DS1307 RTC;

// set all moisture sensors PIN ID
int moisture1 = A0;
int moisture2 = A1;
int moisture3 = A2;
int moisture4 = A3;

// declare moisture values
int moisture1_value = 0 ;
int moisture2_value = 0;
int moisture3_value = 0;
int moisture4_value = 0;

// declare minimum moisture threshold to trigger watering
int min_moisture1 = 50;
int min_moisture2 = 50;
int min_moisture3 = 50;
int min_moisture4 = 50;

// declare maximum moisture threshold to stop watering
int max_moisture1 = 75;
int max_moisture2 = 75;
int max_moisture3 = 75;
int max_moisture4 = 75;

// set water relays
int relay1 = 6;
int relay2 = 8;
int relay3 = 9;
int relay4 = 10;

// set water pump
int pump = 4;

// set button
int button = 12;

//pump state    1:open   0:close
int pump_state_flag = 0;

//relay1 state    1:open   0:close
int relay1_state_flag = 0;

//relay2 state   1:open   0:close
int relay2_state_flag = 0;

//relay3 state  1:open   0:close
int relay3_state_flag = 0;

//relay4 state   1:open   0:close
int relay4_state_flag = 0;

static unsigned long currentMillis_send = 0;
static unsigned long  Lasttime_send = 0;

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat",};
unsigned long nowtime;
unsigned long endtime;
unsigned long nowtimeNext;
unsigned long nowtime1;
unsigned long endtime1;
unsigned long nowtimeNext1;
unsigned long nowtime2;
unsigned long endtime2;
unsigned long nowtimeNext2;
unsigned long nowtime3;
unsigned long endtime3;
unsigned long nowtimeNext3;


// good tomato
unsigned char bitmap_tomato_good[] U8G_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x80, 0x05, 0x00, 
  0x00, 0x80, 0x04, 0x00, 0x00, 0xC8, 0x02, 0x00, 0x00, 0x7C, 0x3F, 0x00, 
  0x00, 0x6C, 0x67, 0x00, 0x00, 0x4F, 0xF3, 0x00, 0x80, 0x49, 0x9D, 0x01, 
  0xC0, 0x3E, 0x1A, 0x03, 0x60, 0x34, 0x3E, 0x06, 0x30, 0x60, 0x03, 0x0C, 
  0x18, 0xC0, 0x01, 0x08, 0x18, 0x00, 0x00, 0x18, 0x08, 0x00, 0x00, 0x10, 
  0x0C, 0x18, 0x0C, 0x30, 0x2C, 0x18, 0x0C, 0x30, 0x2C, 0x00, 0x00, 0x30, 
  0x2C, 0x00, 0x00, 0x30, 0x68, 0x08, 0x10, 0x10, 0xD8, 0x18, 0x18, 0x18, 
  0x90, 0x30, 0x0C, 0x08, 0xB0, 0xE1, 0x07, 0x0C, 0x60, 0x00, 0x00, 0x06, 
  0xC0, 0x00, 0x00, 0x03, 0x00, 0x07, 0xC0, 0x01, 0x00, 0x1E, 0x7C, 0x00, 
  0x00, 0xF0, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

// bad tomato
unsigned char bitmap_tomato_bad[] U8G_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x80, 0x05, 0x00, 
  0x00, 0x80, 0x04, 0x00, 0x00, 0xC8, 0x02, 0x00, 0x00, 0x7C, 0x3F, 0x00, 
  0x00, 0x6C, 0x67, 0x00, 0x00, 0x4F, 0xF3, 0x00, 0x80, 0x59, 0x9D, 0x01, 
  0xC0, 0x1E, 0x1A, 0x03, 0x60, 0x34, 0x3F, 0x06, 0x30, 0x60, 0x01, 0x0C, 
  0x18, 0xC8, 0x09, 0x08, 0x18, 0x90, 0x04, 0x18, 0x08, 0x20, 0x02, 0x10, 
  0x0C, 0x18, 0x0C, 0x30, 0x2C, 0x18, 0x0C, 0x30, 0x2C, 0x00, 0x00, 0x30, 
  0x2C, 0x00, 0x00, 0x30, 0x68, 0x00, 0x00, 0x10, 0xD8, 0xF8, 0x0F, 0x18, 
  0x90, 0xF8, 0x0F, 0x08, 0x30, 0x01, 0x00, 0x0C, 0x60, 0x01, 0x00, 0x06, 
  0xC0, 0x00, 0x00, 0x03, 0x00, 0x03, 0xC0, 0x01, 0x00, 0x3E, 0x7C, 0x00, 
  0x00, 0xF8, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// good pineapple
unsigned char bitmap_pineapple_good[] U8G_PROGMEM = {
  0x00, 0x38, 0x1C, 0x00, 0x00, 0x70, 0x0F, 0x00, 0x00, 0xA0, 0x05, 0x00, 
  0x00, 0x3C, 0x3D, 0x00, 0x00, 0x7C, 0x3F, 0x00, 0x00, 0xD8, 0x1B, 0x00, 
  0x00, 0x90, 0x0C, 0x00, 0x00, 0x76, 0x56, 0x00, 0x00, 0x7E, 0x7E, 0x00, 
  0x00, 0xCC, 0x31, 0x00, 0x00, 0x18, 0x1B, 0x00, 0x00, 0x30, 0x0E, 0x00, 
  0x00, 0xE0, 0x0F, 0x00, 0x00, 0x38, 0x18, 0x00, 0x00, 0x0C, 0x30, 0x00, 
  0x00, 0x04, 0x20, 0x00, 0x00, 0x62, 0x46, 0x00, 0x00, 0x03, 0xC0, 0x00, 
  0x00, 0x63, 0x46, 0x00, 0x00, 0x62, 0xC6, 0x00, 0x00, 0x01, 0xC0, 0x00, 
  0x00, 0x03, 0x80, 0x00, 0x00, 0x13, 0xC8, 0x00, 0x00, 0x22, 0xC4, 0x00, 
  0x00, 0xC2, 0x43, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x06, 0x60, 0x00, 
  0x00, 0x0C, 0x30, 0x00, 0x00, 0x78, 0x1E, 0x00, 0x00, 0xE0, 0x07, 0x00,
};

// bad pineapple
unsigned char bitmap_pineapple_bad[] U8G_PROGMEM = {
  0x00, 0x38, 0x1C, 0x00, 0x00, 0x70, 0x0F, 0x00, 0x00, 0xA0, 0x07, 0x00, 
  0x00, 0x3C, 0x3D, 0x00, 0x00, 0x7C, 0x3F, 0x00, 0x00, 0xD8, 0x19, 0x00, 
  0x00, 0x90, 0x0D, 0x00, 0x00, 0x76, 0x6C, 0x00, 0x00, 0x7E, 0x7E, 0x00, 
  0x00, 0xCC, 0x31, 0x00, 0x00, 0x18, 0x1B, 0x00, 0x00, 0x30, 0x0E, 0x00, 
  0x00, 0xE0, 0x0F, 0x00, 0x00, 0x38, 0x18, 0x00, 0x00, 0x0C, 0x30, 0x00, 
  0x00, 0x24, 0x24, 0x00, 0x00, 0x42, 0x42, 0x00, 0x00, 0x03, 0xC0, 0x00, 
  0x00, 0x63, 0x46, 0x00, 0x00, 0x62, 0xC6, 0x00, 0x00, 0x01, 0xC0, 0x00, 
  0x00, 0x03, 0x80, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0xE2, 0xC7, 0x00, 
  0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x06, 0x60, 0x00, 
  0x00, 0x0C, 0x30, 0x00, 0x00, 0x38, 0x1C, 0x00, 0x00, 0xE0, 0x0F, 0x00,
};

static unsigned char bitmap_T[] U8G_PROGMEM = {
  0xF7, 0x01, 0x1D, 0x03, 0x0B, 0x02, 0x0C, 0x02, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x08, 0x02,
  0x18, 0x03, 0xF0, 0x01
};

static unsigned char bitmap_H[] U8G_PROGMEM = {
  0x00, 0x00, 0x80, 0x01, 0xC0, 0x03, 0xE0, 0x07, 0xF0, 0x0F, 0xF8, 0x1F, 0xF8, 0x1F, 0xFC, 0x3F,
  0xFC, 0x3F, 0xFE, 0x7F, 0xEE, 0x7F, 0xB3, 0xF7, 0xBB, 0xFB, 0xBB, 0xFD, 0xBB, 0xFD, 0xC7, 0xFE,
  0x7F, 0xC3, 0x3F, 0xDD, 0xBF, 0xFD, 0xDF, 0xDD, 0xEE, 0x5B, 0xFE, 0x7F, 0xFC, 0x3F, 0xF8, 0x1F,
  0xE0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup()
{
  Wire.begin();
  RTC.begin();
  Serial.begin(9600);
  // declare relay as output
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  // declare pump as output
  pinMode(pump, OUTPUT);
  // declare switch as input
  pinMode(button, INPUT);
  // close all valves
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
  // turn off pump
  digitalWrite(pump, LOW);
  //pinMode(ROTARY_ANGLE_SENSOR, INPUT);
  // water_crops();
}

void loop()
{
  // read the value from the moisture sensors:
  read_value();
  water_crops();
  int button_state = digitalRead(button);
  if (button_state == 1)
  {
    read_value();
    u8g.firstPage();
    do
    {
      drawTH();
      drawplants();
     
    } while ( u8g.nextPage() );
  }
  else
  {
    u8g.firstPage();
    do
    {
      drawtime();
    } while (u8g.nextPage());
  }
}

//Set moisture value
void read_value()
{
/**************These is for resistor moisture sensor***********
 float value1 = analogRead(A0);
  moisture1_value = (value1 * 120) / 1023; delay(20);
  float value2 = analogRead(A1);
  moisture2_value = (value2 * 120) / 1023; delay(20);
  float value3 = analogRead(A2);
  moisture3_value = (value3 * 120) / 1023; delay(20);
  float value4 = analogRead(A3);
  moisture4_value = (value4 * 120) / 1023; delay(20);
 **********************************************************/
/************These is for capacity moisture sensor*********/
 float value1 = analogRead(A0);
  moisture1_value =map(value1,590,360,0,100); delay(20);
  if(moisture1_value<0){
    moisture1_value=0;
  }
  float value2 = analogRead(A1);
  moisture2_value =map(value2,600,360,0,100); delay(20);
  if(moisture2_value<0) {
    moisture2_value=0;
  }
  float value3 = analogRead(A2);
  moisture3_value =map(value3,600,360,0,100); delay(20);
  if(moisture3_value<0){
    moisture3_value=0;
  }
  float value4 = analogRead(A3);
  moisture4_value =map(value4,600,360,0,100); delay(20);
  if(moisture4_value<0) {
    moisture4_value=0;
  }
}

void water_crops()
{
  if (moisture1_value <= min_moisture1)
  {
    digitalWrite(relay1, HIGH);
    relay1_state_flag = 1;
    delay(50);
    toggle_pump();
    /*
    if (pump_state_flag == 0)
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }*/
  }
  else if (moisture1_value >= max_moisture1)
  {
    relay1_state_flag = 0;
    toggle_pump();
    digitalWrite(relay1, LOW);
    /*
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
    */
  }
  if (moisture2_value <= min_moisture2)
  {
    digitalWrite(relay2, HIGH);
    relay2_state_flag = 1;
    delay(50);
    toggle_pump();
    /*
    if (pump_state_flag == 0)
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }
    */
  }
  else if (moisture2_value >= max_moisture2)
  {
    relay2_state_flag = 0;
    toggle_pump();
    digitalWrite(relay2, LOW);
    /*
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
    */
  }
  if (moisture3_value <= min_moisture3)
  {
    digitalWrite(relay3, HIGH);
    relay3_state_flag = 1;
    delay(50);
    toggle_pump();
    /*
    if (pump_state_flag == 0)
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }
    */
  }
  else if (moisture3_value >= max_moisture3)
  {
    relay3_state_flag = 0;
    toggle_pump();
    digitalWrite(relay3, LOW);
    /*
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
    */
  }
  if (moisture4_value <= min_moisture4)
  {
    digitalWrite(relay4, HIGH);
    relay4_state_flag = 1;
    delay(50);
    toggle_pump();
    /*
    if (pump_state_flag == 0)
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }
    */
  }
  else if (moisture4_value >= max_moisture4)
  {
    relay4_state_flag = 0;
    toggle_pump();
    digitalWrite(relay4, LOW);
    /*
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
    */
  }

}

void toggle_pump()
{
  // Only turn on the pump if at least one valve is open to prevent overpressurizing water supply manifold
  if ((relay1_state_flag == 1) || (relay2_state_flag == 1) || (relay3_state_flag == 1) || (relay4_state_flag == 1))
  {
    // If pump isn't already on wait 2 seconds to allow solenoids to open fully to prevent overpressure
    if(pump_state_flag == 0)
    {
      delay(2000);
    }
    pump_state_flag = 1;
    digitalWrite(pump, HIGH);
  }
  else //No valves are open... turn off pump
  {
    // If pump is running, turn off and wait 2 seconds before returning so lines depressurize before valves close
    if(pump_state_flag == 1)
    {
      delay(2000); // Wait 2 seconds to ensure spray directly on sensor still results in at least 2 seconds of watering
      pump_state_flag = 0;
      digitalWrite(pump, LOW);
      //Wait 2 seconds before returning so lines depressurize before valves close
      delay(2000);
    }
    else //Double check pump de-energized
    {
      pump_state_flag = 0;
      digitalWrite(pump, LOW);
    }
  }
}



void drawtime(void)
{
  int x = 5;
  float i = 25.00;
  float j = 54;
  DateTime now = RTC.now();
  //Serial.print(now.year(), DEC);
  if (! RTC.isrunning())
  {
    u8g.setFont(u8g_font_6x10);
    u8g.setPrintPos(5, 20);
    u8g.print("RTC is NOT running!");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  else
  {
    u8g.setFont(u8g_font_7x13);
    u8g.setPrintPos(x, 11);
    u8g.print(now.year(), DEC);
    u8g.setPrintPos(x + 80, 11);
    u8g.print(daysOfTheWeek[now.dayOfTheWeek()]);
    u8g.setPrintPos(x + 28, 11);
    u8g.print("/");
    u8g.setPrintPos(x + 33, 11);
    u8g.print(now.month(), DEC);
    if (now.month() < 10)
      x -= 7;
    u8g.setPrintPos(x + 47, 11);
    u8g.print("/");
    u8g.setPrintPos(x + 53, 11);
    u8g.print(now.day(), DEC);
    u8g.setFont(u8g_font_8x13);
    int x = 35;
    u8g.setPrintPos(x, 33);
    u8g.print(now.hour(), DEC);
    if (now.hour() < 10)
      x -= 7;
    u8g.setPrintPos(x + 15, 33);
    u8g.print(":");
    u8g.setPrintPos(x + 21, 33);
    u8g.print(now.minute(), DEC);
    if (now.minute() < 10)
      x -= 7;
    u8g.setPrintPos(x + 36, 33);
    u8g.print(":");
    u8g.setPrintPos(x + 42, 33);
    u8g.print(now.second(), DEC);
  }
}

void drawLogo(uint8_t d)
{
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(8 + d, 30 + d, "E");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(30 + d, 30 + d, "l");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(40 + d, 30 + d, "e");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(55 + d, 30 + d, "c");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(70 + d, 30 + d, "r");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(85 + d, 30 + d, "o");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(100 + d, 30 + d, "w");
}


//Draw plants
void drawplants(void)
{
  if (moisture1_value < 30)
  {
    u8g.drawXBMP(0, 0, 32, 30, bitmap_tomato_bad);
  }
  else
  {
    u8g.drawXBMP(0, 0, 32, 30, bitmap_tomato_good);
  }
  if (moisture2_value < 30)
  {
    u8g.drawXBMP(32, 0, 32, 30, bitmap_tomato_bad);
  }
  else
  {
    u8g.drawXBMP(32, 0, 32, 30, bitmap_tomato_good);
  }
  if (moisture3_value < 30)
  {
    u8g.drawXBMP(64, 0, 32, 30, bitmap_tomato_bad);
  }
  else
  {
    u8g.drawXBMP(64, 0, 32, 30, bitmap_tomato_good);
  }
  if (moisture4_value < 30)
  {
    u8g.drawXBMP(96, 0, 32, 30, bitmap_pineapple_bad);
  }
  else
  {
    u8g.drawXBMP(96, 0, 32, 30, bitmap_pineapple_good);
  }

}


void drawTH(void)
{
  int A = 0;
  int B = 0;
  int C = 64;
  int D = 96;
  char moisture1_value_temp[5] = {0};
  char moisture2_value_temp[5] = {0};
  char moisture3_value_temp[5] = {0};
  char moisture4_value_temp[5] = {0};
  read_value();
  itoa(moisture1_value, moisture1_value_temp, 10);
  itoa(moisture2_value, moisture2_value_temp, 10);
  itoa(moisture3_value, moisture3_value_temp, 10);
  itoa(moisture4_value, moisture4_value_temp, 10);
  u8g.setFont(u8g_font_7x14);
  u8g.setPrintPos(9, 60);
  u8g.print("A0");
  if (moisture1_value < 10)
  {
    //u8g.setPrintPos(A + 14, 45 );
    u8g.drawStr(A + 14, 45, moisture1_value_temp);
    delay(20);
    u8g.drawStr(A + 14, 45, moisture1_value_temp);
    
  }
  else if (moisture1_value < 100)
  {
    //u8g.setPrintPos(A + 7, 45);
    u8g.drawStr(A + 7, 45, moisture1_value_temp);
    delay(20);
    u8g.drawStr(A + 7, 45, moisture1_value_temp);
   
  }
  else
  {
    //u8g.setPrintPos(A + 2, 45 );
    moisture1_value = 100;
    itoa(moisture1_value, moisture1_value_temp, 10);
    u8g.drawStr(A + 2, 45, moisture1_value_temp);
  }
  //u8g.print(moisture1_value);
  u8g.setPrintPos(A + 23, 45 );
  u8g.print("%");
  u8g.setPrintPos(41, 60 );
  u8g.print("A1");
  if (moisture2_value < 10)
  {
    //u8g.setPrintPos(B + 46, 45 );
    u8g.drawStr(B + 46, 45, moisture2_value_temp); 
    delay(20);
    u8g.drawStr(B + 46, 45, moisture2_value_temp); 
  }
  else if (moisture2_value < 100)
  {
    //u8g.setPrintPos(B + 39, 45);
    u8g.drawStr(B + 39, 45, moisture2_value_temp);
    delay(20);
    u8g.drawStr(B + 39, 45, moisture2_value_temp);
  }
  else
  {
    //u8g.setPrintPos(B + 32, 45);
    moisture2_value = 100;
    itoa(moisture2_value, moisture2_value_temp, 10);
    u8g.drawStr(B + 32, 45, moisture2_value_temp);
  }
  // u8g.print(moisture2_value);
  u8g.setPrintPos(B + 54, 45);
  u8g.print("%");
  u8g.setPrintPos(73, 60);
  u8g.print("A2");
  if (moisture3_value < 10)
  {
    //u8g.setPrintPos(C + 14, 45 );
    u8g.drawStr(C + 14, 45, moisture3_value_temp);
    delay(20);
    u8g.drawStr(C + 14, 45, moisture3_value_temp);
    
  }
  else if (moisture3_value < 100)
  {
    // u8g.setPrintPos(C + 7, 45);
   u8g.drawStr(C + 7, 45, moisture3_value_temp);
    delay(20);
    u8g.drawStr(C + 7, 45, moisture3_value_temp);
    
  }
  else
  {
    // u8g.setPrintPos(C + 2, 45);
    moisture3_value = 100;
    itoa(moisture3_value, moisture3_value_temp, 10);
    u8g.drawStr(C + 2, 45, moisture3_value_temp);
  }
  //u8g.print(moisture3_value);
  u8g.setPrintPos(C + 23, 45);
  u8g.print("%");
  u8g.setPrintPos(105, 60);
  u8g.print("A3");
  if (moisture4_value < 10)
  {
    //u8g.setPrintPos(D + 14, 45 );
    u8g.drawStr(D + 14, 45, moisture4_value_temp);
    delay(20);
    u8g.drawStr(D + 14, 45, moisture4_value_temp);
   
  }
  else if (moisture4_value < 100)
  {
    // u8g.setPrintPos(D + 7, 45);
    u8g.drawStr(D + 7, 45, moisture4_value_temp);
    delay(20);
    u8g.drawStr(D + 7, 45, moisture4_value_temp);
  
  }
  else
  {
    //u8g.setPrintPos(D + 2, 45);
    moisture4_value = 100;
    itoa(moisture4_value, moisture4_value_temp, 10);
    u8g.drawStr(D + 2, 45, moisture4_value_temp);
  }
  //u8g.print(moisture4_value);
  u8g.setPrintPos(D + 23, 45);
  u8g.print("%");
}
