  #include <U8g2lib.h>

  #ifdef U8X8_HAVE_HW_SPI
  #include <SPI.h>
  #endif

const int LCD_CLK = 27;  //18
const int LCD_DATA = 33; //23
const int LCD_CS = 14;    //5
const int LCD_RST = 26;  //26
const int LCD_BLA = 25;  //25

int count = 0;

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, LCD_CLK, LCD_DATA, LCD_CS, LCD_RST);

void setup() {
  u8g2.begin();
  pinMode(LCD_BLA, OUTPUT);
  digitalWrite(LCD_BLA, HIGH);
  u8g2.setContrast(128);
}

void loop() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 14, "Hello ESP32!");
    u8g2.drawFrame(0, 0, 128, 64);
  } while (u8g2.nextPage());
  delay(1000);
  u8g2.clear();
  delay(1000);
}
