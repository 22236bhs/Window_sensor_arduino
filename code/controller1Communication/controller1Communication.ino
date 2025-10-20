#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

const int LCD_CLK = 27;
const int LCD_DATA = 25;
const int LCD_CS = 33;
const int LCD_RST = 26;

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, LCD_CLK, LCD_DATA, LCD_CS, LCD_RST);

//Variable Declaration

//MAC Address of the other ESP32 
uint8_t broadcast_address[] = {0xD0, 0xEF, 0x76, 0x48, 0x1C, 0x3C};

// Variable to store if sending data was successful
String success;

// The status of the other ESP32's button. true if button pressed. false if button not pressed
bool incoming_button;

// Previous value of the incoming sensor value
int previous_value = 2;

// True if data has been recently recieved
bool received = false;

// The amount of times that no data has been recieved in a row
int not_received_count = 0;

//Structure for any data being sent or received
typedef struct struct_message {
  bool button_status;
} struct_message;

//struct_message to hold incoming button reading
struct_message incoming_reading;

esp_now_peer_info_t peer_info;


// Callback when data is received
void onDataRecv(const uint8_t * mac, const uint8_t *incoming_data, int len) {
  memcpy(&incoming_reading, incoming_data, sizeof(incoming_reading ));
  Serial.print("Bytes received: ");
  Serial.println(len);
  // Change what is displayed on the LCD to the sensor value if it is different to the last recieved sensor value
  incoming_button = incoming_reading.button_status;
  received = true;
  if (incoming_button != previous_value){
    u8g2.clear();
    if (incoming_button == 1){
      LCDPrint("Closed");
    }
    else if (incoming_button == 0){
      LCDPrint("Open");
    }
    
  }
  previous_value = incoming_button;
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  // Initialise the LCD and set its contrast
  u8g2.begin();
  u8g2.setContrast(50);
  delay(4000);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register peer
  memcpy(peer_info.peer_addr, broadcast_address, 6);
  peer_info.channel = 0;  
  peer_info.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peer_info) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(onDataRecv));
}
 
void loop() { 
  // If no data has been recieved, increase the not recieved count
  if (received){
    received = false;
    not_received_count = 0;
  }
  else{
    not_received_count++;
  }
  // If there no data has been recieved for a while, write "No Response" onto the LCD screen.
  if (not_received_count >= 5){
    LCDPrint("No Response");
    not_received_count = 5;
    previous_value = 2;

  }
  delay(1000);
}

// Function to print text onto the connected LCD
void LCDPrint(char *text){
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 14, text);
  } while (u8g2.nextPage());
}
