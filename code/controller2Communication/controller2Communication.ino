#include <esp_now.h>
#include <WiFi.h>

#include <Wire.h>

//Variable Declaration

//MAC Address of the other ESP32 
uint8_t broadcast_address[] = {0x00, 0x4B, 0x12, 0x2E, 0x21, 0xDC};

// Variable to store if sending data was successful
String success;

//The pin connected to the button
int reed_pin = 25;

// The status of the button. true if button pressed. false if button not pressed.
bool outgoing_button;

//Structure for any data being sent or recieved
typedef struct struct_message {
  bool button_status;
} struct_message;

//struct_message to hold outgoing button data
struct_message outgoing_data;

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
 
void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  delay(4000);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Initialise pins
  pinMode(reed_pin, INPUT);

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
}
 
void loop() { 
  // Set values to send
  outgoing_button = readButton();
  outgoing_data.button_status = outgoing_button;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcast_address, (uint8_t *) &outgoing_data, sizeof(outgoing_data));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(1000);
}

// Read the value of the connected reed switch
bool readButton(){
  return digitalRead(reed_pin);
}
