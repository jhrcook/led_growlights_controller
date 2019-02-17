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
