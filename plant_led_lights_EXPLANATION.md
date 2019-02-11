# Explanation of Arduino Controller Code

**author: Joshua Cook**

**date: 2019-02-10**

## Overview

For those new to Arduino, there are two essential functions: `setup()` and `loop()`. Both are `void` (ie. there return no value). `setup()` runs once at the beginning upon start up of the machine, and `loop()` is the main function that continously loops forever - this is where the main commands of the code run.

The main purpose of the code is to check the time and set the lights appropriately. The temperature is also read and printed every loop. The complexity of the code is introduced by my over-ride button. Pressing this switches the lights to their opposite state (if they are on, it turns them off, and vice versa) and over-rides the timer. Pressing the button again, restores control to the timer.


## The Code

### Header section (runs before `setup()`)

First, I load the libraries I am using. The `OneWire.h` and `DallasTemperature.h` libraries are for the SSD1306 temperature sensor. `Wire.h` is the I2C library. `RTC.h` is for the real-time clock module. `Adafruit_GFX.h` and `Adafruit_SSD1306.h` are for the OLED screen (now that I think about it, I may not need to the graphics library).

```c
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
```

I then initialize a all of the modules. You can see which ones correspond to which piece in the comments.

```c
// initiate RTC module object
RTC_DS3231 rtc;

// IO pins
int relayPin = 7;   // the relay output pin
int buttonPin = 6;  // the button input pin

// OLED declarations
#define OLED_RESET -1  // no reset pin --> set to -1
Adafruit_SSD1306 display(OLED_RESET);

// one-wire SSD1306 declarations
#define ONE_WIRE_BUS 10
#define SSD1306_128_64
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature.
```

### The setup function

Finally, the `setup()` function. Here is it in full, and I broke it down below.

```c
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
``` 

I begin by designating the built-in LED as an output. 

```c
pinMode(LED_BUILTIN, OUTPUT);
```

Then, the `rtc` object is started.

```c
// begin RTC
rtc.begin();
```

The OLED screen is initiated with a blank screen. If you use this code, and the screen does not display anything, try using the address `0x3D` - it's usually one of these two. Note, when using this library, don't forget to call the `.display()` method to actually produce an image (in this case blank) on the screen.

```c
// setup the OLED display
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
display.clearDisplay();
display.display();
```

Next, the temperature sensor in started.

```c
// begin one-wire temp sensor
sensors.begin();
```

The serial communcation baud rate is set to 9600 (standard for the Uno). I added a 3 second pause (`delay(3000)`) to make sure the lights wouldn't just turn on right away (just in case, I guess).

```c
Serial.begin(9600);
delay(3000);
```

The commented out section is code that will reset the RTC time to the current time if it is disconnected from its power source. I leave this commented out because it only works when connected to a computer, not a wall-wart as I currently have it.

```c
if (rtc.lostPower()) {
	Serial.println("RTC lost power, lets set the time!");
	// following line sets the RTC to the date & time this sketch was compiled
	rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

```

I declare the input/output pins for the button and relay. The relay is turned `HIGH` which is `OFF`.

```c
// declare IO
pinMode(buttonPin, INPUT);
pinMode(relayPin, OUTPUT);
digitalWrite(relayPin, HIGH);
```

Finally, I call a function that I wrote (described later) to print the current temperature to the screen.

```c
// print current temperature at the beginning
printTemp();
```

### Global variables

I create some global variables that I like to keep between `setup()` and `loop()`. The first two hold the status for the lights (`false` = off, `true` = on), and if I have pressed the over-ride button.

```c
// current ON/OFF status of lights
bool lightStatus = false;
// if over-ride button is pressed
bool lightOverRide = false;
```
The next two global variables define on and off times for the lights.

```c
// time for lights on and off
int lightsOnHr = 10;   // turns ON at 10 am
int lightsOffHr = 22;  // turns OFF at 10 pm
```

### The loop function

Here is the entire `loop()` function that I then decompose below. On each loop, the bult-in LED is turned according the the over-ride button status, the current time is printed to the Serial console, if the over-ride is off, the lights are checked to be in the right state according to the time, the functions waits for a button press, and finishes by printing the temperature to the OLED. 

```c
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
```

#### Step 1. print the time to Serial

This only does anything, obviously, if the computer is connected.

```c
// print useful info. to Serial
DateTime now = rtc.now();
Serial.print("hour: ");
Serial.print(now.hour());
Serial.print("\tminute: ");
Serial.println(now.minute());
```

#### Step 2. set the lights

The first `if` statement, checks if the over-ride button has been pressed. If so, there is no need to check the lights. If the over-ride button has not been pushed, then the first `if` in the second layer checks if the current time is in the lights-on range, and if the lights are off, uses the `switchLights()` function to turn them on by passing `ture`. Otherwise, the `else if` checks if the time is outside of the lights-on range, and if the lights are on, turns them off by calling the same function and passing `false`.

```c
// check the current time against the time limits
if (!lightOverRide) {
    if ((now.hour() >= lightsOnHr & now.hour() < lightsOffHr) & !lightStatus) {
        switchLights(true);
    } else if ((now.hour() < lightsOnHr | now.hour() >= lightsOffHr) & lightStatus) {
        switchLights(false);
    }
}
```

#### Step 3. wait for button press

`int checkTimeDelay` holds a value for how long to wait for a button press. I start a timer with `unsigned long start_timer = millis();`, wait for a few milliseconds, and the collected the end time with `unsigned long end_timer = millis();`. By subtracting these two values on each loop of the `while`, I have created a timer. If this timer reaches `checkTimeDelay`, then the `while` stops and `loop()` continues.

Inside of the `while` statement, the state of the button is constantly checked, waiting to be pressed. If it isn't pressed, there is a bit of a delay (50 ms) and the `end_timer` variable is reset.

If the button is pressed, the global over-ride variable and lights are switched to their opposite state, the built-in LED is turned to reflect the status of the over-ride button, and there is a half-second delay to prevent double presses from accidentally registering.

```c
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
```

### My functions

This first function is fairly straight forward. It takes a boolean value and sets the lights to that state. This code could be neater by removing the if statements and using `digitalWrite(relayPin, !new_status);`, but I wanted the good looking Serial output. Notice, that this function sets the global variable `lightStatus`, not the if statements in the `loop()` function.

```c
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
```
The function `printTemp()` was primarily used to remove a block of ugly verbose code out of my `loop()` function. It reads the temperature (in Farenheit) using the two statements `sensors.requestTemperatures();` and `float ftemp = sensors.getTempFByIndex(0);`. Then, there is a whole process for writing this to the OLED screen: the display is cleared, text size set, color set, cursor positioned, the statement printed, and the display is shown (`.display()` method). I add a delay to prevent any quick accidental calls to set the display again.

```c
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
```

## TODO

I need to refactor the code such that turning off the over-ride turns the lights to the correct state is determined by the time. Currently, if the button is pressed while the lights are on, and not pressed until after the lights are supposed to be off, the lights will turn back on until the next loop through the timer (which is pretty quickly after).

I should just add the time to the OLED screen, too.