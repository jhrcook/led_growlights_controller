# Explanation of the GrowLight Class

**author: Joshua Cook**

**date: 2019-02-17**

## Overview

This is the first Arduino library I have ever created, and I never make my own classes in my daily job because I do a lot of data analysis with standard data structures in the R programming language. Therefore, I may not have done everything in a standard way, but it still works. If you notice any mistakes or improvements, please open an Issue in the GitHub repo [led\_growlights\_controller/issues](https://github.com/jhrcook/led_growlights_controller/issues "Issues - jhrcook/led_growlights_controller").

Here is a link to the [Arduino tutorial for creating libraries](https://www.arduino.cc/en/Hacking/libraryTutorial "Arduino - LibraryTutorial").

For a library in Arduino, there are two required files, the header (.h) and source file (.cpp). The header files has the definitions of the attributes and methods of the class, and the source file contains the actual code.

## The Header File

Below is the entire code for the header file in "GrowLight.h", followed by an explanation of each section.

### Code

```c
/*
    GrowLight.h - Library for controlling a LED grow light
    Created by Joshua H. Cook (2019/02/16)
*/

#ifndef GrowLight_h
#define GrowLight_h

#include "Arduino.h"

class GrowLight {
    private:
        int controlPin;
        int onHour;
        int offHour;
        bool lightStatus;
        bool overrideStatus;
        void switchLightsOnOff(bool state);
    
    public:
        GrowLight(int control_pin, int on_hour, int off_hour);
        void updateLights(int current_hour);
        void overrideLights(int current_hour);
        bool getLightStatus();
        bool getOverrideStatus();
};

#endif
```
### Explanation

The code begins with an informative header.

```c
/*
    GrowLight.h - Library for controlling a LED grow light
    Created by Joshua H. Cook (2019/02/16)
*/
```

This is followed by some a wrapper that prevents the code from being loaded twice. I'm not entirely sure what this does, but here is my best guess. The `#ifndef` statement stands for "if not defined," therefore, if the library has already been loaded, this will be sipped. The `#endif` statement closes this "if" statement.

```c
#ifndef GrowLight_h
#define GrowLight_h

...

#endif
```

I then explicitly load the Arduino library.

```c
#include "Arduino.h"
```

Finally, I define my class `GrowLight`. There are two sections: `private` and `public`. Basically, put the variables and attributes that can safely be accessed by outsiders into the `public` section. Thus, I put attributes and ability to switch the lights ON or OFF (`switchLightsOnOff`) in `private`, and let the user access them through the methods in the `public` section.

```c
class GrowLight {
    private:
        int controlPin;
        int onHour;
        int offHour;
        bool lightStatus;
        bool overrideStatus;
        void switchLightsOnOff(bool state);
    
    public:
        GrowLight(int control_pin, int on_hour, int off_hour);
        void updateLights(int current_hour);
        void overrideLights(int current_hour);
        bool getLightStatus();
        bool getOverrideStatus();
};
```

## The Source File

The actual code for each attribute or method in the header file is contained in the source file (.cpp). Below is the code in the "GrowLight.cpp" file and each section is explained below.

### Code

```c
/*
    GrowLight.h - Library for controlling a LED grow light
    Created by Joshua H. Cook (2019/02/16)
*/

#include "Arduino.h"
#include "GrowLight.h"

GrowLight::GrowLight(int control_pin, int on_hour, int off_hour) {
    controlPin = control_pin;
    pinMode(controlPin, OUTPUT);
    onHour = on_hour;
    offHour = off_hour;

    lightStatus = false;
    switchLightsOnOff(lightStatus);
    
    overrideStatus = false;
}

// update the lights given the current time
void GrowLight::updateLights(int current_hour) {
    if (!overrideStatus) {
        if (onHour <= current_hour && current_hour < offHour && !lightStatus) {
            switchLightsOnOff(true);
        } else if ((current_hour < onHour | offHour <= current_hour) && lightStatus) {
            switchLightsOnOff(false);
        }
    }
}

// turn the lights on or off
void GrowLight::switchLightsOnOff(bool state) {
    if (state) {
        digitalWrite(controlPin, LOW);   // turn lights ON
        lightStatus = state;
    } else {
        digitalWrite(controlPin, HIGH);  // turn lights OFF
        lightStatus = state;
    }
}

// switch to override status
void GrowLight::overrideLights(int current_hour) {
    overrideStatus = !overrideStatus;
    if (overrideStatus) {
        switchLightsOnOff(!lightStatus);
    } else {
        updateLights(current_hour);
    }
}

// request the current state of the lights
bool GrowLight::getLightStatus() {
    return lightStatus;
}

// request the current state of the override
bool GrowLight::getOverrideStatus() {
    return overrideStatus;
}
```

### Explanation

Again, the file starts with a header. According to the Arduino documentation, it is customary to copy it from the header file (.h).

```c
/*
    GrowLight.h - Library for controlling a LED grow light
    Created by Joshua H. Cook (2019/02/16)
*/
```

Next, I inlcude the Arduino library and the header file for this library.

```c
#include "Arduino.h"
#include "GrowLight.h"
```

The first function is the "constructor". This is the function automatically called upon a new instance of the class (similar to `.__init__(self)` in Python) Notice that is does not have a type assignment. It takes three arguments and assigns them to attributes defined in the header file. Further, I included the declaration of the mode of the control pin (the pin to the relay switch). Also, the constructur initiates the object with the lights off using `lightStatus = false` and `switchLightsOnOff(lightStatus)`. Finally, the object is initiated with the over-ride OFF.

```c
GrowLight::GrowLight(int control_pin, int on_hour, int off_hour) {
    controlPin = control_pin;
    pinMode(controlPin, OUTPUT);
    onHour = on_hour;
    offHour = off_hour;

    lightStatus = false;
    switchLightsOnOff(lightStatus);
    
    overrideStatus = false;
}
```

The `updateLights` function is responsible for controlling the lights in response to the over-ride status and current time. If the over-ride status of the object is ON, then everything is skipped. Otherwise, the current time is compared to the ON and OFF times to decide to turn the light ON or OFF.

```c
// update the lights given the current time
void GrowLight::updateLights(int current_hour) {
    if (!overrideStatus) {
        if (onHour <= current_hour && current_hour < offHour && !lightStatus) {
            switchLightsOnOff(true);
        } else if ((current_hour < onHour | offHour <= current_hour) && lightStatus) {
            switchLightsOnOff(false);
        }
    }
}
```

The `switchLightsOnOff` is a private method for switching of the lights. Remember, in the header file, I declared this as `private` to prevent the user from controlling it manually. This was to prevent disruptions to the logic system of the class. It takes a boolean value and assigns it to the `lightStatus` as well as directly changing the relay switch.

```c
// turn the lights on or off
void GrowLight::switchLightsOnOff(bool state) {
    if (state) {
        digitalWrite(controlPin, LOW);   // turn lights ON
        lightStatus = state;
    } else {
        digitalWrite(controlPin, HIGH);  // turn lights OFF
        lightStatus = state;
    }
}
```

This method is called to over-ride the lights. If going from OFF to ON, the lights are switched to their opposite status. If going from ON to OFF, the decision of how to adjust the lights is given to the timer by calling `updateLights` passing the current time.

```c
// switch to override status
void GrowLight::overrideLights(int current_hour) {
    overrideStatus = !overrideStatus;
    if (overrideStatus) {
        switchLightsOnOff(!lightStatus);
    } else {
        updateLights(current_hour);
    }
}
```
The last two wrapper methods just return the status of the lights or over-ride switch. I included these are safe ways for the user to access the information.

```c
// request the current state of the lights
bool GrowLight::getLightStatus() {
    return lightStatus;
}

// request the current state of the override
bool GrowLight::getOverrideStatus() {
    return overrideStatus;
}
```



---
### TODO

I may switch to accepting `DateTime` objects instead of integers for time. This would allow for more specific timings of the lights and seems more intuitive from a user stand-point.
