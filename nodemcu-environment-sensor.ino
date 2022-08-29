#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>
#include "Max44009.h"

// OLED Display
#define SCREEN_ADDRESS 0x3C // OLED I2C Address
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Temp/Humid Sensor
#define SENSOR_ADDRESS 0x38; // Sensor I2C Address
Adafruit_AHTX0 aht;
int TEMPF = 0;
int TEMPC = 0;
int HMDTY = 0;

//Light Sensor
#define LIGHT_SENSOR_ADDRESS 0x4A // OLED I2C Address
Max44009 myLux(LIGHT_SENSOR_ADDRESS);
uint32_t lastDisplay = 0;
float luxNum = 0;
int luxLineLength = 0;

// Button
// constants won't change. They're used here to set pin 
// const int buttonPin = 12;    // the number of the pushbutton
// const int buttonPin2 = 13;    // the number of the pushbutton
// const int buttonPin3 = 15;    // the number of the pushbutton

// Variables will change:
int buttonStateOne;             // the current reading from the input pin
int lastButtonStateOne = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void pressButtonOne() {
  Serial.println("PRESSED BUTTON ONE");
}

void update_display() {

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
  display.print(TEMPF);

  display.setTextSize(1);
  display.setCursor(82, 0);
  display.print("Celsius");
  
  display.setTextSize(2);
  display.setCursor(92, 11);
  display.print(TEMPC);
  display.drawCircle(120, 11, 2, SSD1306_WHITE);


  display.setTextSize(1);
  display.setCursor(82, 29);
  display.print("Humid");

  display.setTextSize(2);
  display.setCursor(92, 41);
  display.print(HMDTY);
  display.drawCircle(120, 43, 2, SSD1306_WHITE);
  display.drawLine(125, 43, 119, 53, SSD1306_WHITE);
  display.drawCircle(124, 52, 2, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 56);
  display.print("Lux:");

  int luxLineMeterLimit = 2000;
  int luxLineLimit = 104;
  // luxLineLength = 20;
  luxLineLength = (luxNum / luxLineMeterLimit)*luxLineLimit;
  if(luxNum > luxLineMeterLimit) luxLineLength = luxLineLimit;
  display.fillRect(24, 57, luxLineLength, 6, SSD1306_WHITE);

  display.display();

}

void setup(void) {

  Serial.begin(115200);
  Serial.print("\nStart max44009_interrupt : ");
  Serial.println(MAX44009_LIB_VERSION);

  Wire.begin();
  myLux.setContinuousMode();

  myLux.setHighThreshold(30);
  Serial.print("HighThreshold:\t");
  Serial.println(myLux.getHighThreshold());

  myLux.setLowThreshold(10);
  Serial.print("LowThreshold:\t");
  Serial.println(myLux.getLowThreshold());

  myLux.setThresholdTimer(2);

  myLux.enableInterrupt();

  // Buttons
  // pinMode(buttonPin, INPUT);
  // pinMode(buttonPin2, INPUT);
  // pinMode(buttonPin3, INPUT);

  // Temperature Sensor
  if (! aht.begin()) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Could not find AHT? Check wiring");
    display.display();
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay(); // Clear
  display.setTextColor(SSD1306_WHITE); // Draw white text (white ~= LED-ON)
  display.cp437(true); // Use full 256 char 'Code Page 437' font
  display.dim(false);

}

void loop() {

  Serial.println("--Begin--Loop--");

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
      Serial.print("Error:\t");
      Serial.println(err);
    }
    else
    {
      Serial.print("lux:\t");
      Serial.print(lux);
      if (st == 1) Serial.println("\tIRQ occurred");
      else Serial.println();
    }
  }

  // Measure Temp/Humid
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  TEMPC = temp.temperature;
  TEMPF = (temp.temperature * 1.8) + 32; // convert to Fahrenheit 
  HMDTY = humidity.relative_humidity;
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  // Buttons

  // // read the state of the switch into a local variable:
  // int reading = digitalRead(buttonPin);
  // // check to see if you just pressed the button
  // // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // // since the last press to ignore any noise:
  // // If the switch changed, due to noise or pressing:
  // if (reading != lastButtonStateOne) {
  //   // reset the debouncing timer
  //   lastDebounceTime = millis();
  // }
  // if ((millis() - lastDebounceTime) > debounceDelay) {
  //   // whatever the reading is at, it's been there for longer than the debounce
  //   // delay, so take it as the actual current state:
  //   // if the button state has changed:
  //   if (reading != buttonStateOne) {
  //     buttonStateOne = reading;
  //     // only toggle the LED if the new button state is HIGH
  //     if (buttonStateOne == LOW) {
  //       pressButtonOne();
  //     }
  //   }
  // }
  // lastButtonStateOne = reading;

  // Update Display
  update_display();

  Serial.println("--End--Loop--");
  delay(800);
}
