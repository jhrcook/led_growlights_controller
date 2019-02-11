#include <OneWire.h> 
#include <DallasTemperature.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, HIGH);

    // print current temperature at the beginning
    printTemp();
}

// current ON/OFF status of lights
bool lightStatus = false;
// if over-ride button is pressed
bool lightOverRide = false;

// time for lights on and off
int lightsOnHr = 10;   // turns ON at 10 am
int lightsOffHr = 22;  // turns OFF at 10 pm

void loop() {
    // builtin-led ON/OFF according to over-ride status
    digitalWrite(LED_BUILTIN, lightOverRide);
    
    // print useful info. to Serial
    DateTime now = rtc.now();
    Serial.print("hour: ");
    Serial.print(now.hour());
    Serial.print("\tminute: ");
    Serial.println(now.minute());
    
    // check the current time against the time limits
    if (!lightOverRide) {
        if ((now.hour() >= lightsOnHr & now.hour() < lightsOffHr) & !lightStatus) {
            switchLights(true);
        } else if ((now.hour() < lightsOnHr | now.hour() >= lightsOffHr) & lightStatus) {
            switchLights(false);
        }
    }
    
    // check time every 15 sec, otherwise look for button presses
    int checkTimeDelay = 15000;
    unsigned long start_timer = millis();
    delay(10);
    unsigned long end_timer = millis();
    while(end_timer > start_timer & checkTimeDelay > (end_timer - start_timer)) {
        int buttonState = digitalRead(buttonPin);
        if (buttonState == HIGH) {
            lightOverRide = !lightOverRide;
            switchLights(!lightStatus);
            Serial.println("button pressed");
            digitalWrite(LED_BUILTIN, lightOverRide);
            delay(500);
        }
        delay(50);
        end_timer = millis();
    }

    printTemp();  // update OLED display with current temp
}

// turn lights ON (true) or OFF (false)
void switchLights(bool new_status) {
    if (new_status) {
        Serial.println("\t--- Turning lights ON ---");
        digitalWrite(relayPin, LOW);
        lightStatus = new_status;
    } else {
        Serial.println("\t--- Turning lights OFF ---");
        digitalWrite(relayPin, HIGH);
        lightStatus = new_status;
    }
}

void printTemp(void) {
    // call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    sensors.requestTemperatures(); // Send the command to get temperature readings
    float ftemp = sensors.getTempFByIndex(0);  // 0 refers to the first IC on the wire
    // display on OLED
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("TEMP.");
    display.println();
    display.print(ftemp);
    display.println(" F");
    display.display();
    delay(250);
}
