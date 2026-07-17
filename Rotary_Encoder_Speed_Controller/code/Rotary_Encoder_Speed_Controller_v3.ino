// ============================================================
// Pin assignments
// ============================================================
constexpr uint8_t encoderPinA = 2;
constexpr uint8_t encoderPinB = 3;
constexpr uint8_t encoderButtonPin = 4;
constexpr uint8_t pwmPin = 9;

// ============================================================
// Configuration constants
// ============================================================
constexpr int8_t transitionsPerDetent = 4;
constexpr unsigned long debounceDelayMs = 30;

constexpr uint8_t pwmLevels[] = {
  0, 16, 32, 48, 64, 80, 96, 112, 128,
  144, 160, 176, 192, 208, 224, 240, 255
};

constexpr uint8_t maxSpeedLevel =
    (sizeof(pwmLevels) / sizeof(pwmLevels[0])) - 1;

// ============================================================
// ISR-owned encoder state
// ============================================================
volatile uint8_t previousEncoderState = 0;
volatile int8_t transitionAccumulator = 0;
volatile long encoderCount = 0;

// ============================================================
// Main-loop encoder state
// ============================================================
long lastProcessedEncoderCount = 0;
int8_t lastProcessedEncoderDirection = 0;

// ============================================================
// Motor-control state
// ============================================================
uint8_t selectedSpeedLevel = 8;
uint8_t selectedPWM = pwmLevels[selectedSpeedLevel];
uint8_t appliedPWM = 0;

bool motorEnabled = false;

// ============================================================
// Button debounce state
// ============================================================
bool lastRawButtonState = HIGH;
bool stableButtonState = HIGH;

unsigned long lastDebounceTime = 0;

// ============================================================
// Diagnostics
// ============================================================
bool statusChanged = true;

// ============================================================
// Forward Declarations
// ============================================================
void configurePins();
void initializeInputs();
void initializeMotor();
void configureInterrupts();

void processEncoder();
void processButton();
void updateMotor();
void printStatus();

void handleEncoderChange();

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

  const uint8_t currentEncoderState = (digitalRead(encoderPinA) << 1) | digitalRead(encoderPinB);

  const uint8_t transition = (previousEncoderState << 2) | currentEncoderState;

  // Save the current state for the next interrupt.
  previousEncoderState = currentEncoderState;

  const int8_t movement = transitionTable[transition];

  // Ignore invalid or unchanged transitions without destroying
  // valid progress already stored in the accumulator.
  if (movement == 0) {
    return;
  }

  transitionAccumulator += movement;

  if (transitionAccumulator >= transitionsPerDetent) {
    encoderCount++;
    transitionAccumulator -= transitionsPerDetent;
  } else if (transitionAccumulator <= -transitionsPerDetent) {
    encoderCount--;
    transitionAccumulator += transitionsPerDetent;
  }
}

// ============================================================
// Arduino lifecycle
// ============================================================
void setup() {
  Serial.begin(9600);

  configurePins();
  initializeInputs();
  initializeMotor();
  configureInterrupts();
}

void loop() {
  processEncoder();
  processButton();
  updateMotor();
  printStatus();
}

// ============================================================
// Hardware configuration
// ============================================================
void configurePins() {
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(encoderButtonPin, INPUT_PULLUP);
  pinMode(pwmPin, OUTPUT);
}

void initializeInputs() {
  // Capture the encoder's real starting state before interrupts begin.
  previousEncoderState = (digitalRead(encoderPinA) << 1) | digitalRead(encoderPinB);

  // Prevent a held button during startup from being treated as a new press.
  const bool initialButtonState = digitalRead(encoderButtonPin);

  lastRawButtonState = initialButtonState;
  stableButtonState = initialButtonState;
}

void initializeMotor() {
  appliedPWM = 0;
  analogWrite(pwmPin, appliedPWM);
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

// ============================================================
// Main-loop processing
// ============================================================
void processEncoder() {
  long currentEncoderCount;

  // A long is multi-byte on the Uno, so copy it atomically.
  noInterrupts();
  currentEncoderCount = encoderCount;
  interrupts();

  if (currentEncoderCount == lastProcessedEncoderCount) {
    return;
  }

  const long detentDifference =
      currentEncoderCount - lastProcessedEncoderCount;

  lastProcessedEncoderDirection =
      detentDifference > 0 ? 1 : -1;

  long requestedSpeedLevel =
      static_cast<long>(selectedSpeedLevel) +
      detentDifference;

  if (requestedSpeedLevel < 0) {
    requestedSpeedLevel = 0;
  } else if (requestedSpeedLevel > maxSpeedLevel) {
    requestedSpeedLevel = maxSpeedLevel;
  }

  selectedSpeedLevel =
      static_cast<uint8_t>(requestedSpeedLevel);

  selectedPWM = pwmLevels[selectedSpeedLevel];
  lastProcessedEncoderCount = currentEncoderCount;

  statusChanged = true;
}

void processButton() {
  const bool rawButtonState =
      digitalRead(encoderButtonPin);

  if (rawButtonState != lastRawButtonState) {
    lastRawButtonState = rawButtonState;
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime < debounceDelayMs) {
    return;
  }

  if (rawButtonState == stableButtonState) {
    return;
  }

  stableButtonState = rawButtonState;

  // INPUT_PULLUP means LOW represents a pressed button.
  if (stableButtonState == LOW) {
    motorEnabled = !motorEnabled;
    statusChanged = true;
  }
}

void updateMotor() {
  const uint8_t requestedPWM = motorEnabled ? selectedPWM : 0;

  if (requestedPWM == appliedPWM) {
    return;
  }

  appliedPWM = requestedPWM;
  analogWrite(pwmPin, appliedPWM);

  statusChanged = true;
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

  if (lastProcessedEncoderDirection > 0) {
    Serial.print("CW");
  } else if (lastProcessedEncoderDirection < 0) {
    Serial.print("CCW");
  } else {
    Serial.print("None");
  }

  Serial.print(" | Selected PWM: ");
  Serial.print(selectedPWM);

  Serial.print(" | Applied PWM: ");
  Serial.print(appliedPWM);

  Serial.print(" | Speed Level: ");
  Serial.print(selectedSpeedLevel);
  Serial.print("/");
  Serial.print(maxSpeedLevel);

  Serial.print(" | Motor: ");
  Serial.println(motorEnabled ? "ON" : "OFF");
}