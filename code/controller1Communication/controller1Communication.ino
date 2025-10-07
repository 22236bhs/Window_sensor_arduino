#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

const int LCD_CLK = 27;
const int LCD_DATA = 33;
const int LCD_CS = 14;
const int LCD_RST = 26;
const int LCD_BLA = 25;

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, LCD_CLK, LCD_DATA, LCD_CS, LCD_RST);

//Variable Declaration

//MAC Address of the other ESP32 
uint8_t broadcast_address[] = {0xD0, 0xEF, 0x76, 0x48, 0x1C, 0x3C};

// Variable to store if sending data was successful
String success;

//The pin connected to the button
int button_pin = 36;

//The pin connected to the LED
int led_pin = 25;

// The status of the button. true if button pressed. false if button not pressed.
bool outgoing_button;

// The status of the other ESP32's button. true if button pressed. false if button not pressed
bool incoming_button;

int previous_value = 2;

bool received = false;

int not_received_count = 0;

//Structure for any data being sent or received
typedef struct struct_message {
  bool button_status;
} struct_message;

//struct_message to hold outgoing button data
struct_message outgoing_data;

//struct_message to hold incoming button reading
struct_message incoming_reading;

esp_now_peer_info_t peer_info;

// Callback when data is sent
void onDataSent(const wifi_tx_info_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void onDataRecv(const uint8_t * mac, const uint8_t *incoming_data, int len) {
  memcpy(&incoming_reading, incoming_data, sizeof(incoming_reading ));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incoming_button = incoming_reading.button_status;
  //Serial.println("Incoming: " + String(incoming_button));
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
  delay(4000);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  u8g2.begin();
  u8g2.setContrast(128);

  //Initialise pins
  pinMode(button_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(LCD_BLA, OUTPUT);
  digitalWrite(led_pin, LOW);
  digitalWrite(LCD_BLA, HIGH);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(onDataSent);
  
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
  // Set values to send
  outgoing_data.button_status = false;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcast_address, (uint8_t *) &outgoing_data, sizeof(outgoing_data));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

  Serial.println("Received: " + String(received));
  Serial.println("Count: " + String(not_received_count));

  if (received){
    received = false;
    not_received_count = 0;
  }
  else{
    not_received_count++;
  }
  if (not_received_count >= 5){
    LCDPrint("No Response");
    not_received_count = 5;
    previous_value = 2;

  }
  delay(1000);
}

bool readButton(){
  return digitalRead(button_pin);
}

void setLED(bool state){
  digitalWrite(led_pin, state);
}

void LCDPrint(char *text){
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 14, text);
  } while (u8g2.nextPage());
}
