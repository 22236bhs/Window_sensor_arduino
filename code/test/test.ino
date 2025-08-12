//Variable Declarations

int button_pin = 26;
int led_pin = 32;


void setup(){
  Serial.begin(9600);
  pinMode(button_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
}
 
void loop(){
  setLED(readButton());
  Serial.println(readButton());
}

bool readButton(){
  return digitalRead(button_pin);
}

void setLED(bool state){
  digitalWrite(led_pin, state);
}