#include "MenuConfig.h"

#include <cstddef>

namespace {

// Node type in a descriptor row. The active fields in MenuNodeDef depend on kind.
enum class NodeKind {
    ACTION,
    SUBMENU,
    VALUE,
    TOGGLE,
};

// Stable action IDs used by descriptor rows. These map to executable lambdas.
enum class ActionId {
    FLASH_SCREEN_1,
    FLASH_SCREEN_2,
    SUB_ITEM_1,
    SUB_ITEM_2,
    ABOUT_1,
    STATUS_2,
};

// IDs for int32_t value bindings in MenuBuildContext.
enum class ValueBindingId {
    BRIGHTNESS_1,
    BRIGHTNESS_2,
};

// IDs for bool toggle bindings in MenuBuildContext.
enum class ToggleBindingId {
    AUTO_ROTATE_1,
    AUTO_ROTATE_2,
};

/**
 * @brief Compile-time descriptor for one menu entry.
 *
 * Field usage by kind:
 * - ACTION: label, kind, actionId
 * - SUBMENU: label, kind, children, childCount
 * - VALUE: label, kind, valueBindingId, minValue, maxValue, step
 * - TOGGLE: label, kind, toggleBindingId
 *
 * Unused fields for a given kind are ignored by the tree builder.
 */
struct MenuNodeDef {
    const char* label;
    NodeKind kind;
    ActionId actionId;
    ValueBindingId valueBindingId;
    ToggleBindingId toggleBindingId;
    int32_t minValue;
    int32_t maxValue;
    int32_t step;
    const MenuNodeDef* children;
    size_t childCount;
};

// Board 1 submenu descriptor table.
const MenuNodeDef SUBMENU_1_ITEMS[] = {
    {"Sub Item 1", NodeKind::ACTION, ActionId::SUB_ITEM_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Sub Item 2", NodeKind::ACTION, ActionId::SUB_ITEM_2, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
};

// Board 1 root descriptor table.
const MenuNodeDef ROOT_MENU_1[] = {
    {"Flash Screen", NodeKind::ACTION, ActionId::FLASH_SCREEN_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Sub Menu", NodeKind::SUBMENU, ActionId::FLASH_SCREEN_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, SUBMENU_1_ITEMS, sizeof(SUBMENU_1_ITEMS) / sizeof(SUBMENU_1_ITEMS[0])},
    {"Brightness", NodeKind::VALUE, ActionId::FLASH_SCREEN_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 100, 5, nullptr, 0},
    {"Auto Rotate", NodeKind::TOGGLE, ActionId::FLASH_SCREEN_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"About", NodeKind::ACTION, ActionId::ABOUT_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"test1", NodeKind::ACTION, ActionId::ABOUT_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"test2", NodeKind::ACTION, ActionId::ABOUT_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"test3", NodeKind::ACTION, ActionId::ABOUT_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"test4", NodeKind::ACTION, ActionId::ABOUT_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"test5", NodeKind::ACTION, ActionId::ABOUT_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"test6", NodeKind::ACTION, ActionId::ABOUT_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"test7", NodeKind::ACTION, ActionId::ABOUT_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"test8", NodeKind::ACTION, ActionId::ABOUT_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},

};

// Board 2 root descriptor table.
const MenuNodeDef ROOT_MENU_2[] = {
    {"Flash Screen", NodeKind::ACTION, ActionId::FLASH_SCREEN_2, ValueBindingId::BRIGHTNESS_2, ToggleBindingId::AUTO_ROTATE_2, 0, 0, 0, nullptr, 0},
    {"Brightness", NodeKind::VALUE, ActionId::FLASH_SCREEN_2, ValueBindingId::BRIGHTNESS_2, ToggleBindingId::AUTO_ROTATE_2, 0, 100, 5, nullptr, 0},
    {"Auto Rotate", NodeKind::TOGGLE, ActionId::FLASH_SCREEN_2, ValueBindingId::BRIGHTNESS_2, ToggleBindingId::AUTO_ROTATE_2, 0, 0, 0, nullptr, 0},
    {"Status", NodeKind::ACTION, ActionId::STATUS_2, ValueBindingId::BRIGHTNESS_2, ToggleBindingId::AUTO_ROTATE_2, 0, 0, 0, nullptr, 0},
};

// Resolve VALUE binding IDs to concrete int32_t storage in the runtime context.
int32_t* resolveValueBinding(ValueBindingId id, const MenuBuildContext& ctx) {
    switch (id) {
        case ValueBindingId::BRIGHTNESS_1:
            return ctx.brightness1;
        case ValueBindingId::BRIGHTNESS_2:
            return ctx.brightness2;
    }
    return nullptr;
}

// Resolve TOGGLE binding IDs to concrete bool storage in the runtime context.
bool* resolveToggleBinding(ToggleBindingId id, const MenuBuildContext& ctx) {
    switch (id) {
        case ToggleBindingId::AUTO_ROTATE_1:
            return ctx.autoRotate1;
        case ToggleBindingId::AUTO_ROTATE_2:
            return ctx.autoRotate2;
    }
    return nullptr;
}

// Map action IDs to executable callbacks bound to runtime objects.
std::function<void()> buildAction(ActionId id, const MenuBuildContext& ctx) {
    switch (id) {
        case ActionId::FLASH_SCREEN_1:
            return [ctx]() {
                ctx.display1->fillScreen(TFT_RED);
                delay(200);
                ctx.display1->fillScreen(TFT_BLACK);
                ctx.menu1->redraw();
            };
        case ActionId::FLASH_SCREEN_2:
            return [ctx]() {
                ctx.display2->fillScreen(TFT_BLUE);
                delay(200);
                ctx.display2->fillScreen(TFT_BLACK);
                ctx.menu2->redraw();
            };
        case ActionId::SUB_ITEM_1:
            return [ctx]() {
                ctx.display1->setCursor(10, 100);
                ctx.display1->fillRect(10, 90, 200, 20, TFT_BLACK);
                ctx.display1->setTextColor(TFT_YELLOW);
                ctx.display1->printf("Sub 1 Selected");
                ctx.menu1->redraw();
            };
        case ActionId::SUB_ITEM_2:
            return [ctx]() {
                ctx.display1->setCursor(10, 100);
                ctx.display1->fillRect(10, 90, 200, 20, TFT_BLACK);
                ctx.display1->setTextColor(TFT_MAGENTA);
                ctx.display1->printf("Sub 2 Selected");
                ctx.menu1->redraw();
            };
        case ActionId::ABOUT_1:
            return [ctx]() {
                ctx.display1->fillScreen(TFT_BLACK);
                ctx.display1->setCursor(20, 50);
                ctx.display1->setTextColor(TFT_GREEN);
                ctx.display1->println("Display 1 Menu");
                ctx.display1->println("Version 1.0");
                ctx.display1->println("Rotate to navigate");
                ctx.display1->println("Press to select");
                delay(1500);
                ctx.menu1->redraw();
            };
        case ActionId::STATUS_2:
            return [ctx]() {
                ctx.display2->fillScreen(TFT_BLACK);
                ctx.display2->setCursor(20, 50);
                ctx.display2->setTextColor(TFT_CYAN);
                ctx.display2->printf("Brightness: %d%%\n", *ctx.brightness2);
                ctx.display2->printf("Auto Rotate: %s\n", *ctx.autoRotate2 ? "ON" : "OFF");
                delay(1500);
                ctx.menu2->redraw();
            };
    }

    return nullptr;
}

// Recursively convert descriptor rows into heap-allocated MenuItem trees.
std::vector<MenuItem*> buildMenuTree(const MenuNodeDef* defs, size_t count, const MenuBuildContext& ctx) {
    std::vector<MenuItem*> items;
    items.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        const MenuNodeDef& def = defs[i];
        switch (def.kind) {
            case NodeKind::ACTION:
                items.push_back(new MenuItem(def.label, MenuItem::ACTION, buildAction(def.actionId, ctx)));
                break;
            case NodeKind::SUBMENU: {
                auto children = buildMenuTree(def.children, def.childCount, ctx);
                items.push_back(new MenuItem(def.label, children));
                break;
            }
            case NodeKind::VALUE:
                items.push_back(new MenuItem(def.label,
                                             resolveValueBinding(def.valueBindingId, ctx),
                                             def.minValue,
                                             def.maxValue,
                                             def.step));
                break;
            case NodeKind::TOGGLE:
                items.push_back(new MenuItem(def.label, resolveToggleBinding(def.toggleBindingId, ctx)));
                break;
        }
    }

    return items;
}

}  // namespace

// Build both board menus and attach them to their MenuSystem instances.
void configureMenus(const MenuBuildContext& ctx) {
    auto menu1Root = buildMenuTree(ROOT_MENU_1, sizeof(ROOT_MENU_1) / sizeof(ROOT_MENU_1[0]), ctx);
    auto menu2Root = buildMenuTree(ROOT_MENU_2, sizeof(ROOT_MENU_2) / sizeof(ROOT_MENU_2[0]), ctx);

    ctx.menu1->setRootMenu(menu1Root);
    ctx.menu2->setRootMenu(menu2Root);
}