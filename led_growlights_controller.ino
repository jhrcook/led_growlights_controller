#include <OneWire.h> 
#include <DallasTemperature.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_SSD1306.h>
#include "GrowLight.h"

// initiate RTC module object
RTC_DS3231 rtc;

// IO pins
int relayPin = 7;
int buttonPin = 6;

// OLED declarations
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

// one-wire SSD1306 declarations
#define ONE_WIRE_BUS 10
#define SSD1306_128_64
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature.

// self-defined grow-lights object
GrowLight gl(relayPin, 10, 22);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    
    // begin RTC
    rtc.begin();
    
    // setup the OLED display
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.display();
    
    // begin one-wire temp sensor
    sensors.begin();
    
    Serial.begin(9600);
    delay(3000);
    
//    if (rtc.lostPower()) {
//        Serial.println("RTC lost power, lets set the time!");
//        // following line sets the RTC to the date & time this sketch was compiled
//        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//    }
    
    // declare IO
    pinMode(buttonPin, INPUT);
}

void loop() {
    // print the current temperature
    printTemp();
    
    // builtin-led ON/OFF according to over-ride status
    digitalWrite(LED_BUILTIN, gl.getOverrideStatus());
    
    // print useful info. to Serial
    DateTime now = rtc.now();
    Serial.print("hour: ");
    Serial.print(now.hour());
    Serial.print("\tminute: ");
    Serial.println(now.minute());
    
    // check the current time against the time limits
    gl.updateLights(now.hour());
    
    // check time every 15 sec, otherwise look for button presses
    int checkTimeDelay = 15000;
    unsigned long start_timer = millis();
    unsigned long end_timer = start_timer;
    while(end_timer >= start_timer && checkTimeDelay > (end_timer - start_timer)) {
        int buttonState = digitalRead(buttonPin);
        if (buttonState == HIGH) {
            gl.overrideLights(now.hour());
            Serial.println("button pressed");
            digitalWrite(LED_BUILTIN, gl.getOverrideStatus());
            delay(500);
            break;
        }
        delay(50);
        end_timer = millis();
    }
}
