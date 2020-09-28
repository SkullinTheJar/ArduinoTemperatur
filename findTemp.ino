#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

class ScreenState
{
  private:
    String firstRow;
    int colorR;
    int colorG;
    int colorB;
    int myMin = -40;
    int myMax = 125;
  public:
    int value = 0;
    int stateType;
    
  ScreenState(String _firstRow, int _colorR, int _colorG, int _colorB, int _stateType) 
  {
    firstRow = _firstRow;
    colorR = _colorR;
    colorG = _colorG;
    colorB = _colorB;
    stateType = _stateType;
  }

  void changedTo()
  {
    lcd.setRGB(colorR, colorG, colorB);
    lcd.setCursor(0, 0);
    lcd.print(firstRow);
  }

  void changeValue(int newValue)
  {
    value = map(newValue, 0, 1023, myMax, myMin);
  }

  void untouchedChangeValue(int newValue)
  {
    //value = newValue;
    value = map(newValue, 0, 1023, myMin, myMax);
  }

  void myDisplay()
  {
    lcd.setCursor(0, 1);
    lcd.print("" + String(value) + "     ");
  }
};

int currentState = 0;
int maxStates = 3;
bool myIsPressed = false;
ScreenState screens[5] = {
    ScreenState("Temperaturen er:", 0, 255, 0, 0),
    ScreenState("Indstil min:    ", 0, 0, 255, 1),
    ScreenState("Indstil max:    ", 255, 0, 0, 1),
    ScreenState("Temp for lav:   ", 0, 0, 255, 2),
    ScreenState("Temp for hoej:  ", 255, 0, 0, 2),
 };
 int buttonPin = 4;
 int rotatingStuffPin = A0;
 int tempPin = A1;
// NEW - START
 const int B = 4275;
 const int R0 = 100000;
// NEW - END
void setup() 
{
  
  pinMode(buttonPin, INPUT);
  pinMode(rotatingStuffPin, INPUT);
  pinMode(tempPin, INPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(0, 0, 0);
  delay(10);
  screens[currentState].changedTo();
}

void loop() 
{
    int buttonRead = digitalRead(buttonPin);
    int rotatingStuffRead = analogRead(rotatingStuffPin);
    
    if (buttonRead == 1 and !myIsPressed)
    {
      currentState = currentState + 1;
      if (currentState >= maxStates)
      {
        currentState = 0;
      }
      myIsPressed = true;

      // change screen state
      screens[currentState].changedTo();
    }
    else if (buttonRead == 0)
    {
      myIsPressed = false;
    }
    screens[currentState].changeValue(rotatingStuffRead);
    if (screens[currentState].stateType == 1)
    {
      rotatingStuffRead = analogRead(rotatingStuffPin);
    }
    else if (screens[currentState].stateType == 0 or screens[currentState].stateType == 2)
    {
      rotatingStuffRead = analogRead(tempPin);
      screens[currentState].untouchedChangeValue(rotatingStuffRead);
    }
    
    screens[currentState].myDisplay();

}
