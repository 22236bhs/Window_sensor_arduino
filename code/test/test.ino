int pin = 36;


void setup(){
  Serial.begin(9600);
  pinMode(pin, INPUT);
}
 
void loop(){
  Serial.println(readButton());
  
}

bool readButton(){
  return digitalRead(pin);
}

void setLED(bool state){
  digitalWrite(pin, state);
}