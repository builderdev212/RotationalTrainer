// Since this is a header file, you have to import the arduino library.
#include <Arduino.h>

// Import the functions needed for the display.
#include <TFT_eSPI.h>
#include <SPI.h>

// Import the other files that contain important functions.
#include "controls.h"
#include "otherFunctions.h"

// Import the functions used to display pngs.
#include "pngFunctions.h"

// Import the png.
#include "manual.h"

// Defines the fonts that will be used.
#define sans &FreeSans9pt7b
#define sansBold &FreeSansBold9pt7b
#define titleFont &FreeSansBold24pt7b

const String VERSION_NUMBER = "V1.0.1";

class UI {
  public:
    // Main function that will run through the loop.
    void mainloop(TFT_eSPI &_tft) {
      // display the boot image.
      boot(_tft);

      // setup anything that is needed while the boot menu is there.
      setup_controls();

      // Go into the actual program.
      delay(1000);
      // Set the inital background to white.
      _tft.fillScreen(TFT_WHITE);
      // Create the menu.
      createMenu(_tft);
      // Create the bottom bar.
      createBar(_tft);
      // Draw the currently selected button to the screen.
      hoverControl(_tft, currentSelectedColumn);
      pageSelect(_tft, currSel);

      // Draw the currently selected page.
      changePage(_tft);

      // Loop to run through the actions that could happen.
      while (true) {
        // Update the bottom info.
        updateBar(_tft);

        // Update the controls and what buttons are selected.
        updateLocation(_tft);
        selectedButton(_tft);

        // If a new page has been selected, change to it.
        changePage(_tft);

        // If the info page is currently selected, update the button sensors.
        updateInfo(_tft);
      }
    }
  private:
    /*
      GENERAL DISPLAY VARIABLES
    */

    // Display dimensions.
    const uint16_t D_WIDTH = 320;
    const uint16_t D_HEIGHT = 240;

    /*
      SENSOR VARIABLES
    */

    // Joystick pins.
    const int X_PIN = 36;
    const int Y_PIN = 39;
    const int Z_PIN = 32;

    // Button pins.
    const int FRONT0 = 33;
    const int BACK0 = 25;

    // Joystick class.
    joystick Joystick;

    but but1;
    but but2;

    // Stores the past joystick value.
    int pastX = 0;
    int pastY = 0;

    /*
      TIME VARIABLES
    */

    // Time variables.
    unsigned long startTime = millis();
    unsigned long currentTime = millis();

    /*
      MENU VARIABLES
    */

    // Array to hold which page is active.
    bool menuPage[4] = {true, false, false, false};

    // Stores the currently selected page.
    int currSel = 0;
    int lastCurrSel = 92;

    // The array is layed out as follows: {{side menu values}, {home page values}, {info page values}, {help page values}, {bench page values}}
    bool buttonLayout[5][4] = {
      {true, false, false, false},
      {false},
      {},
      {},
      {false}
    };

    // This stores the location of the currently hovered option
    int currentSelectedRow = 0;
    int currentSelectedColumn = 0;

    // Variables to store the other text stored.
    const String menuTitle = "Menu";

    // Variable to store the current page selected.
    uint8_t pageOn = 0;

    // Variables to hold the reps.
    int reps = 0;

    // Strings to be displayed.
    const String repString = "Reps:";
    const String timeString = "Time:";

    // Variables for bar color.
    const uint32_t BAR_COLOR = TFT_ORANGE;
    const uint32_t BAR_TEXT_COLOR = TFT_BLACK;

    /*
      BUTTON VARIABLES
    */

    // Button order.
    const int HOME = 0;
    const int BENCH = 1;
    const int INFO = 2;
    const int HELP = 3;

    // Button hover/selection colors.
    const uint32_t SELECTED_COLOR = TFT_RED;
    const uint32_t HOVER_COLOR = TFT_BLACK;

    // Variables for button sizes.
    const uint16_t BUTTON_W = 75;
    const uint16_t BUTTON_H = 24;
    const uint16_t EDGE_R = 3;
    const uint16_t BUTTON_BORDER = 5;

    // Array to store the button names.
    String buttonNames[4] = {"Home", "Bench", "Info", "Help"};
    String *buttonLabels = buttonNames;

    // Variables for button color.
    const uint32_t BUTTON_COLOR = TFT_DARKGREY;
    const uint32_t SIDE_MENU_COLOR = TFT_ORANGE;
    const uint32_t BUTTON_TEXT_COLOR = TFT_BLACK;

    /*
      BOOT VARIABLES
    */

    // Colors for the boot screen
    const uint32_t BOOT_BG_COLOR = TFT_GOLD;
    const uint32_t BOOT_TEXT_COLOR = TFT_BLACK;

    // Text for the boot screen.
    const String TOP_ROW = "Rotational";
    const String BOTTOM_ROW = "Trainer";

    // Text offset.
    const uint8_t BOOT_TEXT_MARGIN = 3;

    /*
      PAGE VARIABLES
    */

    const int PAGE_W = (D_WIDTH - (BUTTON_W + (BUTTON_BORDER * 2)));
    const int PAGE_H = D_HEIGHT - 24;
    const String RESET_TEXT = "Reset";

    const int QR_CODE_SIDE_L = 74;

    const String HELP_TEXT[9] = {"To remove the display,", "remove both of the usb", "type c connectors. Keep", "in mind connector labeled", "S is where the sensor",
                                 "cable plugs in.", "P is where the", "power cable", "plugs in."};

    /*
      CONSTANTS FOR THE STEP LOCATION DISPLAY.
    */

    // Constants for the model location.
    const int W_MOD = 125;
    const int H_MOD = 200;
    const int X_MOD = PAGE_W / 2 - W_MOD / 2;
    const int Y_MOD = PAGE_H / 2 - H_MOD / 2;

    // Constants for the model size.
    const int ADJ_BAR_W = 55;
    const int ADJ_BAR_H = 10;
    const int MID_BAR_W = 15;
    const int MID_BAR_Y_OFFSET = 1;

    // Constants for the feet enclosure.
    const int SIDE_OFFSET = 5;
    const int ENCLOSURE_WIDTH = 5;
    const int MID_W = 40;

    // Constants for model colors.
    const int END_BAR_COLOR = TFT_DARKGREY;
    const int ADJ_BAR_COLOR = TFT_BLACK;
    const int FOOT_ENCLOSURE_COLOR = TFT_BLACK;

    // Constants for button indicators.
    const int IND_SIDE_LEN = 4;
    const uint32_t OFF_COLOR = TFT_RED;
    const uint32_t ON_COLOR = TFT_GREEN;

    /*
      CONFIRMATION PAGE VARIABLES
    */

    // Strings to be displayed on the confirmation page.
    const String TOP_CONF = "Have you set the";
    const String BOTTOM_CONF = "resistance to max?";
    const String N_CONF = "No";
    const String Y_CONF = "Yes";
    const String NO_TOP = "Please change the";
    const String NO_BOT = "resistance to max.";

    // Strings to determine the button size.
    const int CONF_BUT_W = 50;
    const int CONF_BUT_H = 24;

    bool confHov[2] = {true, false};

    /*
      BOOT AND SETUP FUNCTIONS
    */

    // Function to display the boot screen.
    void boot(TFT_eSPI &_tft) {
      // Set the background color.
      _tft.fillScreen(BOOT_BG_COLOR);

      // Display the text.
      _tft.setTextColor(BOOT_TEXT_COLOR);
      _tft.setFreeFont(titleFont);
      _tft.drawString(TOP_ROW, ((D_WIDTH / 2) - (_tft.textWidth(TOP_ROW) / 2)), ((D_HEIGHT / 2) - _tft.fontHeight() - BOOT_TEXT_MARGIN));
      _tft.drawString(BOTTOM_ROW, ((D_WIDTH / 2) - (_tft.textWidth(BOTTOM_ROW) / 2)), ((D_HEIGHT / 2) + BOOT_TEXT_MARGIN));
    }

    // Setup the joystick and button sets.
    void setup_controls() {
      // Setup the joystick on the X and Y pin.
      Joystick.begin(X_PIN, Y_PIN, Z_PIN);

      // Setup the two sets of buttons for rep counting.
    }

    /*
      USER CONTROL FUNCTIONS
    */

    // User controls.
    void updateLocation(TFT_eSPI &_tft) {
      // Read the x and y values of the joystick.
      int joyXVal = Joystick.readJoy(0);
      int joyYVal = Joystick.readJoy(1);

      // Check and see if the value is the same as the last value. If so, no change will happen.
      if (joyXVal == pastX) {
        joyXVal = 0;
      } else {
        pastX = joyXVal;
      }

      if (joyYVal == pastY) {
        joyYVal = 0;
      } else {
        pastY = joyYVal;
      }

      // Change the selected row/column based upon the joystick input.
      currentSelectedRow += joyXVal;
      currentSelectedColumn += joyYVal;

      // Make sure the row value doesn't exceed the maximum.
      if (currentSelectedRow < 0) {
        currentSelectedRow = 1;
      } else if (currentSelectedRow > 1) {
        currentSelectedRow = 0;
      }

      // Make sure on the pages without controls in them it doesn't go to another row.
      if (menuPage[INFO] || menuPage[HELP]) {
        currentSelectedRow = 0;
      }

      // If on the page menu.
      if (currentSelectedRow == 0) {
        if (currentSelectedColumn < 0) {
          currentSelectedColumn = 3;
        } else if (currentSelectedColumn > 3) {
          currentSelectedColumn = 0;
        }

        // If that button isn't hovered.
        if (buttonLayout[currentSelectedRow][currentSelectedColumn] == false) {
          // Set all the states to false.
          for (int i = 0; i < 4; i++) {
            buttonLayout[currentSelectedRow][i] = false;
          }

          // Set the hovered button to true.
          buttonLayout[currentSelectedRow][currentSelectedColumn] = true;

          // Output the hover.
          hoverControl(_tft, currentSelectedColumn);
          pageSelect(_tft, currSel);
        }

        if (menuPage[HOME]) {
          // Hover over the button on the home screen.
          resetUNHover(_tft);
        } else if (menuPage[BENCH]) {
          // Hover over the button on the benchmark screen.
          benchUNHover(_tft);
        }
      } else if (currentSelectedRow == 1) {
        if (menuPage[HOME]) {
          // Hover over the button on the home screen.
          for (int i = 0; i < 4; i++) {
            menuPage[i] = false;
          }
          menuPage[HOME] = true;
          resetHover(_tft);
        } else if (menuPage[BENCH]) {
          // Hover over the button on the benchmark screen.
          for (int i = 0; i < 4; i++) {
            menuPage[i] = false;
          }
          menuPage[BENCH] = true;
          benchHover(_tft);
        }
        for (int i = 0; i < 4; i++) {
          for (int n = 0; n < 3; n++) {
            if (i != currSel) {
              _tft.drawRoundRect((D_WIDTH - BUTTON_W - BUTTON_BORDER) + n, ((BUTTON_H * (i + 1)) + ((BUTTON_BORDER + 3) * (i + 1))) + n, BUTTON_W - n * 2, BUTTON_H - n * 2, EDGE_R, BUTTON_COLOR);
            }
          }
        }
      }
    }

    // Function to select what page you're viewing.
    void selectedButton(TFT_eSPI &_tft) {
      bool joyZVal = Joystick.readZ();

      if (joyZVal) {
        if (currentSelectedRow == 0) {
          for (int n = 0; n < 4; n++) {
            if (buttonLayout[0][n]) {
              for (int i = 0; i < 4; i++) {
                menuPage[i] = false;
              }
              menuPage[n] = true;
              currSel = n;
              for (int i = 0; i < 4; i++) {
                for (int n = 0; n < 4; n++) {
                  _tft.drawRoundRect((D_WIDTH - BUTTON_W - BUTTON_BORDER) + n, ((BUTTON_H * (i + 1)) + ((BUTTON_BORDER + 3) * (i + 1))) + n, BUTTON_W - n * 2, BUTTON_H - n * 2, EDGE_R, BUTTON_COLOR);
                }
              }
              pageSelect(_tft, n);
            }
          }
        } else if (currentSelectedRow == 1) {
          if (menuPage[HOME]) {
            resetBar(_tft);
          }
          if (menuPage[BENCH]) {
            benchLoop(_tft);
          }
        }
      }
    }

    void changePage(TFT_eSPI &_tft) {
      if (currSel != lastCurrSel) {
        // If the home page has been selected, output the homepage.
        if (menuPage[HOME]) {
          createHome(_tft);
        }

        // If the info page has been selected, output the info page.
        if (menuPage[INFO]) {
          createInfo(_tft);
        }

        // If the help page has been selected, output the help page.
        if (menuPage[HELP]) {
          createHelp(_tft);
        }

        // If the bench page has been selected, output the benchmark page.
        if (menuPage[BENCH]) {
          createBench(_tft);
        }

        lastCurrSel = currSel;
      }
    }

    /*
      INFO PAGE FUNCTIONS
    */

    void updateInfo(TFT_eSPI &_tft) {
      if (menuPage[INFO]) {
        if (digitalRead(FRONT0) == 0) {
          _tft.fillRect(X_MOD + ADJ_BAR_W / 2 - IND_SIDE_LEN / 3, Y_MOD + ADJ_BAR_H / 2 - IND_SIDE_LEN / 2, IND_SIDE_LEN, IND_SIDE_LEN, ON_COLOR);
        } else {
          _tft.fillRect(X_MOD + ADJ_BAR_W / 2 - IND_SIDE_LEN / 3, Y_MOD + ADJ_BAR_H / 2 - IND_SIDE_LEN / 2, IND_SIDE_LEN, IND_SIDE_LEN, OFF_COLOR);
        }
        if (digitalRead(BACK0) == 0) {
          _tft.fillRect(X_MOD + ADJ_BAR_W + MID_BAR_W + ADJ_BAR_W / 2 - IND_SIDE_LEN / 3, Y_MOD + ADJ_BAR_H / 2 - IND_SIDE_LEN / 2, IND_SIDE_LEN, IND_SIDE_LEN, ON_COLOR);
        } else {
          _tft.fillRect(X_MOD + ADJ_BAR_W + MID_BAR_W + ADJ_BAR_W / 2 - IND_SIDE_LEN / 3, Y_MOD + ADJ_BAR_H / 2 - IND_SIDE_LEN / 2, IND_SIDE_LEN, IND_SIDE_LEN, OFF_COLOR);
        }
      }
    }

    /*
      BAR FUNCTIONS
    */

    // Reset the values in the bar.
    void resetBar(TFT_eSPI &_tft) {
      reps = 0;
      startTime = millis();
      currentTime = startTime;
      counter(_tft, 3 + _tft.textWidth(repString) + 5, D_HEIGHT - 24 + 3, reps, 999, BAR_TEXT_COLOR, BAR_COLOR);
      minClock(_tft, 3 + _tft.textWidth(repString) + 2 + _tft.textWidth("999") + 30 + _tft.textWidth(timeString) + 15, (D_HEIGHT - 24 + 3), currentTime - startTime, BAR_TEXT_COLOR, BAR_COLOR);
    }

    // Function to update the content displayed on the bottom bar.
    void updateBar(TFT_eSPI &_tft) {
      if ((but1.readButton(BACK0)) || (but1.readButton(FRONT0))) {
        reps++;
        counter(_tft, 3 + _tft.textWidth(repString) + 3, D_HEIGHT - 24 + 3, reps, 999, BAR_TEXT_COLOR, BAR_COLOR);
      }

      currentTime = millis();

      minClock(_tft, 3 + _tft.textWidth(repString) + 2 + _tft.textWidth("999") + 30 + _tft.textWidth(timeString) + 10, (D_HEIGHT - 24 + 3), currentTime - startTime, BAR_TEXT_COLOR, BAR_COLOR);
    }

    /*
      BUTTON DISPLAY FUNCTIONS
    */

    // Display which menu option is being hovered upon.
    void hoverControl(TFT_eSPI &_tft, int page) {
      for (int i = 0; i < 4; i++) {
        for (int n = 0; n < 3; n++) {
          _tft.drawRoundRect((D_WIDTH - BUTTON_W - BUTTON_BORDER) + n, ((BUTTON_H * (i + 1)) + ((BUTTON_BORDER + 3) * (i + 1))) + n, BUTTON_W - n * 2, BUTTON_H - n * 2, EDGE_R, BUTTON_COLOR);
        }
      }
      for (int n = 0; n < 3; n++) {
        _tft.drawRoundRect((D_WIDTH - BUTTON_W - BUTTON_BORDER) + n, ((BUTTON_H * (page + 1)) + ((BUTTON_BORDER + 3) * (page + 1))) + n, BUTTON_W - n * 2, BUTTON_H - n * 2, EDGE_R, HOVER_COLOR);
      }
    }

    // Display which menu option is selected.
    void pageSelect(TFT_eSPI &_tft, int page) {
      for (int n = 0; n < 4; n++) {
        _tft.drawRoundRect((D_WIDTH - BUTTON_W - BUTTON_BORDER) + n, ((BUTTON_H * (page + 1)) + ((BUTTON_BORDER + 3) * (page + 1))) + n, BUTTON_W - n * 2, BUTTON_H - n * 2, EDGE_R, SELECTED_COLOR);
      }
    }

    // Hover over the reset button.
    void resetHover(TFT_eSPI &_tft) {
      for (int i = 0; i < 3; i++) {
        _tft.drawRoundRect(PAGE_W / 2 - BUTTON_W / 2 + i, PAGE_H / 2 - BUTTON_H / 2 + 25 + i, BUTTON_W - i * 2, BUTTON_H - i * 2, EDGE_R, HOVER_COLOR);
      }
    }

    // Don't hover over the reset button.
    void resetUNHover(TFT_eSPI &_tft) {
      for (int i = 0; i < 3; i++) {
        _tft.drawRoundRect(PAGE_W / 2 - BUTTON_W / 2 + i, PAGE_H / 2 - BUTTON_H / 2 + 25 + i, BUTTON_W - i * 2, BUTTON_H - i * 2, EDGE_R, BUTTON_COLOR);
      }
    }

    // Hover over the start button.
    void benchHover(TFT_eSPI &_tft) {
      for (int i = 0; i < 3; i++) {
        _tft.drawRoundRect(PAGE_W / 2 - BUTTON_W / 2 + i, PAGE_H / 2 + i, BUTTON_W - i * 2, BUTTON_H - i * 2, EDGE_R, HOVER_COLOR);
      }
    }

    // Don't hover over the start button.
    void benchUNHover(TFT_eSPI &_tft) {
      for (int i = 0; i < 3; i++) {
        _tft.drawRoundRect(PAGE_W / 2 - BUTTON_W / 2 + i, PAGE_H / 2 + i, BUTTON_W - i * 2, BUTTON_H - i * 2, EDGE_R, BUTTON_COLOR);
      }
    }

    /*
      BENCHMARK INTERFACE FUNCTIONS
    */

    void hoverConf(TFT_eSPI &_tft) {
      if (confHov[0]) {
        for (int i = 0; i < 3; i++) {
          _tft.drawRoundRect(PAGE_W / 2 - CONF_BUT_W - 5 + i, PAGE_H / 2 + i, CONF_BUT_W - i * 2, CONF_BUT_H - i * 2, 3, HOVER_COLOR);
        }
        for (int i = 0; i < 3; i++) {
          _tft.drawRoundRect(PAGE_W / 2 + 5 + i, PAGE_H / 2 + i, CONF_BUT_W - i * 2, CONF_BUT_H - i * 2, 3, BUTTON_COLOR);
        }
      } else if (confHov[1]) {
        for (int i = 0; i < 3; i++) {
          _tft.drawRoundRect(PAGE_W / 2 + 5 + i, PAGE_H / 2 + i, CONF_BUT_W - i * 2, CONF_BUT_H - i * 2, 3, HOVER_COLOR);
        }
        for (int i = 0; i < 3; i++) {
          _tft.drawRoundRect(PAGE_W / 2 - CONF_BUT_W - 5 + i, PAGE_H / 2 + i, CONF_BUT_W - i * 2, CONF_BUT_H - i * 2, 3, BUTTON_COLOR);
        }
      }
    }

    int pastConf = 0;
    int confHoverLocal = 0;

    void confControl(TFT_eSPI &_tft) {
      // Read the x and y values of the joystick.
      int joyXVal = Joystick.readJoy(0);

      // Check and see if the value is the same as the last value. If so, no change will happen.
      if (joyXVal == pastConf) {
        joyXVal = 0;
      } else {
        pastConf = joyXVal;
      }

      confHoverLocal += joyXVal;

      // Make sure the selected button is within range.
      if (confHoverLocal < 0) {
        confHoverLocal = 1;
      } else if (confHoverLocal > 1) {
        confHoverLocal = 0;
      }

      for (int i = 0; i < 2; i++) {
        confHov[i] = false;
      }

      confHov[confHoverLocal] = true;
      hoverConf(_tft);
    }

    void benchLoop(TFT_eSPI &_tft) {
      createConf(_tft);
      bool conf = true;

      while (conf) {
        confControl(_tft);

        bool joyZVal = Joystick.readZ();

        if (joyZVal) {
          break;
        }
      }

      if (confHov[1]) {
        _tft.setFreeFont(sansBold);

        _tft.fillRect(0, 0, PAGE_W, PAGE_H, TFT_SILVER);
        for (int i = 0; i < 4; i++) {
          _tft.fillRect(PAGE_W / 2 - 10, PAGE_H / 2 - 10, 20, 20, TFT_SILVER);
          _tft.drawString(benchCountdown[i], PAGE_W / 2 - _tft.textWidth(benchCountdown[i]) / 2, PAGE_H / 2 - _tft.fontHeight() / 2);
          delay(700);
        }
        _tft.fillRect(0, 0, PAGE_W, PAGE_H, TFT_SILVER);

        bool benchmark = true;
        unsigned long benchStart = millis();
        unsigned long benchEnd = benchStart + 1000 * 30;
        int repsDone = 0;

        const int TIME_Y = 5 + tft.fontHeight() * 3;

        _tft.drawString("Benchmark", PAGE_W / 2 - _tft.textWidth("Benchmark") / 2, 5);
        _tft.drawString("Reps: ", PAGE_W / 2 - _tft.textWidth("Reps: ") / 2, 5 + _tft.fontHeight());
        _tft.drawString("Time Left", PAGE_W / 2 - _tft.textWidth("Time Left") / 2, 5 + _tft.fontHeight() * 2);

        bar timeBar;
        TFT_eSprite Bar = TFT_eSprite(&_tft);
        createBorder(_tft, TIME_X - 3, TIME_Y - 3, TIME_W + 3 * 2, TIME_H + 3 * 2, 3, TFT_DARKGREY);
        timeBar.barSetup(_tft, Bar, TIME_X, TIME_Y, TIME_W, TIME_H, 0, TFT_BLUE, TFT_SILVER);

        counter(_tft, PAGE_W / 2 + _tft.textWidth("Reps: ") / 2, 5 + _tft.fontHeight(), repsDone, 999, TFT_BLACK, TFT_SILVER);

        while (benchmark) {
          timeBar.barState(Bar, (benchEnd - millis()) / 1000 * 3);

          if ((but1.readButton(BACK0)) || (but1.readButton(FRONT0))) {
            repsDone++;
            counter(_tft, PAGE_W / 2 + _tft.textWidth("Reps: ") / 2, 5 + _tft.fontHeight(), repsDone, 999, TFT_BLACK, TFT_SILVER);
            delay(10);
          }

          if (millis() >= benchEnd) {
            benchmark = false;
          }
        }

        _tft.fillRect(0, 0, PAGE_W, PAGE_H, TFT_GREEN);
        _tft.setFreeFont(sansBold);
        _tft.drawString("Your Score: ", PAGE_W / 2 - _tft.textWidth("Your Score: ") / 3 * 2, PAGE_H / 2 - _tft.fontHeight() / 2);
        _tft.drawString(String(repsDone), PAGE_W / 2 + _tft.textWidth("Your Score: ") / 3 + 4, PAGE_H / 2 - _tft.fontHeight() / 2);
        delay(5000);
      } else {
        _tft.fillRect(0, 0, PAGE_W, PAGE_H, TFT_NAVY);
        _tft.drawString(NO_TOP, PAGE_W / 2 - _tft.textWidth(NO_TOP) / 2, PAGE_H / 2 - _tft.fontHeight());
        _tft.drawString(NO_BOT, PAGE_W / 2 - _tft.textWidth(NO_BOT) / 2, PAGE_H / 2);
        delay(5000);
      }

      createBench(_tft);
    }

    const String benchCountdown[4] = {"3", "2", "1", "Go!"};
    const int TIME_W = 90;
    const int TIME_H = 20;
    const int TIME_X = PAGE_W / 2 - TIME_W / 2;

    /*
      CREATION FUNCTIONS
    */

    // Side menu
    void createMenu(TFT_eSPI &_tft) {
      // Display the blue background.
      _tft.fillRect((D_WIDTH - BUTTON_W - (BUTTON_BORDER * 2)), 0, (BUTTON_W + (BUTTON_BORDER * 2)), D_HEIGHT, SIDE_MENU_COLOR);
      // Change the text color.
      _tft.setTextColor(BUTTON_TEXT_COLOR);
      // Set the font.
      _tft.setFreeFont(sansBold);
      // Display the title.
      _tft.drawString(menuTitle, ((D_WIDTH - BUTTON_W - (BUTTON_BORDER * 2)) + ((BUTTON_W + (BUTTON_BORDER * 2)) - _tft.textWidth(menuTitle)) / 2), 3);
      // Display the version number.
      _tft.drawString(VERSION_NUMBER, ((D_WIDTH - BUTTON_W - (BUTTON_BORDER * 2)) + ((BUTTON_W + (BUTTON_BORDER * 2)) - _tft.textWidth(VERSION_NUMBER)) / 2), (D_HEIGHT - _tft.fontHeight()));
      // Display the buttons.
      for (int i = 0; i < 4; i++) {
        _tft.fillRoundRect((D_WIDTH - BUTTON_W - BUTTON_BORDER), ((BUTTON_H * (i + 1)) + ((BUTTON_BORDER + 3) * (i + 1))), BUTTON_W, BUTTON_H, EDGE_R, BUTTON_COLOR);
        _tft.drawString(*(buttonLabels + i), (D_WIDTH - BUTTON_W - BUTTON_BORDER + ((BUTTON_W - _tft.textWidth(*(buttonLabels + i))) / 2)), (((BUTTON_H * (i + 1)) + ((BUTTON_BORDER + 3) * (i + 1))) + ((BUTTON_H - _tft.fontHeight()) / 2) + 4));
      }
    }

    // Bottom bar.
    void createBar(TFT_eSPI &_tft) {
      _tft.fillRect(0, D_HEIGHT - 24, (D_WIDTH - BUTTON_W - (BUTTON_BORDER * 2)), 24, BAR_COLOR);
      _tft.setTextColor(BAR_TEXT_COLOR);
      _tft.setFreeFont(sansBold);
      _tft.drawString(repString, 3, (D_HEIGHT - 24 + 3));
      _tft.drawString(timeString, 3 + _tft.textWidth(repString) + 2 + _tft.textWidth("999") + 30, (D_HEIGHT - 24 + 3));
      counter(_tft, 3 + _tft.textWidth(repString) + 5, D_HEIGHT - 24 + 3, reps, 999, BAR_TEXT_COLOR, BAR_COLOR);
      minClock(_tft, 3 + _tft.textWidth(repString) + 2 + _tft.textWidth("999") + 30 + _tft.textWidth(timeString) + 15, (D_HEIGHT - 24 + 3), currentTime - startTime, BAR_TEXT_COLOR, BAR_COLOR);
    }

    void createHome(TFT_eSPI &_tft) {
      _tft.fillRect(0, 0, PAGE_W, PAGE_H, TFT_SILVER);
      _tft.setTextColor(TFT_BLACK);
      _tft.setFreeFont(titleFont);
      _tft.drawString(TOP_ROW, PAGE_W / 2 - _tft.textWidth(TOP_ROW) / 2, 5);
      _tft.drawString(BOTTOM_ROW, PAGE_W / 2 - _tft.textWidth(BOTTOM_ROW) / 2, 5 + _tft.fontHeight() + 2);
      _tft.fillRoundRect(PAGE_W / 2 - BUTTON_W / 2, PAGE_H / 2 - BUTTON_H / 2 + 25, BUTTON_W, BUTTON_H, EDGE_R, BUTTON_COLOR);
      _tft.setFreeFont(sansBold);
      _tft.drawString(RESET_TEXT, PAGE_W / 2 - _tft.textWidth(RESET_TEXT) / 2, PAGE_H / 2 - _tft.fontHeight() / 2 + 25 + 3);
    }

    void createInfo(TFT_eSPI &_tft) {
      _tft.fillRect(0, 0, PAGE_W, PAGE_H, TFT_SILVER);
      createModel(_tft);
    }

    void createHelp(TFT_eSPI &_tft) {
      _tft.fillRect(0, 0, PAGE_W, PAGE_H, TFT_SILVER);
      _tft.setFreeFont(sans);
      _tft.setTextColor(TFT_BLACK);

      for (int i = 0; i < 9; i++) {
        _tft.drawString(HELP_TEXT[i], 5, 5+_tft.fontHeight()*i);
      }

      setPngPosition(PAGE_W-QR_CODE_SIDE_L, PAGE_H-QR_CODE_SIDE_L);
      load_file(manual, sizeof(manual));
    }

    void createBench(TFT_eSPI &_tft) {
      _tft.fillRect(0, 0, PAGE_W, PAGE_H, TFT_SILVER);
      _tft.setFreeFont(sansBold);
      _tft.setTextColor(TFT_BLACK);
      _tft.drawString("Benchmark", PAGE_W / 2 - _tft.textWidth("Benchmark") / 2, PAGE_H / 2 - _tft.fontHeight());
      _tft.fillRoundRect(PAGE_W / 2 - BUTTON_W / 2, PAGE_H / 2, BUTTON_W, BUTTON_H, EDGE_R, BUTTON_COLOR);
      _tft.drawString("Start", PAGE_W / 2 - _tft.textWidth("Start") / 2, PAGE_H / 2 + 4);
    }

    void createModel(TFT_eSPI &_tft) {
      // Create the top bar.
      _tft.fillRect(X_MOD, Y_MOD, ADJ_BAR_W, ADJ_BAR_H, END_BAR_COLOR);
      _tft.fillRect(X_MOD + ADJ_BAR_W + MID_BAR_W, Y_MOD, ADJ_BAR_W, ADJ_BAR_H, END_BAR_COLOR);
      _tft.fillRect(X_MOD + ADJ_BAR_W, Y_MOD + MID_BAR_Y_OFFSET, MID_BAR_W, ADJ_BAR_H - (MID_BAR_Y_OFFSET * 2), ADJ_BAR_COLOR);

      // Create the bottom bar.
      _tft.fillRect(X_MOD, Y_MOD + H_MOD - ADJ_BAR_H, ADJ_BAR_W, ADJ_BAR_H, END_BAR_COLOR);
      _tft.fillRect(X_MOD + ADJ_BAR_W + MID_BAR_W, Y_MOD + H_MOD - ADJ_BAR_H, ADJ_BAR_W, ADJ_BAR_H, END_BAR_COLOR);
      _tft.fillRect(X_MOD + ADJ_BAR_W, Y_MOD + H_MOD - ADJ_BAR_H + MID_BAR_Y_OFFSET, MID_BAR_W, ADJ_BAR_H - (MID_BAR_Y_OFFSET * 2), ADJ_BAR_COLOR);

      // Create the foot on the left.
      _tft.fillRect(X_MOD + SIDE_OFFSET, Y_MOD + ADJ_BAR_H, ENCLOSURE_WIDTH, H_MOD - (ADJ_BAR_H * 2), FOOT_ENCLOSURE_COLOR);
      _tft.fillRect(X_MOD + SIDE_OFFSET + ENCLOSURE_WIDTH + MID_W, Y_MOD + ADJ_BAR_H, ENCLOSURE_WIDTH, H_MOD - (ADJ_BAR_H * 2), FOOT_ENCLOSURE_COLOR);
      _tft.fillRect(X_MOD + SIDE_OFFSET + ENCLOSURE_WIDTH, Y_MOD + ADJ_BAR_H, MID_W, ENCLOSURE_WIDTH, FOOT_ENCLOSURE_COLOR);
      _tft.fillRect(X_MOD + SIDE_OFFSET + ENCLOSURE_WIDTH, Y_MOD + H_MOD - ADJ_BAR_H - ENCLOSURE_WIDTH, MID_W, ENCLOSURE_WIDTH, FOOT_ENCLOSURE_COLOR);

      // Create the foot on the right.
      _tft.fillRect(X_MOD + W_MOD - MID_W - (ENCLOSURE_WIDTH * 2) - SIDE_OFFSET, Y_MOD + ADJ_BAR_H, ENCLOSURE_WIDTH, H_MOD - (ADJ_BAR_H * 2), FOOT_ENCLOSURE_COLOR);
      _tft.fillRect(X_MOD + W_MOD - ENCLOSURE_WIDTH - SIDE_OFFSET, Y_MOD + ADJ_BAR_H, ENCLOSURE_WIDTH, H_MOD - (ADJ_BAR_H * 2), FOOT_ENCLOSURE_COLOR);
      _tft.fillRect(X_MOD + W_MOD - MID_W - ENCLOSURE_WIDTH - SIDE_OFFSET, Y_MOD + ADJ_BAR_H, MID_W, ENCLOSURE_WIDTH, FOOT_ENCLOSURE_COLOR);
      _tft.fillRect(X_MOD + W_MOD - MID_W - ENCLOSURE_WIDTH - SIDE_OFFSET, Y_MOD + H_MOD - ADJ_BAR_H - ENCLOSURE_WIDTH, MID_W, ENCLOSURE_WIDTH, FOOT_ENCLOSURE_COLOR);

      // Create the legend.
      _tft.setTextColor(TFT_GREEN);
      _tft.setFreeFont(sansBold);
      _tft.drawString("ON", 5, 5);
      _tft.setTextColor(TFT_RED);
      _tft.drawString("OFF", 5, 5 + _tft.fontHeight());
    }

    // Function to create the confirmation page.
    void createConf(TFT_eSPI &_tft) {
      _tft.fillRect(0, 0, PAGE_W, PAGE_H, TFT_SILVER);
      _tft.setFreeFont(sansBold);
      _tft.setTextColor(TFT_BLACK);
      _tft.drawString(TOP_CONF, PAGE_W / 2 - _tft.textWidth(TOP_CONF) / 2, PAGE_H / 2 - _tft.fontHeight() * 2);
      _tft.drawString(BOTTOM_CONF, PAGE_W / 2 - _tft.textWidth(BOTTOM_CONF) / 2, PAGE_H / 2 - _tft.fontHeight());

      _tft.fillRoundRect(PAGE_W / 2 - CONF_BUT_W - 5, PAGE_H / 2, CONF_BUT_W, CONF_BUT_H, 3, BUTTON_COLOR);
      _tft.fillRoundRect(PAGE_W / 2 + 5, PAGE_H / 2, CONF_BUT_W, CONF_BUT_H, 3, BUTTON_COLOR);

      _tft.drawString(N_CONF, PAGE_W / 2 - CONF_BUT_W / 2 - 5 - _tft.textWidth(N_CONF) / 2, PAGE_H / 2 + CONF_BUT_H / 2 - _tft.fontHeight() / 2 + 2);
      _tft.drawString(Y_CONF, PAGE_W / 2 + CONF_BUT_W / 2 + 5 - _tft.textWidth(Y_CONF) / 2, PAGE_H / 2 + CONF_BUT_H / 2 - _tft.fontHeight() / 2 + 2);
    }
};
