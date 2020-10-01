// Import libraries
#include <Wire.h>
#include "rgb_lcd.h"
#include <math.h>

rgb_lcd lcd;

class ScreenState
// This class holds the value of the temperature and can change the screens color and text
{
  private:
    int colorR;
    int colorG;
    int colorB;
    int minTemp = -40;
    int maxTemp = 125;
  public:
    String firstRow;
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
  // Changes the displayed color and text
  {
    lcd.setRGB(colorR, colorG, colorB);
    lcd.setCursor(0, 0);
    lcd.print(firstRow);
  }

  void changeValue(int newValue)
  // Maps the max- and min-values of the rotater to the max- and min-values of the temp-reader.
  {
    value = map(newValue, 0, 1023, maxTemp, minTemp);
  }

  void calcTemperature(int newValue)
  // Calculates the temperature
  {
    const int B = 4275;
    const int R0 = 100000;
    float R = 1023.0/newValue-1.0;
    R = R0*R;
    value = 1.0/(log(R/R0)/B+1/298.15)-273.15;
  }

  void myDisplay()
  // Prints the temperature
  {
    lcd.setCursor(0, 1);
    lcd.print("" + String(value) + " C" + (char)223);
  }
};

// Initiates states and variables
int currentState = 0;
int maxStates = 3;
bool buttonIsPressed = false;
ScreenState screens[3] = {
    ScreenState("Temperaturen er:", 0, 255, 0, 0),
    ScreenState("Indstil min:    ", 0, 0, 255, 1),
    ScreenState("Indstil max:    ", 255, 0, 0, 1),
 };
int buttonPin = 4;
int rotatingStuffPin = A0;
int tempPin = A1;
int buzzPin = 6;
int myMin = -40;
int myMax = 125;
int myMinTempo = myMin;
int myMaxTempo = myMax;
void setup() 
{
  pinMode(buttonPin, INPUT);
  pinMode(rotatingStuffPin, INPUT);
  pinMode(tempPin, INPUT);
  pinMode(buzzPin, OUTPUT);
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
    int tempRead = analogRead(tempPin);
    if (buttonRead == 1 and !buttonIsPressed)
    // If button is pressed, change the state
    {
      currentState = currentState + 1;
      if (currentState >= maxStates)
      {
        currentState = 0;
      }
      buttonIsPressed = true;

      // change screen state
      screens[currentState].changedTo();
    }
    else if (buttonRead == 0)
    {
      buttonIsPressed = false;
    }
    if (currentState == 1)
    // If it is 1 set the min-temperature
    {
      screens[currentState].changeValue(rotatingStuffRead);
      myMinTempo = screens[currentState].value;
    }
    if (currentState == 2)
    // If it is 2 set the max-temperature
    {
      screens[currentState].changeValue(rotatingStuffRead);
      myMaxTempo = screens[currentState].value;
    }
    if (currentState == 0)
    // If it is 0 beep and change text if the temperature is too high or low
    {
      myMin = myMinTempo;
      myMax = myMaxTempo;
      screens[currentState].calcTemperature(tempRead);
      if (myMax < screens[currentState].value)
      {
        screens[currentState].firstRow = "Temp for hoej:  ";
        digitalWrite(6, HIGH);
      }
      else if (myMin > screens[currentState].value)
      {
        screens[currentState].firstRow = "Temp for lav:   ";
        digitalWrite(6, HIGH);
        delay(1000);
        digitalWrite(6, LOW);
        delay(1000);
      }
      else
      {
        screens[currentState].firstRow = "Temperaturen er:";
        digitalWrite(6, LOW);
      }
      screens[currentState].changedTo();
    }
    screens[currentState].myDisplay();

}
