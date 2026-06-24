#include <Arduino.h>
#include <unity.h>
#include <LovyanGFX.hpp>

// Expose internals for focused unit assertions in test builds only.
#define private public
#include "DisplayBoard.h"
#undef private

// Pull production implementations into the test image when test_build_src is not enabled.
#include "../../src/RotaryEncoder.cpp"
#include "../../src/DisplayBoard.cpp"

namespace {
constexpr int8_t TEST_BACKLIGHT_PIN = 2;

#ifndef RUN_MANUAL_DISPLAY_TESTS
#define RUN_MANUAL_DISPLAY_TESTS 0
#endif

void print_manual_expectation(const char* message) {
    TEST_MESSAGE(message);
    Serial.print("[MANUAL CHECK] ");
    Serial.println(message);
}

void test_constructor_and_getters_store_values() {
    DisplayBoard board(nullptr, nullptr, -1, "Unit Test Board");

    TEST_ASSERT_NULL(board.getDisplay());
    TEST_ASSERT_NULL(board.getEncoder());
    TEST_ASSERT_EQUAL_STRING("Unit Test Board", board.getName().c_str());
    TEST_ASSERT_TRUE(board.backlightOn);
    TEST_ASSERT_EQUAL(-1, board.backlightPin);
}

void test_set_backlight_updates_internal_state_without_pin() {
    DisplayBoard board(nullptr, nullptr, -1, "No Pin Board");

    board.setBacklight(false);
    TEST_ASSERT_FALSE(board.backlightOn);

    board.setBacklight(true);
    TEST_ASSERT_TRUE(board.backlightOn);
}

void test_toggle_backlight_flips_internal_state() {
    DisplayBoard board(nullptr, nullptr, -1, "Toggle Board");

    TEST_ASSERT_TRUE(board.backlightOn);
    board.toggleBacklight();
    TEST_ASSERT_FALSE(board.backlightOn);
    board.toggleBacklight();
    TEST_ASSERT_TRUE(board.backlightOn);
}

void test_begin_configures_backlight_pin_and_sets_high() {
    DisplayBoard board(nullptr, nullptr, TEST_BACKLIGHT_PIN, "Backlight Board");

#if RUN_MANUAL_DISPLAY_TESTS
    print_manual_expectation(
        "If TEST_BACKLIGHT_PIN drives a real display backlight, it should turn ON after begin().");
#endif

    board.begin();

    // OUTPUT mode should drive as set by begin().
    TEST_ASSERT_EQUAL(HIGH, digitalRead(TEST_BACKLIGHT_PIN));
    TEST_ASSERT_TRUE(board.backlightOn);
}

void test_set_backlight_writes_pin_level_when_configured() {
    DisplayBoard board(nullptr, nullptr, TEST_BACKLIGHT_PIN, "Backlight Write Board");
    board.begin();

#if RUN_MANUAL_DISPLAY_TESTS
    print_manual_expectation(
        "After setBacklight(false), physical backlight should go OFF; after setBacklight(true), it should return ON.");
#endif

    board.setBacklight(false);
    TEST_ASSERT_FALSE(board.backlightOn);
    TEST_ASSERT_EQUAL(LOW, digitalRead(TEST_BACKLIGHT_PIN));

    board.setBacklight(true);
    TEST_ASSERT_TRUE(board.backlightOn);
    TEST_ASSERT_EQUAL(HIGH, digitalRead(TEST_BACKLIGHT_PIN));
}

void test_begin_with_non_null_display_pointer_path() {
    lgfx::LGFX_Device display;
    DisplayBoard board(&display, nullptr, -1, "Display Board");

#if RUN_MANUAL_DISPLAY_TESTS
    print_manual_expectation(
        "With a real initialized panel, begin() should initialize it, set rotation to 0, and clear screen to black.");
#endif

    board.begin();

    // Method path is exercised; pointer identity remains valid.
    TEST_ASSERT_EQUAL_PTR(&display, board.getDisplay());
}

void test_clear_display_with_non_null_display_pointer_path() {
    lgfx::LGFX_Device display;
    DisplayBoard board(&display, nullptr, -1, "Display Clear Board");

#if RUN_MANUAL_DISPLAY_TESTS
    print_manual_expectation(
        "With a real initialized panel, clearDisplay() should immediately paint the full screen black.");
#endif

    board.clearDisplay();

    TEST_ASSERT_EQUAL_PTR(&display, board.getDisplay());
}

void test_clear_display_is_safe_with_null_display() {
    DisplayBoard board(nullptr, nullptr, -1, "Null Display");

    board.clearDisplay();

    TEST_PASS();
}

void test_update_is_safe_with_null_encoder() {
    DisplayBoard board(nullptr, nullptr, -1, "Null Encoder");

    board.update();

    TEST_PASS();
}

void test_begin_and_update_with_real_encoder_path() {
    // Use valid GPIO numbers for ESP32 to exercise encoder-present code path.
    RotaryEncoder encoder(21, 22, 26);
    DisplayBoard board(nullptr, &encoder, -1, "Encoder Board");

    board.begin();
    board.update();

    // No motion expected without hardware interaction.
    TEST_ASSERT_EQUAL(0, encoder.getPosition());
}

}  // namespace

void setup() {
    delay(1000);
    UNITY_BEGIN();

    RUN_TEST(test_constructor_and_getters_store_values);
    RUN_TEST(test_set_backlight_updates_internal_state_without_pin);
    RUN_TEST(test_toggle_backlight_flips_internal_state);
    RUN_TEST(test_begin_configures_backlight_pin_and_sets_high);
    RUN_TEST(test_set_backlight_writes_pin_level_when_configured);
    RUN_TEST(test_begin_with_non_null_display_pointer_path);
    RUN_TEST(test_clear_display_with_non_null_display_pointer_path);
    RUN_TEST(test_clear_display_is_safe_with_null_display);
    RUN_TEST(test_update_is_safe_with_null_encoder);
    RUN_TEST(test_begin_and_update_with_real_encoder_path);

#if RUN_MANUAL_DISPLAY_TESTS
    TEST_MESSAGE("Manual DisplayBoard visual checks are enabled.");
#else
    TEST_MESSAGE("Manual DisplayBoard visual checks skipped (set RUN_MANUAL_DISPLAY_TESTS=1 to enable).");
#endif

    UNITY_END();
}

void loop() {
}
