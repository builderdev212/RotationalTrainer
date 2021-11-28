#include <Arduino.h>

class Button
{
  public:
    void begin(uint8_t button1, uint8_t button2) {
      btn1 = button1;
      btn2 = button2;
      state = 0;
      lastState = state;
      pinMode(btn1, INPUT_PULLUP);
      pinMode(btn2, INPUT_PULLUP);
    }
    bool read() {
      state = (digitalRead(btn1) && digitalRead(btn2));
      delay(5);
      if (state == lastState) {
        return false;
      } else {
        lastState = state;
        return state;
      }
    }
  private:
    uint8_t btn1;
    uint8_t btn2;
    bool lastState;
    bool state;
};

const int X_AXIS = 0;
const int Y_AXIS = 1;

class joystick {
  public:

    int xPin;
    int yPin;
    int zPin;
    void begin(int x, int y, int z) {
      xPin = x;
      yPin = y;
      zPin = z;
      pinMode(zPin, INPUT_PULLUP);
    };

    int readJoy(int axis) {
      switch (axis) {
        // x axis
        case (X_AXIS):
          {
            int16_t xVal = analogRead(xPin);
            delay(10);
            if (xVal < 1800) {
              return 1;
            } else if (xVal > 2200) {
              return -1;
            } else {
              return 0;
            }
            break;
          }
        // y axis
        case (Y_AXIS):
          {
            int16_t yVal = analogRead(yPin);
            delay(10);
            if (yVal < 1700) {
              return -1;
            } else if (yVal > 2300) {
              return 1;
            } else {
              return 0;
            }
            break;
          }
        default:
          return 0;
          break;
      }
    }

    bool readZ() {
      int buttonState = digitalRead(zPin);

      if (buttonState != lastButtonState) {
        lastButtonState = buttonState;
        if (buttonState == 0) {
          return true;
        }
      }

      return false;
    }

  private:
    int lastButtonState = 1;
};
