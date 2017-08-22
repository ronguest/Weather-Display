//
//  Copyright (C) 2017 Ronald Guest <http://about.me/ronguest>

/***************************************************

Displays the weather on a 2.8" Adafruit TFT. Two Python scripts are run by cronlog on the Linux side.
One of the scripts grabs the current conditions (temperature and humidity) and the other gets the
forecast highs and low for the current day and the next day.

The Arduino script reads the values from the SD card periodically and updates the display.

Linux side is using JSON from weatherunderground to fetch conditions and forecast data
http://www.wunderground.com/weather/api/d/436da0958aa624c8/edit.html
 ****************************************************/

#include "WeatherDisplay.h"

long lastDownloadUpdate = -(1000L * UPDATE_INTERVAL_SECS)-1;    // Forces initial screen draw

void setup() {
  Bridge.begin();
  Console.begin();
  FileSystem.begin();

  Console.println("capacitive TFT");
  if (!ts.begin()) {
    Console.println("Couldn't start touchscreen controller");
    while (1);
  }
  Console.println("Touchscreen started");

  tft.begin();

  // Rotate the display 180 degrees so the power cable comes out on the bottom
  // Parameter values are: 0=cord on top, 1=cord on right, 2=cord on bottom, 3=cord on left
  tft.setRotation(2);

  // Set some TFT metrics we need regularly
  tftHeight = tft.height();
  tftMiddle = tftHeight/2;
  tftWidth = tft.width();

  tft.setTextSize(extrasTextSize-1);
  tft.setTextColor(ILI9341_GREEN);
  tft.println("Please wait");
  for (int i=0; i < 5; i++) {
    tft.print(".");
    delay(1000);        // Seems to be a startup issue when first powered on, maybe a delay getting the SD card going?
  }
}

void loop(void) {
  // Check if we should update weather information
  if ((millis() - lastDownloadUpdate) > (1000L * UPDATE_INTERVAL_SECS)) {
    reDrawBottom = true;
    lastDownloadUpdate = millis();

    // Clear the screen
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, 0);

    // Load the latest weather data every updateMinutes
    loadData();

    // Display the current temperature
    displayHeader(F("    TODAY"));
    displayCurrent();
  }

  // If user touches screen, toggle bottom display values
  // If reDrawBottom true then just draw the bottom again, don't change mode
  if (ts.touched() || reDrawBottom) {
    eraseBottom();
    tft.setCursor(0, tftMiddle+separatorWidth);
    switch (bottom) {
      case forecast:              // Display indoor temp and humidity
        displayTodaysForecast();
        displayTodaysExtras();
        if (!reDrawBottom) bottom = todayExtras;
        break;
      case todayExtras:          // Display humidity and expected conditions today
        displayTodaysExtras();
        if (!reDrawBottom) bottom = tomorrow;       // Change content for next iteration of bottom display
        break;
      case tomorrow:             // Display tomorrow's forecast
        //drawSeparator();
        // Add a spacer below the separator line so the text doesn't touch the line
        //tft.setTextSize(1);
        //tft.println();
        displayHeader(F("  TOMORROW"));
        displayTomorrowsForecast();
        if (!reDrawBottom) bottom = indoor;        // Change content for next iteration of bottom display
        break;
      case indoor:              // Display indoor temp and humidity
        tft.setTextSize(1);
        tft.println();
        displayHeader(F("   INDOOR"));
        displayIndoor();
        if (!reDrawBottom) bottom = forecast;
        break;
    }
    reDrawBottom = false;
  }
}

// Load all the data
void loadData() {
  loadCurrent();
  loadToday();
  loadTomorrow();
  loadIndoor();
}

// Load batch one of data (this is one of several load data functions)
void loadCurrent() {
  String value;      // temporary String for reading strings from files

  // Get current temperature and humidity
  File curTempFile = FileSystem.open(TemperatureFileName, FILE_READ);
  if (readFile(curTempFile, value)) {
    // Round the temperature here
    float f = value.toFloat();
    f = f + 0.5f;
    currentTemperature = (int) f;
    curTempFile.close();
  }
  value="";
  File humidityFile = FileSystem.open(HumidityFileName, FILE_READ);
  if (readFile(humidityFile, value)) {
    humidityFile.close();
  }
  currentHumidity = value.toInt();
  //value="";
}

void loadToday() {
  String value;

  // Get today's forecast: hi, low, conditions and pop
  File lowFile = FileSystem.open(TodaysForecastLowFileName, FILE_READ);
  if (readFile(lowFile, value)) {
    lowFile.close();
  }
  todayConditions.setLow(value.toInt());
  value="";
  File highFile = FileSystem.open(TodaysForecastHighFileName, FILE_READ);
  if (readFile(highFile, value)) {
    highFile.close();
  }
  todayConditions.setHigh(value.toInt());
  //value="";

  todayPop = "";    // Need to null it out before reading new value
  File todPopFile = FileSystem.open(TodaysForecastPopFileName, FILE_READ);
  if (readFile(todPopFile, todayPop)) {
    todPopFile.close();
  }
  todayForecastConditions="";    // Need to null it out before reading new value
  File todCondFile = FileSystem.open(TodaysForecastConditionsFileName, FILE_READ);
  if (readFile(todCondFile, todayForecastConditions)) {
    todCondFile.close();
  }
  //value="";
}

// Second of two load data functions
void loadTomorrow() {
  String value;

  // Get tomorow's Low and High
  File tomLowFile = FileSystem.open(TomorrowsForecastLowFileName, FILE_READ);
  if (readFile(tomLowFile, value)) {
    tomLowFile.close();
  }
  tomorrowConditions.setLow(value.toInt());
  value="";
  File tomHighFile = FileSystem.open(TomorrowsForecastHighFileName, FILE_READ);
  if (readFile(tomHighFile, value)) {
    tomHighFile.close();
  }
  tomorrowConditions.setHigh(value.toInt());
  //value="";

  // Tomorrow's Pop and Conditions
  tomorrowForecastConditions = "";    // null out before reading new value
  File conditionsFile = FileSystem.open(TomorrowsForecastConditionsFileName, FILE_READ);
  if (readFile(conditionsFile, tomorrowForecastConditions)) {
    conditionsFile.close();
  }
  tomorrowPop = "";  // null out before reading new value
  File popFile = FileSystem.open(TomorrowsForecastPopFileName, FILE_READ);
  if (readFile(popFile, tomorrowPop)) {
    popFile.close();
  }
}

void loadIndoor() {
  String value;

  // Get indoor conditions: temp and humidity
  File inTempFile = FileSystem.open(InsideTemperatureFileName, FILE_READ);
  if (readFile(inTempFile, value)) {
    // The Arduino UNO+XBee reports temp with a tenth after a period as in 76.1
    // The Ecobee API reports without the period but with the tenth, as in 761
    // so if using the Ecobee for indoor conditions need to get the float and then divide by ten
    // Need to round the indoor temperature
    //Console.print("Ins temp value is ");Console.println(value);
    float f = value.toFloat();
    //Console.print("Inside temp ");Console.println(f);
    f = f / 10.0f;    // Comment this out if not using the Ecobee, see comment above, as ecobee reports 76.1 sa 761
    //Console.print("Inside temp / 10 ");Console.println(f);
    f = f + 0.5f;
    //Console.print("Rounded temp ");Console.println(f);
    indoorConditions.setTemperature((int) f);
    inTempFile.close();
  }
  value = "";
  File inHumidityFile = FileSystem.open(InsideHumidityFileName, FILE_READ);
  if (readFile(inHumidityFile, value)) {
    // Need to round the indoor humidity
    float f = value.toFloat();
    f = f + 0.5f;
    indoorConditions.setHumidity((int) f);
    inHumidityFile.close();
  } //else { Console.println("inHumFile err"); }
  //value="";
}

void displayCurrent() {
  // Read the current temperature from the SD card and draw it on the screen

  printIntTemperature(currentTemperature, DefaultTextSize, 2);
  // Add space for the next set of data
  tft.setTextSize(DefaultTextSize);
  tft.println();
}

void displayHumidity(int textSize, int textColor) {
  // Read the current humidity from the SD card and draw it on the screen
  // Add a bit of spacer below the middle of the screen
  //tft.setTextSize(3);
  //tft.println("");
  tft.setTextColor(textColor);
  tft.setTextSize(textSize);
  tft.print(F("Humid: "));
  tft.print(currentHumidity);
  tft.println("%");
}

void displayTodaysForecast() {
  // Display  today's low & high temperature on the screen
  printIntTemperature(todayConditions.getLow(), forecastTextSize, 2);
  printIntTemperature(todayConditions.getHigh(), forecastTextSize, 2);
}

void displayTodaysExtras() {
  // Read  today's humidity, conditions and pop
  String conditions;

  displayHumidity(extrasTextSize, extrasTextColor);

  if (todayPop.toInt() > 0) {
    tft.setTextColor(pickPopColor(todayPop));
    tft.setTextSize(extrasTextSize);
    tft.print(F("Precip: "));
    tft.print(todayPop);
    tft.println(F("%"));
  }

  tft.setTextColor(extrasTextColor);

  // Display the conditions, adjust to multiple lines if necessary
  displayConditions(extrasTextSize, todayForecastConditions);

  // Add spacer text for next set of content
//  tft.setTextSize(extrasTextSize);
//  tft.println();
}

void displayIndoor() {
  // Display indoor temp and humidity

  printIntTemperature(indoorConditions.getTemperature(), forecastTextSize, 2);
  tft.print(" ");    // Add a space before printing the humidity

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(forecastTextSize);
  tft.print(indoorConditions.getHumidity());
  tft.println("%");
}

void displayTomorrowsForecast() {
  // Display  tomorrow's low & high temperature on the screen
  String pop, conditions;

  printIntTemperature(tomorrowConditions.getLow(), forecastTextSize, 2);
  printIntTemperature(tomorrowConditions.getHigh(), forecastTextSize, 2);

  // Move to next line for the conditions text
  tft.setTextSize(forecastTextSize);
  tft.println();

  if (tomorrowPop.toInt() > 0 ){
    // Only show pop if it might actually rain/snow
    tft.setTextColor(pickPopColor(pop));
    tft.setTextSize(extrasTextSize);
    tft.print(F("Precip: "));
    tft.print(tomorrowPop);
    tft.println(F("%"));
  }

  // Display the conditions, adjust to multiple lines if necessary
  displayConditions(extrasTextSize, tomorrowForecastConditions);
}

void displayConditions(int size, String text) {
  int textLength;

  textLength = text.length();
  tft.setTextSize(extrasTextSize);
  if (textLength <= 13) {
    tft.println(text);
  } else {
    // Won't fit on one line
    // If it is longer than 26 it won't fit on two lines either so go with a smaller font
    if (textLength > 26) {
      tft.setTextSize(extrasTextSize-2);
    }
    // As a start, just break the text after the final space in the string
    int spaceIndex = text.lastIndexOf(' ');
    tft.println(text.substring(0,spaceIndex));
    tft.println(text.substring(spaceIndex+1,text.length()));
  }
}

void displayHeader(String header) {
  // Displays the header text
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_RED);
  tft.println(header);
  tft.setTextSize(1);
  tft.println(" ");
}

/* Unused right now
void drawSeparator() {
  // draws a line between today and tomorrow's content
  int height = tft.height();
  int lineLocation = (height/2) + separatorWidth;    // The +2 is how far below the middle of the screen to draw the text

  // A one pixel line is too narrow so we actually draw lineWidth lines to make it heavier
  for (int i=0; i<separatorWidth; i++) {
    tft.drawFastHLine(0, (lineLocation + i), tft.width(), ILI9341_MAGENTA);
  }
}*/

long pickColor(int number) {
  // Take a value (temperature) and returns the corresponding color for the text display
  if (number > 100) {
    return ILI9341_RED;
  } else if (number > 90) {
    return ORANGE;
  } else if (number > 80 ) {
    return ILI9341_YELLOW;
  } else if (number > 60) {
    return ILI9341_GREEN;
  } else if (number > 40) {
    return ILI9341_WHITE;
  } else if (number > 30) {
    return ILI9341_BLUE;
  } else {
    return ILI9341_MAGENTA;
  }
/* Should be coded a better way
   - Red if above 100
   - Yellow between 80 and 100
   - Green between 60 and 80
   - White between 40 and 60
   - Blue between 30 and 40
   - Magenta below 30
   - Dark orange might work 0xFF8C00
*/
}

// Erase (paint black) the bottom half of the screen so it can update independently
void eraseBottom() {
  int lineLocation = tftMiddle + separatorWidth;    // separatorWidth is how far below the middle of the screen to draw the text

  tft.fillRect(0, lineLocation, tftWidth, tftMiddle, ILI9341_BLACK);
}

// This is the common code for displaying temperature
// Attempts to deal with 100+ degrees
void printIntTemperature(int temperature, int textSize, int degreeSize) {

  tft.setTextColor(pickColor(temperature));
  tft.setTextSize(textSize);
  if (temperature < 100) {
    // if not triple digit temp need to pad at the front
    tft.print(" ");
    if ((temperature < 10) && (temperature >= 0)) {
      // If single digit temperature >0 but <10 need to add another pad
      tft.print(" ");
    }
  }
  tft.print(temperature);
  // Reduce the text size and print a 'o' small like a degree symbol
  tft.setTextSize(degreeSize);
  tft.print("o");
}

// Reads the string value from the specified file
// Returns true on success
boolean readFile(File dataFile, String& value) {
  if (dataFile) {
    while (dataFile.available() > 0) {
      char c = dataFile.read();
      if (c == '\n') {
        break;
      } else {
        value += c;
      }
    }
    dataFile.close();
    return true;
  } else {
//    Console.print(F("File open err"));
    return false;
  }
}

// Pick the text color for the probability of precipitation
long pickPopColor(String pop) {
  int number;

  number = pop.toInt();        // Get the int value from the String
  if (number >= 70) {
    return ILI9341_RED;
  } else if (number >= 50) {
    return ILI9341_YELLOW;
  } else if (number >=30) {
    return ILI9341_WHITE;
  } else {
    return ILI9341_GREEN;
  }
}
