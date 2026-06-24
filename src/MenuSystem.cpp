#include "MenuSystem.h"
#include <LovyanGFX.hpp>

// MenuItem Implementation
MenuItem::MenuItem(const String& label, Type type, std::function<void()> action)
    : label(label), type(type), action(action), valuePtr(nullptr), 
      minValue(0), maxValue(100), step(1), boolPtr(nullptr), 
      currentBool(false), selectedIndex(0) {}

MenuItem::MenuItem(const String& label, std::vector<MenuItem*> subItems)
    : label(label), type(SUBMENU), action(nullptr), valuePtr(nullptr),
      minValue(0), maxValue(100), step(1), boolPtr(nullptr),
      currentBool(false), selectedIndex(0) {
    this->subItems = subItems;
}

MenuItem::MenuItem(const String& label, int32_t* valuePtr, int32_t min, int32_t max, int32_t step)
    : label(label), type(VALUE), action(nullptr), valuePtr(valuePtr),
      minValue(min), maxValue(max), step(step), boolPtr(nullptr),
      currentBool(false), selectedIndex(0) {}

MenuItem::MenuItem(const String& label, bool* boolPtr)
    : label(label), type(TOGGLE), action(nullptr), valuePtr(nullptr),
      minValue(0), maxValue(1), step(1), boolPtr(boolPtr),
      currentBool(false), selectedIndex(0) {
    if (boolPtr) currentBool = *boolPtr;
}

MenuItem::~MenuItem() {
    // Don't delete subItems here - they're managed by MenuSystem
}

void MenuItem::select() {
    if (action && type == ACTION) {
        action();
    }
}

String MenuItem::getValueString() const {
    if (type == VALUE && valuePtr) {
        return String(*valuePtr);
    } else if (type == TOGGLE) {
        return boolPtr ? (*boolPtr ? "ON" : "OFF") : "---";
    }
    return "";
}

void MenuItem::incrementValue() {
    if (type == VALUE && valuePtr) {
        *valuePtr += step;
        if (*valuePtr > maxValue) *valuePtr = maxValue;
    }
}

void MenuItem::decrementValue() {
    if (type == VALUE && valuePtr) {
        *valuePtr -= step;
        if (*valuePtr < minValue) *valuePtr = minValue;
    }
}

void MenuItem::toggle() {
    if (type == TOGGLE && boolPtr) {
        *boolPtr = !*boolPtr;
        currentBool = *boolPtr;
    }
}

// MenuSystem Implementation
MenuSystem::MenuSystem(DisplayBoard* boardPtr, const String& menuTitle)
    : board(boardPtr), currentMenu(nullptr), selectedIndex(0), 
      topVisible(0), visibleItems(8), _needsRedraw(true), title(menuTitle),
      fontSize(1), orientation(MenuSystem::LANDSCAPE) {
    // Set display rotation for landscape by default
    if (board && board->getDisplay()) {
        board->getDisplay()->setRotation(1);  // 1 = landscape
    }
}

void MenuSystem::ensureBackItems(std::vector<MenuItem*>& menu, bool isRoot) {
    if (!isRoot) {
        if (menu.empty() || menu[0]->getLabel() != "Back") {
            menu.insert(menu.begin(), new MenuItem("Back", MenuItem::ACTION));
        }
    }

    for (MenuItem* item : menu) {
        if (!item || item->getType() != MenuItem::SUBMENU) {
            continue;
        }
        auto& subItems = item->getSubItems();
        if (!subItems.empty()) {
            ensureBackItems(subItems, false);
        }
    }
}

void MenuSystem::setRootMenu(std::vector<MenuItem*> menu) {
    rootMenu = menu;
    ensureBackItems(rootMenu, true);
    currentMenu = &rootMenu;
    menuStack.clear();
    selectedStack.clear();
    topVisibleStack.clear();
    selectedIndex = 0;
    topVisible = 0;
    _needsRedraw = true;
}

void MenuSystem::begin() {
    if (board && board->getDisplay()) {
        auto display = board->getDisplay();
        display->fillScreen(TFT_BLACK);
        display->setTextColor(TFT_WHITE, TFT_BLACK);
        display->setTextSize(fontSize);
        renderMenu();
    }
}

void MenuSystem::update() {
    if (!board) return;
    
    auto encoder = board->getEncoder();
    if (!encoder) return;
    
    if (encoder->hasChanged()) {
        int direction = encoder->getPosition() - encoder->getPosition();
        // Actually we need to track position changes differently
        // This is simplified - in real use you'd store previous position
    }
    
    if (_needsRedraw) {
        renderMenu();
        _needsRedraw = false;
    }
}

void MenuSystem::renderMenu() {
    auto display = board->getDisplay();
    if (!display || !currentMenu) return;
    
    display->fillScreen(TFT_BLACK);
    display->setTextSize(fontSize);
    
    // Draw title
    display->setTextColor(TFT_CYAN, TFT_BLACK);
    display->setCursor(10, 5);
    display->print(title);
    
    // Draw separator line
    int separatorY = 8 * fontSize + 10;
    display->drawLine(0, separatorY, display->width(), separatorY, TFT_DARKGREY);
    
    // Calculate item height and spacing based on font size
    int itemHeight = 8 * fontSize + 4;  // Approximate height per item
    int titleHeight = separatorY;
    int maxVisible = (display->height() - titleHeight - 10) / itemHeight;
    if (maxVisible < 1) {
        maxVisible = 1;
    }
    visibleItems = maxVisible;
    int menuSize = static_cast<int>(currentMenu->size());
    if (menuSize <= 0) {
        return;
    }

    if (selectedIndex < 0) {
        selectedIndex = 0;
    } else if (selectedIndex >= menuSize) {
        selectedIndex = menuSize - 1;
    }
    
    // Adjust topVisible if needed
    if (selectedIndex < topVisible) {
        topVisible = selectedIndex;
    } else if (selectedIndex >= topVisible + maxVisible) {
        topVisible = selectedIndex - maxVisible + 1;
    }
    
    // Draw menu items
    int startY = titleHeight + 5;
    for (int i = topVisible; i < min(topVisible + maxVisible, menuSize); i++) {
        MenuItem* item = (*currentMenu)[i];
        bool selected = (i == selectedIndex);
        int yPos = startY + (i - topVisible) * itemHeight;
        drawMenuItem(item, i, yPos, selected, itemHeight);
    }
    
    // Draw scroll indicators
    if (topVisible > 0) {
        display->setCursor(display->width() - 15, startY);
        display->setTextColor(TFT_DARKGREY);
        display->print("^");
    }
    if (topVisible + maxVisible < menuSize) {
        display->setCursor(display->width() - 15, display->height() - 15);
        display->setTextColor(TFT_DARKGREY);
        display->print("v");
    }
}

void MenuSystem::drawMenuItem(MenuItem* item, int index, int yPos, bool selected, int height) {
    auto display = board->getDisplay();
    if (!display) return;
    
    // Background
    if (selected) {
        display->fillRect(0, yPos - 2, display->width(), height, TFT_NAVY);
    }
    
    // Item label
    uint16_t color = selected ? TFT_WHITE : TFT_LIGHTGREY;
    display->setTextColor(color);
    display->setCursor(10, yPos);
    
    String text = item->getLabel();
    
    // Add submenu indicator
    if (item->getType() == MenuItem::SUBMENU) {
        text += " >";
    }
    
    // Add value display
    if (item->getType() == MenuItem::VALUE || item->getType() == MenuItem::TOGGLE) {
        String valueStr = item->getValueString();
        display->setCursor(display->width() - 60, yPos);
        display->setTextColor(selected ? TFT_YELLOW : TFT_GREEN);
        display->print(valueStr);
        display->setCursor(10, yPos);
        display->setTextColor(color);
    }
    
    display->print(text);
}

void MenuSystem::goToParent() {
    if (!menuStack.empty()) {
        currentMenu = menuStack.back();
        menuStack.pop_back();

        selectedIndex = selectedStack.back();
        selectedStack.pop_back();

        topVisible = topVisibleStack.back();
        topVisibleStack.pop_back();
    } else {
        currentMenu = &rootMenu;
        selectedIndex = 0;
        topVisible = 0;
    }
    _needsRedraw = true;
}

void MenuSystem::selectCurrentItem() {
    if (!currentMenu || currentMenu->empty()) return;
    if (selectedIndex >= static_cast<int>(currentMenu->size())) return;
    
    MenuItem* item = (*currentMenu)[selectedIndex];

    if (item && item->getLabel() == "Back") {
        goToParent();
        return;
    }
    
    switch (item->getType()) {
        case MenuItem::ACTION:
            item->select();
            _needsRedraw = true;
            break;
            
        case MenuItem::SUBMENU: {
            auto& subItems = item->getSubItems();
            if (!subItems.empty()) {
                menuStack.push_back(currentMenu);
                selectedStack.push_back(selectedIndex);
                topVisibleStack.push_back(topVisible);
                currentMenu = &subItems;
                selectedIndex = 0;
                topVisible = 0;
                _needsRedraw = true;
            }
            break;
        }
        
        case MenuItem::TOGGLE:
            item->toggle();
            _needsRedraw = true;
            break;
            
        case MenuItem::VALUE:
            // For VALUE type, selection enters edit mode
            // In this example, we'll just show the value
            _needsRedraw = true;
            break;
    }
}

void MenuSystem::changeValue(int direction) {
    if (!currentMenu || currentMenu->empty()) return;
    if (selectedIndex >= static_cast<int>(currentMenu->size())) return;
    
    MenuItem* item = (*currentMenu)[selectedIndex];
    
    if (item->getType() == MenuItem::VALUE) {
        if (direction > 0) {
            item->incrementValue();
        } else {
            item->decrementValue();
        }
        _needsRedraw = true;
    }
}

void MenuSystem::selectUp() {
    if (!currentMenu || currentMenu->empty()) return;
    
    if (selectedIndex > 0) {
        selectedIndex--;
        _needsRedraw = true;
        
        // Scroll up if needed
        if (selectedIndex < topVisible) {
            topVisible = selectedIndex;
        }
    }
}

void MenuSystem::selectDown() {
    if (!currentMenu || currentMenu->empty()) return;
    
    if (selectedIndex < static_cast<int>(currentMenu->size()) - 1) {
        selectedIndex++;
        _needsRedraw = true;
        
        // Scroll down if needed
        int maxVisible = visibleItems > 0 ? visibleItems : 1;
        if (selectedIndex >= topVisible + maxVisible) {
            topVisible = selectedIndex - maxVisible + 1;
        }
    }
}

void MenuSystem::setOrientation(Orientation orient) {
    orientation = orient;
    if (board && board->getDisplay()) {
        // 0 = portrait, 1 = landscape
        board->getDisplay()->setRotation(orient == LANDSCAPE ? 1 : 0);
    }
    _needsRedraw = true;
}

void MenuSystem::setFontSize(uint8_t size) {
    // Clamp size between 1 and 7
    fontSize = (size < 1 ? 1 : (size > 7 ? 7 : size));
    _needsRedraw = true;
}
