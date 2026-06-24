#include "RotaryEncoder.h"

/**
 * @file RotaryEncoder.cpp
 * @brief Implements rotary encoder quadrature and push-button handling.
 *
 * High-level behavior present in this file:
 * - Initializes encoder/button GPIOs as INPUT_PULLUP.
 * - Decodes state transitions to update position.
 * - Debounces and latches push-button press events.
 *
 * Notes based on current code:
 * - lastEncoderReadTime is initialized but not used by current implementation.
 */

/**
 * Function: RotaryEncoder::RotaryEncoder(uint8_t aPin, uint8_t bPin, uint8_t pushPin)
 * Purpose: Construct a rotary encoder object with configured pin assignments and reset internal state.
 * Parameters:
 * - uint8_t aPin: GPIO pin for encoder channel A.
 * - uint8_t bPin: GPIO pin for encoder channel B.
 * - uint8_t pushPin: GPIO pin for encoder push button.
 * Returns: None (constructor).
 * Usage example: RotaryEncoder enc(21, 22, 26);
 * Warnings/constraints:
 * - Position starts at 0 and buttonPressed starts false.
 * - lastEncoderReadTime is initialized to 0 but not used in this file.
 */
RotaryEncoder::RotaryEncoder(uint8_t aPin, uint8_t bPin, uint8_t pushPin)
    : pinA(aPin), pinB(bPin), pinPush(pushPin), position(0), lastPosition(0),
      lastButtonReadTime(0), lastEncoderReadTime(0), lastButtonState(HIGH),
      buttonPressed(false), encoderState(0) {}

/**
 * Function: RotaryEncoder::begin()
 * Purpose: Configure encoder and button pins as input pull-ups and capture initial quadrature state.
 * Parameters:
 * - None.
 * Returns: void.
 * Usage example: encoder.begin();
 * Warnings/constraints:
 * - Assumes pin numbers were provided via constructor.
 */
void RotaryEncoder::begin() {
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    pinMode(pinPush, INPUT_PULLUP);

    // Initialize encoder state
    int a = digitalRead(pinA);
    int b = digitalRead(pinB);
    encoderState = (a << 1) | b;
}

/**
 * Function: RotaryEncoder::update()
 * Purpose: Read encoder/button inputs, update position based on transition direction, and debounce button press events.
 * Parameters:
 * - None.
 * Returns: void.
 * Usage example: encoder.update();
 * Warnings/constraints:
 * - Position changes only when a recognized state transition occurs.
 * - Button press latches buttonPressed=true only on HIGH->LOW edge and debounce timing check.
 */
void RotaryEncoder::update() {
    // Read encoder state
    int a = digitalRead(pinA);
    int b = digitalRead(pinB);
    int newState = (a << 1) | b;

    if (newState != encoderState) {
        // Determine direction
        if ((encoderState == 0b00 && newState == 0b01) ||
            (encoderState == 0b01 && newState == 0b11) ||
            (encoderState == 0b11 && newState == 0b10) ||
            (encoderState == 0b10 && newState == 0b00)) {
            position++;
        } else if ((encoderState == 0b00 && newState == 0b10) ||
                   (encoderState == 0b10 && newState == 0b11) ||
                   (encoderState == 0b11 && newState == 0b01) ||
                   (encoderState == 0b01 && newState == 0b00)) {
            position--;
        }
        encoderState = newState;
    }

    // Update button state with debouncing
    bool currentButtonState = digitalRead(pinPush);
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        if (millis() - lastButtonReadTime > DEBOUNCE_DELAY) {
            buttonPressed = true;
            lastButtonReadTime = millis();
        }
    }
    lastButtonState = currentButtonState;
}

/**
 * Function: RotaryEncoder::isButtonPressed()
 * Purpose: Return and clear the latched button-press flag.
 * Parameters:
 * - None.
 * Returns: bool, true when a debounced press was latched since last call; otherwise false.
 * Usage example: if (encoder.isButtonPressed()) { // handle press }
 * Warnings/constraints:
 * - Calling this function consumes the latched press state.
 */
bool RotaryEncoder::isButtonPressed() {
    if (buttonPressed) {
        buttonPressed = false;
        return true;
    }
    return false;
}
