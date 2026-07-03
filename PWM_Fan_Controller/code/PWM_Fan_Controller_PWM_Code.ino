const int buttonPin = 2;
const int pwmPin = 9;

int fanMode = 0;
int pwmLevels[] = {0, 64, 128, 255};

int lastButtonState = HIGH;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(pwmPin, OUTPUT);
  analogWrite(pwmPin, pwmLevels[fanMode]);
  Serial.begin(9600);
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  
  if (lastButtonState == HIGH && buttonState == LOW) {
		fanMode = (fanMode + 1) % 4;
		analogWrite(pwmPin, pwmLevels[fanMode]);
		
		Serial.print("Fan mode: ");
    Serial.print(fanMode);
    Serial.print(" | PWM: ");
    Serial.println(pwmLevels[fanMode]);
    
	  delay(50);
  }
  
	lastButtonState = buttonState;
}
