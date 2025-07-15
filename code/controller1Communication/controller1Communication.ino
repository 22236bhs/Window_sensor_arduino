#include <esp_now.h>
#include <WiFi.h>

#include <Wire.h>

//Variable Declaration

//MAC Address of the other ESP32
uint8_t broadcast_address[] = {D4, 8A, FC, C8, 47, 24};

// Variable to store if sending data was successful
String success;

//The pin connected to the button
int button_pin

// The status of the button. 1 if button pressed. 0 if button not pressed.
int outgoing_button

// The status of the other ESP32's button. 1 if button pressed. 0 if button not pressed
int incoming_button 

//Structure for any data being sent or recieved
typedef struct struct_message {
  int button_status;
} struct_message;

//struct_message to hold outgoing button data
struct_message outgoing_data;

//struct_message to hold incoming button reading
struct_message incoming_reading;

esp_now_peer_info_t peer_info;

// Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
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
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

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
  outgoing_button = digitalRead(button_pin);
  outgoing_data.button_status = outgoing_button;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcast_address, (uint8_t *) &outgoing_data, sizeof(outgoing_data));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
