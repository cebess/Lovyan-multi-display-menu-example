#include "MenuConfig.h"

#include <cstddef>

namespace {

// Node type in a descriptor row. The active fields in MenuNodeDef depend on kind.
enum class NodeKind {
    ACTION,
    SUBMENU,
    VALUE,
};

// Stable action IDs used by descriptor rows. These map to executable lambdas.
enum class ActionId {
    SUBMENU_RUN,
    SUBMENU_PASS,
    SUBMENU_KICK,
    ABOUT,
};

/**
 * @brief Compile-time descriptor for one menu entry.
 *
 * Field usage by kind:
 * - ACTION: label, kind, actionId
 * - SUBMENU: label, kind, children, childCount
 * - VALUE: label, kind, valueBindingId, minValue, maxValue, step
 * Unused fields for a given kind are ignored by the tree builder.
 */
struct MenuNodeDef {
    const char* label;
    String desciption;
    NodeKind kind;
    const MenuNodeDef* children;
    size_t childCount;
};

// Board 1 submenu descriptor table.
const MenuNodeDef SUBMENU_PASS[] = {
    {"Over the Middle", NodeKind::VALUE, nullptr, 0},
    {"Halfback option", NodeKind::VALUE, ActionId::SUBMENU_PASS, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Four Verticals", NodeKind::VALUE, ActionId::SUBMENU_PASS, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Smash", NodeKind::ACTION, ActionId::SUBMENU_PASS, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Mesh", NodeKind::ACTION, ActionId::SUBMENU_PASS, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Slant-Flat", NodeKind::ACTION, ActionId::SUBMENU_PASS, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Sail", NodeKind::ACTION, ActionId::SUBMENU_PASS, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Stick", NodeKind::ACTION,ActionId::SUBMENU_PASS, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Flea Flicker", NodeKind::ACTION, ActionId::SUBMENU_PASS, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Hail Mary", NodeKind::ACTION, ActionId::SUBMENU_PASS, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Hook and Lateral", NodeKind::ACTION, ActionId::SUBMENU_PASS, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},

};

const MenuNodeDef SUBMENU_RUN[] = {
    {"Power", NodeKind::ACTION, ActionId::SUBMENU_RUN, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Counter", NodeKind::ACTION, ActionId::SUBMENU_RUN, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Trap", NodeKind::ACTION, ActionId::SUBMENU_RUN, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Isolation", NodeKind::ACTION, ActionId::SUBMENU_RUN, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Inside Zone", NodeKind::ACTION, ActionId::SUBMENU_RUN, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Outside Zone", NodeKind::ACTION, ActionId::SUBMENU_RUN, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Pitch", NodeKind::ACTION, ActionId::SUBMENU_RUN, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Sweep", NodeKind::ACTION, ActionId::SUBMENU_RUN, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Read Option", NodeKind::ACTION, ActionId::SUBMENU_RUN, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Jet Sweep", NodeKind::ACTION, ActionId::SUBMENU_RUN, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Short-Yardage Wedge", NodeKind::ACTION,ActionId::SUBMENU_RUN, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
};

const MenuNodeDef SUBMENU_KICK[] = {
    {"Punt", NodeKind::ACTION, ActionId::SUB_ITEM_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Field goal", NodeKind::ACTION, ActionId::SUB_ITEM_2, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
};

// Board 1 root descriptor table.
const MenuNodeDef OFFENSE_MENU[] = {
    {"Run", NodeKind::SUBMENU, SUBMENU_RUN, sizeof(SUBMENU_RUN) / sizeof(SUBMENU_RUN[0])},
    {"Pass", NodeKind::SUBMENU, SUBMENU_PASS, sizeof(SUBMENU_PASS) / sizeof(SUBMENU_PASS[0])},
    {"Kick", NodeKind::SUBMENU, SUBMENU_KICK, sizeof(SUBMENU_KICK) / sizeof(SUBMENU_KICK[0])},
};

// Board 2 root descriptor table.
const MenuNodeDef DEFENSE_MENU[] = {
    {"Man-to-man", NodeKind::ACTION, ActionId::SUB_ITEM_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Zone", NodeKind::ACTION, ActionId::SUB_ITEM_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Blitz", NodeKind::ACTION, ActionId::SUB_ITEM_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Prevent", NodeKind::ACTION, ActionId::SUB_ITEM_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
    {"Goal line", NodeKind::ACTION, ActionId::SUB_ITEM_1, ValueBindingId::BRIGHTNESS_1, ToggleBindingId::AUTO_ROTATE_1, 0, 0, 0, nullptr, 0},
};

// Map action IDs to executable callbacks bound to runtime objects.
std::function<void()> buildAction(ActionId id, const MenuBuildContext& ctx) {
    switch (id) {
        case ActionId::SUBMENU_RUN:
            return [ctx]() {
                ctx.display1->setCursor(10, 100);
                ctx.display1->fillRect(10, 90, 200, 20, TFT_BLACK);
                ctx.display1->setTextColor(TFT_YELLOW);
                ctx.display1->printf("Sub 1 Selected");
                ctx.menu1->redraw();
            };
        case ActionId::SUBMENU_PASS:
            return [ctx]() {
                ctx.display1->setCursor(10, 100);
                ctx.display1->fillRect(10, 90, 200, 20, TFT_BLACK);
                ctx.display1->setTextColor(TFT_MAGENTA);
                ctx.display1->printf("Sub 2 Selected");
                ctx.menu1->redraw();
            };
        case ActionId::SUBMENU_KICK:
            return [ctx]() {
                ctx.display1->setCursor(10, 100);
                ctx.display1->fillRect(10, 90, 200, 20, TFT_BLACK);
                ctx.display1->setTextColor(TFT_MAGENTA);
                ctx.display1->printf("Sub 2 Selected");
                ctx.menu1->redraw();
            };
        case ActionId::ABOUT:
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
    auto offenseRoot = buildMenuTree(OFFENSE_MENU, sizeof(OFFENSE_MENU) / sizeof(OFFENSE_MENU[0]), ctx);
    auto defenseRoot = buildMenuTree(DEFENSE_MENU, sizeof(DEFENSE_MENU) / sizeof(DEFENSE_MENU[0]), ctx);

    ctx.menu1->setRootMenu(offenseRoot);
    ctx.menu2->setRootMenu(defenseRoot);
}