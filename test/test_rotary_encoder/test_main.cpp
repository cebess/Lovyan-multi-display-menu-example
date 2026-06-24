#include <Arduino.h>
#include <unity.h>

// White-box access for targeted state assertions in tests.
#define private public
#include "RotaryEncoder.h"
#undef private

// Pull production implementation into test image when test_build_src is not enabled.
#include "../../src/RotaryEncoder.cpp"

namespace {

constexpr uint8_t TEST_PIN_A = 21;
constexpr uint8_t TEST_PIN_B = 22;
constexpr uint8_t TEST_PIN_PUSH = 26;
constexpr uint32_t MANUAL_TIMEOUT_MS = 12000;

// Manual interaction tests are opt-in so unattended `pio test` runs don't appear to hang.
#ifndef RUN_MANUAL_ROTARY_TESTS
#define RUN_MANUAL_ROTARY_TESTS 0
#endif

void print_manual_header(const char* testName, const char* setup, const char* expected) {
    Serial.println();
    Serial.println("============================================================");
    Serial.print("[MANUAL TEST] ");
    Serial.println(testName);
    Serial.println("Action required:");
    Serial.println(setup);
    Serial.println("Expected behavior:");
    Serial.println(expected);
    Serial.println("You have 12 seconds. Watch this monitor while interacting.");
    Serial.println("============================================================");

    TEST_MESSAGE(testName);
    TEST_MESSAGE(setup);
    TEST_MESSAGE(expected);
}

void print_manual_timeout_reminders(const char* reminder) {
    uint32_t started = millis();
    uint32_t lastTick = started;
    uint8_t secondsShown = 0;
    while (millis() - started < 3000) {
        if (millis() - lastTick >= 1000) {
            ++secondsShown;
            lastTick = millis();
            Serial.print("[MANUAL TEST] ");
            Serial.print(reminder);
            Serial.print(" (starting in ");
            Serial.print(3 - secondsShown);
            Serial.println("s)");
        }
        delay(5);
    }
}

void test_constructor_initializes_defaults() {
    RotaryEncoder encoder(TEST_PIN_A, TEST_PIN_B, TEST_PIN_PUSH);

    TEST_ASSERT_EQUAL(TEST_PIN_A, encoder.pinA);
    TEST_ASSERT_EQUAL(TEST_PIN_B, encoder.pinB);
    TEST_ASSERT_EQUAL(TEST_PIN_PUSH, encoder.pinPush);
    TEST_ASSERT_EQUAL(0, encoder.position);
    TEST_ASSERT_EQUAL(0, encoder.lastPosition);
    TEST_ASSERT_EQUAL(0, encoder.lastButtonReadTime);
    TEST_ASSERT_EQUAL(0, encoder.lastEncoderReadTime);
    TEST_ASSERT_TRUE(encoder.lastButtonState);
    TEST_ASSERT_FALSE(encoder.buttonPressed);
}

void test_reset_position_resets_position_and_last_position() {
    RotaryEncoder encoder(TEST_PIN_A, TEST_PIN_B, TEST_PIN_PUSH);
    encoder.position = 17;
    encoder.lastPosition = -4;

    encoder.resetPosition();

    TEST_ASSERT_EQUAL(0, encoder.position);
    TEST_ASSERT_EQUAL(0, encoder.lastPosition);
}

void test_has_changed_reflects_position_vs_last_position() {
    RotaryEncoder encoder(TEST_PIN_A, TEST_PIN_B, TEST_PIN_PUSH);

    encoder.position = 3;
    encoder.lastPosition = 3;
    TEST_ASSERT_FALSE(encoder.hasChanged());

    encoder.position = 4;
    TEST_ASSERT_TRUE(encoder.hasChanged());
}

void test_is_button_pressed_returns_true_once_then_clears_latch() {
    RotaryEncoder encoder(TEST_PIN_A, TEST_PIN_B, TEST_PIN_PUSH);
    encoder.buttonPressed = true;

    TEST_ASSERT_TRUE(encoder.isButtonPressed());
    TEST_ASSERT_FALSE(encoder.buttonPressed);
    TEST_ASSERT_FALSE(encoder.isButtonPressed());
}

void test_begin_initializes_encoder_state_from_pin_reads() {
    RotaryEncoder encoder(TEST_PIN_A, TEST_PIN_B, TEST_PIN_PUSH);

    encoder.begin();

    TEST_ASSERT_TRUE(encoder.encoderState >= 0);
    TEST_ASSERT_TRUE(encoder.encoderState <= 3);
}

void test_manual_clockwise_rotation_increases_position() {
    RotaryEncoder encoder(TEST_PIN_A, TEST_PIN_B, TEST_PIN_PUSH);
    encoder.begin();
    encoder.resetPosition();

    print_manual_header(
        "Clockwise rotation increases position",
        "Rotate the encoder clockwise at least one detent within 12 seconds.",
        "Expected: encoder position increases above the starting value.");
    print_manual_timeout_reminders("Prepare to rotate clockwise");

    int32_t start = encoder.getPosition();
    uint32_t started = millis();
    uint32_t lastTick = started;
    bool detected = false;
    while (millis() - started < MANUAL_TIMEOUT_MS) {
        encoder.update();
        if (encoder.getPosition() > start) {
            detected = true;
            break;
        }
        if (millis() - lastTick >= 1000) {
            lastTick = millis();
            uint32_t elapsed = (millis() - started) / 1000;
            uint32_t remaining = (MANUAL_TIMEOUT_MS / 1000) - elapsed;
            Serial.print("[MANUAL TEST] Waiting for clockwise motion | remaining: ");
            Serial.print(remaining);
            Serial.println("s");
        }
        delay(2);
    }

    if (!detected) {
        TEST_IGNORE_MESSAGE("No clockwise interaction detected. Keep Serial Monitor open and repeat test while turning encoder.");
    }

    TEST_ASSERT_GREATER_THAN_INT32(start, encoder.getPosition());
}

void test_manual_counterclockwise_rotation_decreases_position() {
    RotaryEncoder encoder(TEST_PIN_A, TEST_PIN_B, TEST_PIN_PUSH);
    encoder.begin();
    encoder.resetPosition();

    print_manual_header(
        "Counterclockwise rotation decreases position",
        "Rotate the encoder counterclockwise at least one detent within 12 seconds.",
        "Expected: encoder position decreases below the starting value.");
    print_manual_timeout_reminders("Prepare to rotate counterclockwise");

    int32_t start = encoder.getPosition();
    uint32_t started = millis();
    uint32_t lastTick = started;
    bool detected = false;
    while (millis() - started < MANUAL_TIMEOUT_MS) {
        encoder.update();
        if (encoder.getPosition() < start) {
            detected = true;
            break;
        }
        if (millis() - lastTick >= 1000) {
            lastTick = millis();
            uint32_t elapsed = (millis() - started) / 1000;
            uint32_t remaining = (MANUAL_TIMEOUT_MS / 1000) - elapsed;
            Serial.print("[MANUAL TEST] Waiting for counterclockwise motion | remaining: ");
            Serial.print(remaining);
            Serial.println("s");
        }
        delay(2);
    }

    if (!detected) {
        TEST_IGNORE_MESSAGE("No counterclockwise interaction detected. Keep Serial Monitor open and repeat test while turning encoder.");
    }

    TEST_ASSERT_LESS_THAN_INT32(start, encoder.getPosition());
}

void test_manual_button_press_sets_latched_press_flag() {
    RotaryEncoder encoder(TEST_PIN_A, TEST_PIN_B, TEST_PIN_PUSH);
    encoder.begin();

    print_manual_header(
        "Button press latches once",
        "Press and release the encoder push button once within 12 seconds.",
        "Expected: isButtonPressed() returns true once, then false on the next call.");
    print_manual_timeout_reminders("Prepare to press and release button");

    bool detected = false;
    uint32_t started = millis();
    uint32_t lastTick = started;
    while (millis() - started < MANUAL_TIMEOUT_MS) {
        encoder.update();
        if (encoder.isButtonPressed()) {
            detected = true;
            break;
        }
        if (millis() - lastTick >= 1000) {
            lastTick = millis();
            uint32_t elapsed = (millis() - started) / 1000;
            uint32_t remaining = (MANUAL_TIMEOUT_MS / 1000) - elapsed;
            Serial.print("[MANUAL TEST] Waiting for button press | remaining: ");
            Serial.print(remaining);
            Serial.println("s");
        }
        delay(2);
    }

    if (!detected) {
        TEST_IGNORE_MESSAGE("No button press detected. Keep Serial Monitor open and repeat test while pressing encoder button.");
    }

    TEST_ASSERT_FALSE(encoder.isButtonPressed());
}

}  // namespace

void setup() {
    delay(1000);
    Serial.begin(115200);
    delay(300);
    Serial.println();
    Serial.println("RotaryEncoder test suite starting.");
    Serial.println("Manual tests are opt-in via RUN_MANUAL_ROTARY_TESTS=1.");
    Serial.println("Pins expected: A=21, B=22, PUSH=26");
    UNITY_BEGIN();

    RUN_TEST(test_constructor_initializes_defaults);
    RUN_TEST(test_reset_position_resets_position_and_last_position);
    RUN_TEST(test_has_changed_reflects_position_vs_last_position);
    RUN_TEST(test_is_button_pressed_returns_true_once_then_clears_latch);
    RUN_TEST(test_begin_initializes_encoder_state_from_pin_reads);

    // Manual verification tests: require a physically connected encoder on pins 21/22/26.
#if RUN_MANUAL_ROTARY_TESTS
    RUN_TEST(test_manual_clockwise_rotation_increases_position);
    RUN_TEST(test_manual_counterclockwise_rotation_decreases_position);
    RUN_TEST(test_manual_button_press_sets_latched_press_flag);
#else
    TEST_MESSAGE("Manual rotary interaction tests skipped (set RUN_MANUAL_ROTARY_TESTS=1 to enable).");
#endif

    UNITY_END();
}

void loop() {
}
