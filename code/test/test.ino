//Variable Declarations

int button_pin = 25;
int led_pin = 33;
int button_power_pin = 26;


void setup(){
  Serial.begin(9600);
  pinMode(button_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(button_power_pin, OUTPUT);
  digitalWrite(button_power_pin, HIGH);
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