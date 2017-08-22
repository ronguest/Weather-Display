Weather Display
========

* Runs on Arduino Yun
* Displays the weather on a 2.8" Adafruit TFT with capacitive touch
https://learn.adafruit.com/adafruit-2-8-tft-touch-shield-v2/capacitive-touchscreen-paint-demo
* Two Python scripts are run by crontab on the Linux side.
  - One of the scripts grabs the current conditions (temperature and humidity)
  - The other gets the forecast highs and low for the current day and the next day

* The Arduino code reads the values from the SD card periodically and updates the display

* Touching the display causes the bottom portion to rotate between Tomorrow's forecast, indoor data, and details about today

* Linux side is using JSON from weather underground to fetch conditions and forecast data
http://www.wunderground.com/weather/api/d/436da0958aa624c8/edit.html
