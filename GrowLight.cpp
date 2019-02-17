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
        } else if (current_hour < onHour && offHour <= current_hour && lightStatus) {
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
