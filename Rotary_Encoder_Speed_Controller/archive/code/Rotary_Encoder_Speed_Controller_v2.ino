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
volatile uint8_t previousEncoderState = 0;

// ============================================================
// Main-loop encoder state
// ============================================================
long lastProcessedEncoderCount = 0;
int8_t lastProcessedDirection = 0;

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
// Diagnostics
// ============================================================
bool statusChanged = true;

// ============================================================
// Quadrature interrupt handler
// ============================================================
void handleEncoderChange() {
  static const int8_t transitionTable[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
  };

  uint8_t currentState =
      (digitalRead(encoderPinA) << 1) |
       digitalRead(encoderPinB);

  uint8_t transition =
      (previousEncoderState << 2) | currentState;

  int8_t movement = transitionTable[transition];

  if (movement != 0) {
    encoderCount += movement;
    encoderDirection = movement;
  }

  previousEncoderState = currentState;
}

// ============================================================
// Arduino lifecycle
// ============================================================
void setup() {
  configurePins();

  previousEncoderState = (digitalRead(encoderPinA) << 1) | digitalRead(encoderPinB);

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
    handleEncoderChange,
    CHANGE
  );

  attachInterrupt(
    digitalPinToInterrupt(encoderPinB),
    handleEncoderChange,
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
  int8_t currentDirection;

  noInterrupts();
  currentCount = encoderCount;
  currentDirection = encoderDirection;
  interrupts();

  if (currentCount == lastProcessedEncoderCount) {
    return;
  }

  lastProcessedEncoderCount = currentCount;
  lastProcessedDirection = currentDirection;
  statusChanged = true;
}

void processButton() {
  // TODO: Add non-blocking button debounce.
}

void updateMotor() {
  // TODO: Add PWM output logic.
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

  if (lastProcessedDirection > 0) {
    Serial.print("CW");
  } else if (lastProcessedDirection < 0) {
    Serial.print("CCW");
  } else {
    Serial.print("None");
  }

  Serial.print(" | Selected PWM: ");
  Serial.print(selectedPWM);

  Serial.print(" | Motor: ");
  Serial.println(motorEnabled ? "ON" : "OFF");
}