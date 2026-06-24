#include "DisplayBoard.h"

// RotaryEncoder Implementation
RotaryEncoder::RotaryEncoder(uint8_t aPin, uint8_t bPin, uint8_t pushPin)
    : pinA(aPin), pinB(bPin), pinPush(pushPin), position(0), lastPosition(0),
      lastButtonReadTime(0), lastEncoderReadTime(0), lastButtonState(HIGH),
      buttonPressed(false), encoderState(0) {}

void RotaryEncoder::begin() {
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    pinMode(pinPush, INPUT_PULLUP);
    
    // Initialize encoder state
    int a = digitalRead(pinA);
    int b = digitalRead(pinB);
    encoderState = (a << 1) | b;
}

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

bool RotaryEncoder::isButtonPressed() {
    if (buttonPressed) {
        buttonPressed = false;
        return true;
    }
    return false;
}

// DisplayBoard Implementation
DisplayBoard::DisplayBoard(lgfx::LGFX_Device* displayPtr, RotaryEncoder* encoderPtr,
                           uint8_t blPin, const String& name)
    : display(displayPtr), encoder(encoderPtr), backlightPin(blPin),
      backlightOn(true), boardName(name) {}

void DisplayBoard::begin() {
    if (display) {
        display->init();
        display->setRotation(0);
        display->fillScreen(TFT_BLACK);
    }
    
    if (encoder) {
        encoder->begin();
    }
    
    if (backlightPin != -1) {
        pinMode(backlightPin, OUTPUT);
        digitalWrite(backlightPin, HIGH);
    }
}

void DisplayBoard::update() {
    if (encoder) {
        encoder->update();
    }
}

void DisplayBoard::setBacklight(bool on) {
    backlightOn = on;
    if (backlightPin != -1) {
        digitalWrite(backlightPin, on ? HIGH : LOW);
    }
}

void DisplayBoard::toggleBacklight() {
    setBacklight(!backlightOn);
}

void DisplayBoard::clearDisplay() {
    if (display) {
        display->fillScreen(TFT_BLACK);
    }
}