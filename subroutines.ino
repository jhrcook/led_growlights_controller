
// print the temperature to the OLED display
void printTemp(DateTime dt, bool or_status) {
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
    display.print(dt.hour(), DEC);
    display.print(":");
    display.println(dt.minute(), DEC);

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
