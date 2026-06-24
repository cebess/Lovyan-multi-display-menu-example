#include <LovyanGFX.hpp>
#include "DisplayBoard.h"
#include "MenuSystem.h"
#include "MenuConfig.h"

// Parameterized ST7789 display configuration class
class LGFX_Display : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 _panel;
  lgfx::Bus_SPI _bus;

public:
  LGFX_Display(int pinDc, int pinCs, int pinRst) {
    {
      auto cfg = _bus.config();

      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read  = 16000000;
      cfg.spi_3wire  = false;
      cfg.use_lock   = true;
      cfg.dma_channel = 1;

      cfg.pin_sclk = 18;
      cfg.pin_mosi = 23;
      cfg.pin_miso = -1;
      cfg.pin_dc = pinDc;

      _bus.config(cfg);
    }

    {
      auto cfg = _panel.config();

      cfg.pin_cs  = pinCs;
      cfg.pin_rst = pinRst;
      cfg.pin_busy = -1;

      cfg.memory_width  = 240;
      cfg.memory_height = 320;
      cfg.panel_width   = 240;
      cfg.panel_height  = 320;
      cfg.offset_x = 0;
      cfg.offset_y = 0;

      _panel.config(cfg);
    }

    _panel.setBus(&_bus);
    setPanel(&_panel);
  }
};

// Global objects
LGFX_Display display1(2, 5, 4);
LGFX_Display display2(15, 14, 12);

// Encoder objects
RotaryEncoder encoder1(21, 22, 26);  // A, B, PUSH for display 1
RotaryEncoder encoder2(32, 33, 25);  // A, B, PUSH for display 2

// Display board objects
DisplayBoard board1(&display1, &encoder1, 19, "Board 1");
DisplayBoard board2(&display2, &encoder2, 13, "Board 2");

// Menu systems
MenuSystem menu1(&board1, "Main Menu 1");
MenuSystem menu2(&board2, "Main Menu 2");

// Global variables bound to VALUE/TOGGLE menu items via MenuBuildContext.
int32_t brightness1 = 50;
int32_t brightness2 = 50;
bool autoRotate1 = false;
bool autoRotate2 = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting dual display menu system...");
  
  // Initialize boards
  board1.begin();
  board2.begin();
 
  // // Show initial messages
  // display1.setCursor(10, 50);
  // display1.setTextColor(TFT_GREEN);
  // display1.println("Rotate encoder");
  // display1.println("to navigate menu");
  // display1.println("Press to select");
  
  // display2.setCursor(10, 50);
  // display2.setTextColor(TFT_CYAN);
  // display2.println("Rotate encoder");
  // display2.println("to navigate menu");
  // display2.println("Press to select");
  
  // delay(2000);
  
  // Build both menus from compile-time descriptor tables in MenuConfig.cpp.
  MenuBuildContext menuContext = {
    &display1,
    &display2,
    &menu1,
    &menu2,
    &brightness1,
    &brightness2,
    &autoRotate1,
    &autoRotate2,
  };
  configureMenus(menuContext);
  
  // Set to landscape with font size 2 (larger text)
  menu1.setFontSize(2);
  menu1.setOrientation(MenuSystem::LANDSCAPE);
  // Set to portrait with font size 2 (larger text)
  menu2.setFontSize(2);
  menu2.setOrientation(MenuSystem::PORTRAIT);

  // Initialize menu systems
  menu1.begin();
  menu2.begin();

  menu1.redraw();
  menu2.redraw();
}

// Variables to track encoder state for menu control
int lastPos1 = 0;
int lastPos2 = 0;
bool button1State = false;
bool button2State = false;
int encRemainder1 = 0;
int encRemainder2 = 0;
const int ENCODER_COUNTS_PER_MENU_STEP = 2;

void loop() {
  // Update board states
  board1.update();
  board2.update();
  
  // Process encoder 1
  RotaryEncoder* enc1 = board1.getEncoder();
  if (enc1) {
    int currentPos = enc1->getPosition();
    if (currentPos != lastPos1) {
      int diff = currentPos - lastPos1;
      encRemainder1 += diff;
      if (menu1.isEditingValue()) {
        while (encRemainder1 >= ENCODER_COUNTS_PER_MENU_STEP) {
          menu1.changeValue(1);
          encRemainder1 -= ENCODER_COUNTS_PER_MENU_STEP;
        }
        while (encRemainder1 <= -ENCODER_COUNTS_PER_MENU_STEP) {
          menu1.changeValue(-1);
          encRemainder1 += ENCODER_COUNTS_PER_MENU_STEP;
        }
      } else {
        while (encRemainder1 >= ENCODER_COUNTS_PER_MENU_STEP) {
          menu1.selectDown();
          encRemainder1 -= ENCODER_COUNTS_PER_MENU_STEP;
        }
        while (encRemainder1 <= -ENCODER_COUNTS_PER_MENU_STEP) {
          menu1.selectUp();
          encRemainder1 += ENCODER_COUNTS_PER_MENU_STEP;
        }
      }
      lastPos1 = currentPos;
    }
    
    // Check button press
    if (enc1->isButtonPressed()) {
      menu1.selectCurrentItem();
    }
  }
  
  // Process encoder 2
  RotaryEncoder* enc2 = board2.getEncoder();
  if (enc2) {
    int currentPos = enc2->getPosition();
    if (currentPos != lastPos2) {
      int diff = currentPos - lastPos2;
      encRemainder2 += diff;
      if (menu2.isEditingValue()) {
        while (encRemainder2 >= ENCODER_COUNTS_PER_MENU_STEP) {
          menu2.changeValue(1);
          encRemainder2 -= ENCODER_COUNTS_PER_MENU_STEP;
        }
        while (encRemainder2 <= -ENCODER_COUNTS_PER_MENU_STEP) {
          menu2.changeValue(-1);
          encRemainder2 += ENCODER_COUNTS_PER_MENU_STEP;
        }
      } else {
        while (encRemainder2 >= ENCODER_COUNTS_PER_MENU_STEP) {
          menu2.selectDown();
          encRemainder2 -= ENCODER_COUNTS_PER_MENU_STEP;
        }
        while (encRemainder2 <= -ENCODER_COUNTS_PER_MENU_STEP) {
          menu2.selectUp();
          encRemainder2 += ENCODER_COUNTS_PER_MENU_STEP;
        }
      }
      lastPos2 = currentPos;
    }
    
    if (enc2->isButtonPressed()) {
      menu2.selectCurrentItem();
    }
  }
  
  // Update menu systems (they'll handle redraws)
  menu1.update();
  menu2.update();
  
  delay(20);
}