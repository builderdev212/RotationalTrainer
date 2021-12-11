 // Speeds up the drawing of PNG's
#define USE_LINE_BUFFER

// Import the functions needed for the display.
#include <SPI.h>
#include <TFT_eSPI.h>

// Setup the display class.
TFT_eSPI tft = TFT_eSPI();

// Import the file that holds the GUI.
#include "UI.h"

void setup() {
  // Begin the serial interface.
  Serial.begin(115200);

  // Setup the display.
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);

  pinMode(33, INPUT);
  pinMode(25, INPUT);
}

UI trainer;

void loop() {
  trainer.mainloop(tft);
}
