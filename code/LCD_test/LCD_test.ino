#include <U8g2lib.h>

const int LCD_CLK = 14;  // R/W on screen (clock)
const int LCD_DATA = 13; // RS on screen (data)
const int LCD_CS = 27;   // E on screen (chip select)
const int LCD_RST = 26;  // RST on screen (reset)
const int LCD_BLA = 25;  // backlight control (optional)

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, LCD_CLK, LCD_DATA, LCD_CS, LCD_RST);

void setup() {
  // backlight pin if you want manual control
  pinMode(LCD_BLA, OUTPUT);
  digitalWrite(LCD_BLA, HIGH); // turn on backlight

  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "Hello XC4617!");
  u8g2.sendBuffer();
}

void loop() {
  // nothing else required — U8g2 handles transfers
}
