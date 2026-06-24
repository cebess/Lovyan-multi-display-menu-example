#ifndef MENU_CONFIG_H
#define MENU_CONFIG_H

#include <LovyanGFX.hpp>
#include "MenuSystem.h"

/**
 * @brief Runtime handles used to materialize compile-time menu descriptors.
 *
 * The descriptor tables in MenuConfig.cpp are intentionally static and contain
 * only IDs and constants. This context provides the concrete display/menu
 * instances plus writable value/toggle bindings that the builder resolves.
 */
struct MenuBuildContext {
    // Display targets used by action callbacks.
    lgfx::LGFX_Device* display1;
    lgfx::LGFX_Device* display2;

    // Menu instances that receive redraw/select/value operations.
    MenuSystem* menu1;
    MenuSystem* menu2;

    // Writable numeric bindings used by VALUE menu items.
    int32_t* brightness1;
    int32_t* brightness2;

    // Writable boolean bindings used by TOGGLE menu items.
    bool* autoRotate1;
    bool* autoRotate2;
};

/**
 * @brief Build and attach both menu trees from compile-time descriptor tables.
 *
 * This function keeps main.cpp focused on high-level initialization by moving
 * menu structure definitions and callback wiring into MenuConfig.cpp.
 */
void configureMenus(const MenuBuildContext& ctx);

#endif