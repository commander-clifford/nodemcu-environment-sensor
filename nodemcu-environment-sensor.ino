#include <Wire.h> // I2C / etc
#include <Adafruit_GFX.h> // Graphics Library
#include <Adafruit_SSD1306.h> // OLED Display Library
#include <Adafruit_AHTX0.h> // Temperature/Humidity Sensor
#include "Max44009.h" // Light Sensor - https://github.com/RobTillaart/Max44009https://github.com/RobTillaart/Max44009

/*** DEFINE & SET CORE VALUES ***/ 

/* 
  OLED Display
  */
#define SCREEN_ADDRESS 0x3C // OLED I2C Address
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

/*
  Temp/Humid Sensor
  */
#define SENSOR_ADDRESS 0x38; // Sensor I2C Address
Adafruit_AHTX0 aht; // define sensor
int TEMP_F = 0; // define Fahrenheit variable
int TEMP_C = 0; // define Celsius variable
int HUMIDITY = 0; // define Humidity variable

/* 
  Light Sensor 
  */
#define LIGHT_SENSOR_ADDRESS 0x4A // OLED I2C Address
Max44009 myLux(LIGHT_SENSOR_ADDRESS); // define light sensor
uint32_t lastDisplay = 0; // set display refresh limit
float luxNum = 0; // calculated lux number
int luxLinePixelsLength = 0; // lux display-bar length

/* Constants (const) do not change. Use to set pin values. */
/* Variables (var) can change. Use for values that will change */

/*
  Buttons
  */ 
const int BUTTONPIN1 = 12; // Button 1 board pin
const int BUTTONPIN2 = 13; // Button 2 board pin
const int BUTTONPIN3 = 15; // Button 3 board pin

// debounce variables
int buttonStateOne; // the current reading from the input pin
int lastButtonStateOne = HIGH; // the previous reading from the input pin
int buttonStateTwo; // the current reading from the input pin
int lastButtonStateTwo = HIGH; // the previous reading from the input pin
int buttonStateThree; // the current reading from the input pin
int lastButtonStateThree = HIGH; // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

/*
  Display Mode Variable
  Used to toggle through OLED display modes
  */ 
int displayMode = 0; // display mode variable

/*** INITIATE CORE FUNCTIONS ***/ 

/*
  Button Functions
  */
void pressButtonOne() {
  displayMode = displayMode+1;
  if(displayMode == 2){displayMode = 0;};
  updateDisplay(displayMode);
}
void pressButtonTwo() {
  displayMode = displayMode+1;
  if(displayMode == 2){displayMode = 0;};
  updateDisplay(displayMode);
}
void pressButtonThree() {
  displayMode = displayMode+1;
  if(displayMode == 2){displayMode = 0;};
  updateDisplay(displayMode);
}

/*
  Display Functions
  */

void toggleDisplayMode(int displayMode) {
  displayMode = displayMode+1;
  if(displayMode == 2){displayMode = 0;};
  updateDisplay(displayMode);
}

void updateDisplay(int displayMode) {

  /* Calculate Lux/Lumens */ 
  int luxLineMeterLimit = 2000; // set sensitivity (0-20000)
  int luxLineLimit = 104;
  luxLinePixelsLength = (luxNum / luxLineMeterLimit)*luxLineLimit;
  if(luxNum > luxLineMeterLimit) luxLinePixelsLength = luxLineLimit;

  /* Choose Display Mode */ 
  switch (displayMode) {
  case 1:
    display.clearDisplay(); // Clear
    display.setTextColor(SSD1306_WHITE); // Draw white text (white ~= LED-ON)
    display.cp437(true); // Use full 256 char 'Code Page 437' font

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Temp");

    display.fillCircle(67, 10, 5, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK); // Draw white text (white ~= LED-ON)
    display.setCursor(65, 7);
    display.print("C");

    display.setTextColor(SSD1306_WHITE); // Draw white text (white ~= LED-ON)
    display.setTextSize(5);
    display.setCursor(4, 12);
    display.print(TEMP_C);

    display.setTextSize(1);
    display.setCursor(82, 0);
    display.print("Farnht");
    
    display.setTextSize(2);
    display.setCursor(92, 11);

    display.print(TEMP_F);
    display.drawCircle(120, 11, 2, SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(82, 29);
    display.print("Humid");

    display.setTextSize(2);
    display.setCursor(92, 41);

    display.print(HUMIDITY);

    // percentage sign
    display.drawCircle(120, 43, 2, SSD1306_WHITE);
    display.drawLine(125, 43, 119, 53, SSD1306_WHITE);
    display.drawCircle(124, 52, 2, SSD1306_WHITE);

    // Lux Display
    display.setTextSize(1);
    display.setCursor(0, 56);
    display.print("Lux:");


    display.fillRect(24, 57, luxLinePixelsLength, 6, SSD1306_WHITE);

    break;

  case 2:
    display.clearDisplay(); // Clear

    break;
  default:

    display.clearDisplay(); // Clear
    display.setTextColor(SSD1306_WHITE); // Draw white text (white ~= LED-ON)
    display.cp437(true); // Use full 256 char 'Code Page 437' font

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Temp");

    display.fillCircle(67, 10, 5, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK); // Draw white text (white ~= LED-ON)
    display.setCursor(65, 7);
    display.print("F");

    display.setTextColor(SSD1306_WHITE); // Draw white text (white ~= LED-ON)
    display.setTextSize(5);
    display.setCursor(4, 12);
    display.print(TEMP_F);

    display.setTextSize(1);
    display.setCursor(82, 0);
    display.print("Celsius");
    
    display.setTextSize(2);
    display.setCursor(92, 11);

    display.print(TEMP_C);
    display.drawCircle(120, 11, 2, SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(82, 29);
    display.print("Humid");

    display.setTextSize(2);
    display.setCursor(92, 41);

    display.print(HUMIDITY);
    display.drawCircle(120, 43, 2, SSD1306_WHITE);
    display.drawLine(125, 43, 119, 53, SSD1306_WHITE);
    display.drawCircle(124, 52, 2, SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 56);
    display.print("Lux:");

    display.fillRect(24, 57, luxLinePixelsLength, 6, SSD1306_WHITE);

    break;

  }

  /* Draw Display */
  display.display();

}

/*** SETUP ***/
void setup(void) {

  // Initiate Serial Monitor 
  Serial.begin(115200);
  // Testing Only
  // Serial.print("\nInitiate max44009_interrupt : "); 
  // Serial.println(MAX44009_LIB_VERSION);

  // Initiate I2C
  Wire.begin();

  // Initiate Light Sensor
  myLux.setContinuousMode(); // Initiate continuous mode
  myLux.setHighThreshold(30); // Set High threshold
  myLux.setLowThreshold(10); // Set Low Threshold
  myLux.setThresholdTimer(2); // Set Threshold Timer
  myLux.enableInterrupt(); // Enable Interrupt
  // Testing Only
  // Serial.print("HighThreshold:\t"); 
  // Serial.println(myLux.getHighThreshold());
  // Serial.print("LowThreshold:\t");
  // Serial.println(myLux.getLowThreshold());

  // Buttons
  pinMode(BUTTONPIN1, INPUT_PULLUP);
  pinMode(BUTTONPIN2, INPUT_PULLUP);
  pinMode(BUTTONPIN3, INPUT_PULLUP);

  // Temperature Sensor
  if (! aht.begin()) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Could not find AHT? Check wiring");
    display.display();
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }

  /* Start OLED Display */
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    // TODO: make read-able updateDisplay(error, screenInit)
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay(); // Clear
  display.setTextColor(SSD1306_WHITE); // Draw white text (white ~= LED-ON)
  display.cp437(true); // Use full 256 char 'Code Page 437' font
  display.dim(false);

}

/*** LOOP ***/
void loop() {

  // Serial.println("--Begin--Loop--");

  uint32_t interval = 1000;
  if (millis() - lastDisplay >= interval)
  {
    lastDisplay += interval;
    float lux = myLux.getLux();
    luxNum = lux;
    int err = myLux.getError();
    int st = myLux.getInterruptStatus();
    if (err != 0)
    {
      // Serial.print("Error:\t");
      // Serial.println(err);
    }
    else
    {
      // Serial.print("lux:\t");
      // Serial.print(lux);
      // if (st == 1) Serial.println("\tIRQ occu rred");
      // else Serial.println();
    }
  }

  // Measure Temp/Humid
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  TEMP_C = temp.temperature;
  TEMP_F = (temp.temperature * 1.8) + 32; // convert to Fahrenheit 
  HUMIDITY = humidity.relative_humidity;
  // Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  // Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");


  /*
    Buttons
    TODO: Use a for-loop for this
    */

  // read the state of the switch into a local variable:
  int reading1 = digitalRead(BUTTONPIN1);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:
  // If the switch changed, due to noise or pressing:
  if (reading1 != lastButtonStateOne) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading1 is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading1 != buttonStateOne) {
      buttonStateOne = reading1;
      // only toggle the LED if the new button state is HIGH
      if (buttonStateOne == LOW) {
        pressButtonOne();
      }
    }
  }
  lastButtonStateOne = reading1;

  // read the state of the switch into a local variable:
  int reading2 = digitalRead(BUTTONPIN2);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:
  // If the switch changed, due to noise or pressing:
  if (reading2 != lastButtonStateTwo) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading2 is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading2 != buttonStateTwo) {
      buttonStateTwo = reading2;
      // only toggle the LED if the new button state is HIGH
      if (buttonStateTwo == LOW) {
        pressButtonTwo();
      }
    }
  }
  lastButtonStateTwo = reading2;

  // read the state of the switch into a local variable:
  int reading3 = digitalRead(BUTTONPIN3);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:
  // If the switch changed, due to noise or pressing:
  if (reading3 != lastButtonStateThree) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading3 is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading3 != buttonStateThree) {
      buttonStateThree = reading3;
      // only toggle the LED if the new button state is HIGH
      if (buttonStateThree == LOW) {
        pressButtonThree();
      }
    }
  }
  lastButtonStateThree = reading3;

  // Update Display
  updateDisplay(displayMode);

  // Serial.println("--End--Loop--");

}
