#ifndef DISPLAY_BOARD_H
#define DISPLAY_BOARD_H

#include <LovyanGFX.hpp>
#include <Arduino.h>
#include <vector>
#include <functional>

/**
 * @brief Class for handling rotary encoder with push button
 * 
 * This class manages the EC11 rotary encoder including:
 * - Increment/decrement detection
 * - Push button press detection
 * - Debouncing for reliable operation
 */
class RotaryEncoder {
private:
    uint8_t pinA, pinB, pinPush;
    int32_t position;
    int32_t lastPosition;
    uint32_t lastButtonReadTime;
    uint32_t lastEncoderReadTime;
    bool lastButtonState;
    bool buttonPressed;
    volatile int16_t encoderState;
    static const uint32_t DEBOUNCE_DELAY = 50; // milliseconds
    
public:
    /**
     * @brief Constructor for RotaryEncoder
     * @param aPin GPIO pin for encoder channel A
     * @param bPin GPIO pin for encoder channel B
     * @param pushPin GPIO pin for push button
     */
    RotaryEncoder(uint8_t aPin, uint8_t bPin, uint8_t pushPin);
    
    /**
     * @brief Initialize the encoder pins and state
     */
    void begin();
    
    /**
     * @brief Update encoder state (call in loop)
     */
    void update();
    
    /**
     * @brief Get current encoder position
     * @return Current position value
     */
    int32_t getPosition() const { return position; }
    
    /**
     * @brief Check if encoder position changed
     * @return true if position changed
     */
    bool hasChanged() const { return position != lastPosition; }
    
    /**
     * @brief Check if push button was pressed (single press)
     * @return true if button was pressed
     */
    bool isButtonPressed();
    
    /**
     * @brief Reset encoder position to zero
     */
    void resetPosition() { position = 0; lastPosition = 0; }
};

/**
 * @brief Class for managing a display board with encoder
 * 
 * This class combines a LovyanGFX display with a rotary encoder
 * and push button for interactive control.
 */
class DisplayBoard {
private:
    lgfx::LGFX_Device* display;
    RotaryEncoder* encoder;
    uint8_t backlightPin;
    bool backlightOn;
    String boardName;
    
public:
    /**
     * @brief Constructor for DisplayBoard
     * @param displayPtr Pointer to LovyanGFX display object
     * @param encoderPtr Pointer to RotaryEncoder object
     * @param blPin GPIO pin for backlight control (-1 if not used)
     * @param name Name identifier for this board
     */
    DisplayBoard(lgfx::LGFX_Device* displayPtr, RotaryEncoder* encoderPtr, 
                 uint8_t blPin = -1, const String& name = "Board");
    
    /**
     * @brief Initialize the display and encoder
     */
    void begin();
    
    /**
     * @brief Update the board state (call in loop)
     */
    void update();
    
    /**
     * @brief Get the display object
     * @return Pointer to LovyanGFX display
     */
    lgfx::LGFX_Device* getDisplay() { return display; }
    
    /**
     * @brief Get the encoder object
     * @return Pointer to RotaryEncoder
     */
    RotaryEncoder* getEncoder() { return encoder; }
    
    /**
     * @brief Control backlight
     * @param on true to turn on, false to turn off
     */
    void setBacklight(bool on);
    
    /**
     * @brief Toggle backlight state
     */
    void toggleBacklight();
    
    /**
     * @brief Get board name
     * @return Board name string
     */
    String getName() const { return boardName; }
    
    /**
     * @brief Clear display with black color
     */
    void clearDisplay();
};

#endif // DISPLAY_BOARD_H