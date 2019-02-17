
// print the temperature to the OLED display
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



// this is just some fake code
