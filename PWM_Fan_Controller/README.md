
# PWM Fan Controller

Embedded systems project implementing multi-mode PWM-based fan speed control using Arduino, MOSFET switching, and motor load control.

This project documents the complete development flow from firmware logic and breadboard testing to hardware switching and signal validation.

⸻

## Project Goals

Build a fan controller capable of cycling through multiple speed modes using a single pushbutton.

Target behavior:

* Press 1 → Low speed
* Press 2 → Medium speed
* Press 3 → High speed
* Press 4 → Off

The project emphasizes both hardware and software reasoning.

⸻

## Core Concepts

This project focuses on:

* button input handling
* debounce logic
* edge detection
* state-machine design
* PWM generation
* MOSFET switching
* inductive load protection
* motor control
* signal verification

⸻

## Hardware Components

Controller

* Arduino Uno

Input

* Pushbutton

Output

* LED (Stage 2 validation)
* TT DC motor (final output)

Switching

* IRLZ44N

Protection

* 1N4007 flyback diode

Supporting Components

* 220Ω resistor
* 10kΩ resistor (original pull-down design)
* Jumper wires
* Breadboard

⸻

## Development Stages

⸻

Stage 1 — Button Input Validation

Goal:

* stable button input detection

Focus:

* INPUT_PULLUP configuration
* debounce handling
* edge detection

Key learning:

* pull-up vs pull-down behavior
* floating inputs
* mechanical switch bounce
* reliable press detection

Status: Complete

⸻

Stage 2 — PWM Output Validation

Goal:

* verify PWM control using LED brightness

Focus:

* multi-mode brightness control
* state transitions
* PWM mapping

Key learning:

* Arduino PWM range (0–255)
* duty cycle behavior
* state-driven output control

PWM Levels:

* Off → 0
* Low → 64
* Medium → 128
* High → 255

Status: Complete

⸻

Stage 3 — MOSFET Gate Control

Goal:

* control switching through MOSFET

Focus:

* gate control using PWM output
* switching behavior verification

Key learning:

* gate charge behavior
* MOSFET switching logic
* pull-down gate stability

Status: Complete

⸻

Stage 4 — Motor Integration

Goal:

* drive DC motor with PWM speed control

Focus:

* motor speed control
* full system integration

Key learning:

* motor response to PWM
* switching under load
* system-level validation

Status: Complete

⸻

Stage 5 — Signal Verification

Goal:

* inspect PWM waveform directly

Focus:

* logic analyzer validation
* duty cycle verification

Key learning:

* waveform interpretation
* duty cycle visualization
* software-to-signal correlation

Status: Complete

⸻

Firmware Highlights

Key firmware concepts used:

* debounced button input
* edge-triggered state changes
* multi-mode state machine
* array-based PWM lookup

Example PWM mapping:

int pwmLevels[] = {0, 64, 128, 255};

⸻

Tools

* Arduino IDE
* KiCad
* Logic Analyzer
* Git
* GitHub

⸻

Current Status

* Stage 1: Complete
* Stage 2: Complete
* Stage 3: Complete
* Stage 4: Complete
* Stage 5: Complete

⸻

## Engineering Notes

This project taught me much more than just PWM motor control.

- A circuit can appear functional while still having wiring issues, poor design decisions, or hidden electrical problems. This project taught me to think beyond “it works.”
- As a visual learner, I initially focused too much on physical wire placement.
- Realizing components do not need direct visible lines to be connected and shared nets define electrical connectivity improved my KiCAD understanding and breadboard debugging.
- As wiring became more complex, I started understanding signal flow much better, which made debugging more logical and systematic.
- Seeing LOW, MEDIUM, and HIGH duty cycles side-by-side made PWM intuitive
- I finally understood that PWM is rapid ON/OFF switching, fixed frequencey and changing duty cycles.
⸻

## Long-Term Objective

This project serves as a foundational embedded systems exercise for learning:

* hardware-software interaction
* real-world signal behavior
* actuator control
* embedded debugging methodology

It also serves as a stepping stone toward more advanced embedded systems development using Arduino, ESP32, and wireless communication systems.