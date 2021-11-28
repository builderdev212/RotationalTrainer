#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

#define sans &FreeSans9pt7b
#define sansBold &FreeSansBold9pt7b
#define titleFont &FreeSansBold24pt7b

class bar {
  public:
    void barSetup(TFT_eSPI &_tft, TFT_eSprite &barSprite, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t thk, uint32_t color, uint32_t bg) {
      barX = x;
      barY = y;
      barW = w;
      barH = h;
      borderThk = thk;
      barColor = color;
      bgColor = bg;
      switch (borderThk) {
        case 0:
          isBorder = 0;
          break;
        default:
          isBorder = 1;
          break;
      }
      // Create sprite for speed bar
      barSprite.createSprite(barW - (borderThk * 3), barH - (borderThk * 3));
      // Make sure that the background of the sprite is the same color as the info bar.
      barSprite.fillScreen(barColor);
      // Push the sprite to the screen.
      barSprite.pushSprite(barX + borderThk + (1 * isBorder), barY + borderThk + (1 * isBorder));
    };
    void barState(TFT_eSprite &barSprite, uint16_t barFill) {
      // Fill bar that is set by barFill in pixels.
      barSprite.fillRect(0, 0, barFill, barH - (borderThk * 3), barColor);
      // Fill rest of bar with background color.
      barSprite.fillRect(barFill, 0, barW, barH - (borderThk * 3), bgColor);
      // Output changes to screen.
      barSprite.pushSprite(barX + borderThk + (1 * isBorder), barY + borderThk + (1 * isBorder));
    };
  private:
    uint16_t barX;
    uint16_t barY;
    uint16_t barW;
    uint16_t barH;
    uint16_t borderThk;
    uint32_t barColor;
    uint32_t bgColor;
    uint32_t borderColor;
    uint8_t isBorder;
};

void createBorder(TFT_eSPI &_tft, uint16_t borderX, uint16_t borderY, uint16_t borderW, uint16_t borderH, uint16_t borderThk, uint32_t borderColor) {
  // Create 2 pixel thick border
  for (int i = 1; i <= borderThk; i++) {
    _tft.drawRect(borderX + (i - 1), borderY + (i - 1), borderW - (i - 1) * 2, borderH - (i - 1) * 2, borderColor);
  }
};

void counter(TFT_eSPI &_tft, uint16_t x, uint16_t y, int count, int max_count, uint32_t textColor, uint32_t bgColor) {
  // Clear the place where the coin count is shown.
  _tft.fillRect(x - 1, y - 1, _tft.textWidth(String(max_count)) + 1, _tft.fontHeight() + 1, bgColor);
  // Set the text color and background color.
  _tft.setTextColor(textColor);
  _tft.setFreeFont(sans);
  if (count > max_count) {
    count = max_count;
  }
  // Draw the string with the given coin count to the screen at the given location.
  _tft.drawString(String(count), x, y);
};

void minClock(TFT_eSPI &_tft, uint16_t x, uint16_t y, unsigned long millisec, uint32_t textColor, uint32_t bgColor) {
  // Clear the background.
  _tft.fillRect(x, y, _tft.textWidth("00:000"), _tft.fontHeight(), bgColor);

  // Set the text color and font.
  _tft.setTextColor(textColor);
  _tft.setFreeFont(sans);

  // Calculate the minutes and seconds to draw.
  int mins = millisec/1000/60;
  int secs = (millisec/1000)%60;

  // Make sure they don't go over the max amount allowed.
  if (mins > 99) {
    mins = 99;
  }
  if (secs > 99) {
    secs = 99;
  }

  // Store the time to a string.
  String drawTime = "";

  if (mins < 10) {
    drawTime += "0" + String(mins);
  } else {
    drawTime += String(mins);
  }

  drawTime += ":";

  if (secs < 10) {
    drawTime += "0" + String(secs);
  } else {
    drawTime += String(secs);
  }

  // Draw the time to the screen.
  _tft.drawString(drawTime, x, y);
};
