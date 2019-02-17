
// print the temperature to the OLED display
void printScreen(DateTime dt, bool or_status) {
    // call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    sensors.requestTemperatures(); // Send the command to get temperature readings
    float ftemp = sensors.getTempFByIndex(0);  // 0 refers to the first IC on the wire

    String or_string;
    if (or_status) {
        or_string = "OR: ON";
    } else {
        or_string = "OR: OFF";
    }
    
    // setup display
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);

    // line 1: print temperature
    display.print(ftemp);
    display.println(" F");

    // line 2: time
    String adj_minute = adjustMinuteString(dt.minute());
    display.print(dt.hour(), DEC);
    display.print(":");
    display.println(adj_minute);

    // line 3: date
    display.print(dt.month(), DEC);
    display.print("/");
    display.print(dt.day(), DEC);
    display.print("/");
    display.println(dt.year(), DEC);

    // line 4: override status:
    display.print(or_string);

    // print to screen
    display.display();
    delay(100);
}

// add a leading 0 to the minute if necessary
String adjustMinuteString(int input_minute) {
    if (input_minute < 10) {
        return(String("0" + String(input_minute, DEC)));
    } else {
        return(String(input_minute, DEC));
    }
}

// a quick message to display upon start up
void mySplashScreen() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);

    display.println();
    display.println("LED Growlight");
    display.println();
    display.println("by Josh Cook");
    display.println();
    display.println("starting...");

    display.display();
    delay(100);
}
