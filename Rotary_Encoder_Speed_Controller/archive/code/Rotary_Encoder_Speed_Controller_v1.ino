// ============================================================
// Pin assignments
// ============================================================
const int encoderPinA = 2;
const int encoderPinB = 3;
const int encoderButtonPin = 4;
const int pwmPin = 9;

// ============================================================
// ISR-owned state
// ============================================================
volatile long encoderCount = 0;
volatile int encoderDirection = 0;

// ============================================================
// Main-loop encoder state
// ============================================================
long lastProcessedEncoderCount = 0;

int selectedPWM = 128;
int appliedPWM = 0;
bool motorEnabled = false;

// ============================================================
// Button debounce state
// ============================================================
bool lastRawButtonState = HIGH;
bool stableButtonState = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelayMs = 30;

// ============================================================
// Other handler functions
// ============================================================
void handleEncoderA() {
  bool stateA = digitalRead(encoderPinA);
  bool stateB = digitalRead(encoderPinB);

  if (stateA == stateB) {
    encoderCount++;
    encoderDirection = 1;
  } else {
    encoderCount--;
    encoderDirection = -1;
  }
}

void handleEncoderB() {
  bool stateA = digitalRead(encoderPinA);
  bool stateB = digitalRead(encoderPinB);

  if (stateA != stateB) {
    encoderCount++;
    encoderDirection = 1;
  } else {
    encoderCount--;
    encoderDirection = -1;
  }
}

// ============================================================
// Diagnostics
// ============================================================
bool statusChanged = true;

// ============================================================
// Arduino lifecycle
// ============================================================
void setup() {
  configurePins();
  configureInterrupts();
  initializeMotor();

  Serial.begin(9600);
}

void loop() {
  processEncoder();
  processButton();
  updateMotor();
  printStatus();
}

// ============================================================
// Hardware configurations
// ============================================================
void configurePins() {
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(encoderButtonPin, INPUT_PULLUP);
  pinMode(pwmPin, OUTPUT);
}

void configureInterrupts() {
  attachInterrupt(
    digitalPinToInterrupt(encoderPinA),
    handleEncoderA,
    CHANGE
  );

  attachInterrupt(
    digitalPinToInterrupt(encoderPinB),
    handleEncoderB,
    CHANGE
  );
}

void initializeMotor() {
  analogWrite(pwmPin, 0);
}

// ============================================================
// Main-loop processing
// ============================================================
void processEncoder() {
  long currentCount;

  noInterrupts();
  currentCount = encoderCount;
  interrupts();

  if (currentCount != lastProcessedEncoderCount) {

    lastProcessedEncoderCount = currentCount;

    statusChanged = true;
  }
}

// ============================================================
// Diagnostics
// ============================================================
void printStatus() {
  if (!statusChanged) {
    return;
  }

  statusChanged = false;

  Serial.print("Count: ");
  Serial.print(lastProcessedEncoderCount);

  Serial.print(" | Direction: ");

  if (encoderDirection > 0) {
    Serial.print("CW");
  } else if (encoderDirection < 0) {
    Serial.print("CCW");
  } else {
    Serial.print("None");
  }

  Serial.print(" | Selected PWM: ");
  Serial.print(selectedPWM);

  Serial.print(" | Motor: ");

  if (motorEnabled) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }
}