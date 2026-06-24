#include <Arduino.h>
#include <unity.h>
#include <LovyanGFX.hpp>

// White-box access for targeted state assertions in tests.
#define private public
#include "MenuSystem.h"
#undef private

// Pull production implementation into test image when test_build_src is not enabled.
#include "../../src/RotaryEncoder.cpp"
#include "../../src/DisplayBoard.cpp"
#include "../../src/MenuSystem.cpp"

namespace {

constexpr uint32_t MANUAL_TIMEOUT_MS = 12000;

#ifndef RUN_MANUAL_MENU_TESTS
#define RUN_MANUAL_MENU_TESTS 0
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

class TrackingAction {
public:
    int calls = 0;
    void operator()() { ++calls; }
};

void test_menu_item_action_select_invokes_callback() {
    TrackingAction action;
    MenuItem item("Run", MenuItem::ACTION, [&action]() { action(); });

    item.select();

    TEST_ASSERT_EQUAL(1, action.calls);
    TEST_ASSERT_EQUAL(MenuItem::ACTION, item.getType());
}

void test_menu_item_submenu_constructor_stores_children() {
    std::vector<MenuItem*> children;
    children.push_back(new MenuItem("Child", MenuItem::ACTION));
    MenuItem submenu("Parent", children);

    TEST_ASSERT_EQUAL(MenuItem::SUBMENU, submenu.getType());
    TEST_ASSERT_EQUAL_STRING("Parent", submenu.getLabel().c_str());
    TEST_ASSERT_EQUAL(1, static_cast<int>(submenu.getSubItems().size()));
    TEST_ASSERT_EQUAL_STRING("Child", submenu.getSubItems()[0]->getLabel().c_str());
}

void test_menu_item_value_constructor_increment_decrement_and_clamp() {
    int32_t value = 50;
    MenuItem item("Brightness", &value, 0, 100, 5);

    TEST_ASSERT_EQUAL_STRING("50", item.getValueString().c_str());

    item.incrementValue();
    TEST_ASSERT_EQUAL(55, value);

    item.decrementValue();
    TEST_ASSERT_EQUAL(50, value);

    value = 99;
    item.incrementValue();
    TEST_ASSERT_EQUAL(100, value);
    item.incrementValue();
    TEST_ASSERT_EQUAL(100, value);

    value = 1;
    item.decrementValue();
    TEST_ASSERT_EQUAL(0, value);
    item.decrementValue();
    TEST_ASSERT_EQUAL(0, value);
}

void test_menu_item_toggle_constructor_and_toggle() {
    bool enabled = false;
    MenuItem item("Auto Rotate", &enabled);

    TEST_ASSERT_EQUAL(MenuItem::TOGGLE, item.getType());
    TEST_ASSERT_EQUAL_STRING("OFF", item.getValueString().c_str());

    item.toggle();
    TEST_ASSERT_TRUE(enabled);
    TEST_ASSERT_EQUAL_STRING("ON", item.getValueString().c_str());

    item.toggle();
    TEST_ASSERT_FALSE(enabled);
    TEST_ASSERT_EQUAL_STRING("OFF", item.getValueString().c_str());
}

void test_menu_system_constructor_defaults_and_null_board_are_safe() {
    MenuSystem menu(nullptr, "Main Menu");

    TEST_ASSERT_EQUAL_STRING("Main Menu", menu.title.c_str());
    TEST_ASSERT_EQUAL(MenuSystem::LANDSCAPE, menu.orientation);
    TEST_ASSERT_FALSE(menu.isEditingValue());
    TEST_ASSERT_TRUE(menu.needsRedraw());
    TEST_ASSERT_EQUAL(1, menu.getFontSize());
}

void test_set_font_size_clamps_and_marks_redraw() {
    MenuSystem menu(nullptr, "Main Menu");

    menu.redraw();
    menu.setFontSize(0);
    TEST_ASSERT_EQUAL(1, menu.getFontSize());
    TEST_ASSERT_TRUE(menu.needsRedraw());

    menu.redraw();
    menu.setFontSize(9);
    TEST_ASSERT_EQUAL(7, menu.getFontSize());
    TEST_ASSERT_TRUE(menu.needsRedraw());
}

void test_set_orientation_updates_state_and_marks_redraw() {
    MenuSystem menu(nullptr, "Main Menu");

    menu.setOrientation(MenuSystem::PORTRAIT);
    TEST_ASSERT_EQUAL(MenuSystem::PORTRAIT, menu.getOrientation());
    TEST_ASSERT_TRUE(menu.needsRedraw());
}

void test_set_root_menu_inserts_back_item_for_submenus() {
    MenuSystem menu(nullptr, "Main Menu");
    std::vector<MenuItem*> root;
    std::vector<MenuItem*> children;
    children.push_back(new MenuItem("Child Action", MenuItem::ACTION));
    root.push_back(new MenuItem("Sub Menu", children));

    menu.setRootMenu(root);

    TEST_ASSERT_EQUAL(1, static_cast<int>(menu.rootMenu.size()));
    TEST_ASSERT_EQUAL(MenuItem::SUBMENU, menu.rootMenu[0]->getType());
    TEST_ASSERT_EQUAL(2, static_cast<int>(menu.rootMenu[0]->getSubItems().size()));
    TEST_ASSERT_EQUAL_STRING("Back", menu.rootMenu[0]->getSubItems()[0]->getLabel().c_str());
    TEST_ASSERT_EQUAL_PTR(&menu.rootMenu, menu.currentMenu);
    TEST_ASSERT_EQUAL(0, menu.selectedIndex);
    TEST_ASSERT_EQUAL(0, menu.topVisible);
    TEST_ASSERT_TRUE(menu.needsRedraw());
}

void test_select_current_item_handles_action_toggle_value_and_submenu() {
    bool enabled = false;
    int32_t brightness = 10;
    TrackingAction action;

    std::vector<MenuItem*> root;
    root.push_back(new MenuItem("Run", MenuItem::ACTION, [&action]() { action(); }));
    root.push_back(new MenuItem("Brightness", &brightness, 0, 100, 5));
    root.push_back(new MenuItem("Auto Rotate", &enabled));
    std::vector<MenuItem*> submenuItems;
    submenuItems.push_back(new MenuItem("Child Action", MenuItem::ACTION));
    root.push_back(new MenuItem("Sub Menu", submenuItems));

    MenuSystem menu(nullptr, "Main Menu");
    menu.setRootMenu(root);

    menu.selectedIndex = 0;
    menu.selectCurrentItem();
    TEST_ASSERT_EQUAL(1, action.calls);
    TEST_ASSERT_TRUE(menu.needsRedraw());

    menu.redraw();
    menu.selectedIndex = 1;
    TEST_ASSERT_FALSE(menu.isEditingValue());
    menu.selectCurrentItem();
    TEST_ASSERT_TRUE(menu.isEditingValue());

    menu.selectedIndex = 2;
    menu.selectCurrentItem();
    TEST_ASSERT_TRUE(enabled);
    TEST_ASSERT_TRUE(menu.needsRedraw());

    menu.redraw();
    menu.selectedIndex = 3;
    menu.selectCurrentItem();
    TEST_ASSERT_EQUAL(1, static_cast<int>(menu.menuStack.size()));
    TEST_ASSERT_EQUAL(0, menu.selectedIndex);
    TEST_ASSERT_FALSE(menu.isEditingValue());
}

void test_go_to_parent_restores_previous_menu_state_and_clears_editing() {
    MenuSystem menu(nullptr, "Main Menu");
    std::vector<MenuItem*> root;
    root.push_back(new MenuItem("Root Action", MenuItem::ACTION));
    std::vector<MenuItem*> childItems;
    childItems.push_back(new MenuItem("Child Action", MenuItem::ACTION));
    root.push_back(new MenuItem("Sub Menu", childItems));

    menu.setRootMenu(root);
    menu.selectedIndex = 1;
    menu.selectCurrentItem();
    menu.selectedIndex = 0;
    menu.editingValue = true;

    menu.goToParent();

    TEST_ASSERT_FALSE(menu.isEditingValue());
    TEST_ASSERT_EQUAL_PTR(&menu.rootMenu, menu.currentMenu);
    TEST_ASSERT_EQUAL(1, menu.selectedIndex);
    TEST_ASSERT_EQUAL(0, menu.topVisible);
    TEST_ASSERT_TRUE(menu.needsRedraw());
}

void test_change_value_only_affects_value_items() {
    int32_t brightness = 50;
    bool enabled = false;
    MenuSystem menu(nullptr, "Main Menu");
    std::vector<MenuItem*> root;
    root.push_back(new MenuItem("Brightness", &brightness, 0, 100, 5));
    root.push_back(new MenuItem("Auto Rotate", &enabled));
    menu.setRootMenu(root);

    menu.selectedIndex = 0;
    menu.changeValue(1);
    TEST_ASSERT_EQUAL(55, brightness);
    TEST_ASSERT_TRUE(menu.needsRedraw());

<<<<<<< HEAD
    menu.redraw();
=======
    // Manually clear the flag (white-box access) since update() requires valid board/encoder
    menu._needsRedraw = false;
>>>>>>> 2bf78f3fb220d3f5bc5c9128ee644021a3d3e3ca
    menu.changeValue(-1);
    TEST_ASSERT_EQUAL(50, brightness);
    TEST_ASSERT_TRUE(menu.needsRedraw());

<<<<<<< HEAD
    menu.redraw();
=======
    // Manually clear the flag again before testing non-VALUE item
    menu._needsRedraw = false;
>>>>>>> 2bf78f3fb220d3f5bc5c9128ee644021a3d3e3ca
    menu.selectedIndex = 1;
    menu.changeValue(1);
    TEST_ASSERT_FALSE(menu.needsRedraw());
    TEST_ASSERT_FALSE(enabled);
}

void test_select_up_and_down_move_selection_and_scroll_state() {
    MenuSystem menu(nullptr, "Main Menu");
    std::vector<MenuItem*> root;
    root.push_back(new MenuItem("One", MenuItem::ACTION));
    root.push_back(new MenuItem("Two", MenuItem::ACTION));
    root.push_back(new MenuItem("Three", MenuItem::ACTION));
    menu.setRootMenu(root);
    menu.visibleItems = 2;

    menu.selectDown();
    TEST_ASSERT_EQUAL(1, menu.selectedIndex);
    TEST_ASSERT_TRUE(menu.needsRedraw());

    menu.redraw();
    menu.selectDown();
    TEST_ASSERT_EQUAL(2, menu.selectedIndex);
    TEST_ASSERT_EQUAL(1, menu.topVisible);

    menu.selectUp();
    TEST_ASSERT_EQUAL(1, menu.selectedIndex);
}

void test_update_with_null_board_is_safe() {
    MenuSystem menu(nullptr, "Main Menu");
    menu.update();
    TEST_PASS();
}

void test_begin_with_null_board_is_safe() {
    MenuSystem menu(nullptr, "Main Menu");
    menu.begin();
    TEST_PASS();
}

#if RUN_MANUAL_MENU_TESTS
void test_manual_render_menu_should_show_title_and_items() {
    lgfx::LGFX_Device display;
    DisplayBoard board(&display, nullptr, -1, "Menu Board");
    MenuSystem menu(&board, "Main Menu");

    std::vector<MenuItem*> root;
    root.push_back(new MenuItem("Flash Screen", MenuItem::ACTION));
    root.push_back(new MenuItem("Brightness", new int32_t(50), 0, 100, 5));
    menu.setRootMenu(root);
    menu.setFontSize(2);

    print_manual_header(
        "Menu rendering",
        "Power the display, then run this test with the board connected.",
        "Expected: the title 'Main Menu' appears, menu rows are visible, and the brightness row shows its value on the right.");

    menu.begin();
    TEST_PASS();
}

void test_manual_draw_menu_item_edit_mode_should_highlight_value() {
    lgfx::LGFX_Device display;
    DisplayBoard board(&display, nullptr, -1, "Menu Board");
    MenuSystem menu(&board, "Main Menu");

    std::vector<MenuItem*> root;
    root.push_back(new MenuItem("Brightness", new int32_t(50), 0, 100, 5));
    menu.setRootMenu(root);
    menu.selectedIndex = 0;
    menu.editingValue = true;

    print_manual_header(
        "Selected value item in edit mode",
        "Enter the Brightness item, press to enter edit mode, and observe the display.",
        "Expected: the selected value should be highlighted differently from normal selection, indicating value-edit mode.");

    menu.begin();
    TEST_PASS();
}
#endif

}  // namespace

void setup() {
    delay(1000);
    Serial.begin(115200);
    delay(300);
    Serial.println();
    Serial.println("MenuSystem test suite starting.");
    Serial.println("Manual tests are opt-in via RUN_MANUAL_MENU_TESTS=1.");
    UNITY_BEGIN();

    RUN_TEST(test_menu_item_action_select_invokes_callback);
    RUN_TEST(test_menu_item_submenu_constructor_stores_children);
    RUN_TEST(test_menu_item_value_constructor_increment_decrement_and_clamp);
    RUN_TEST(test_menu_item_toggle_constructor_and_toggle);
    RUN_TEST(test_menu_system_constructor_defaults_and_null_board_are_safe);
    RUN_TEST(test_set_font_size_clamps_and_marks_redraw);
    RUN_TEST(test_set_orientation_updates_state_and_marks_redraw);
    RUN_TEST(test_set_root_menu_inserts_back_item_for_submenus);
    RUN_TEST(test_select_current_item_handles_action_toggle_value_and_submenu);
    RUN_TEST(test_go_to_parent_restores_previous_menu_state_and_clears_editing);
    RUN_TEST(test_change_value_only_affects_value_items);
    RUN_TEST(test_select_up_and_down_move_selection_and_scroll_state);
    RUN_TEST(test_update_with_null_board_is_safe);
    RUN_TEST(test_begin_with_null_board_is_safe);
#if RUN_MANUAL_MENU_TESTS
    RUN_TEST(test_manual_render_menu_should_show_title_and_items);
    RUN_TEST(test_manual_draw_menu_item_edit_mode_should_highlight_value);
#else
    TEST_MESSAGE("Manual MenuSystem visual tests skipped (set RUN_MANUAL_MENU_TESTS=1 to enable).");
#endif

    UNITY_END();
}

void loop() {
}
