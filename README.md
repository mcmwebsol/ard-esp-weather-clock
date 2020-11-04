# ard-esp-weather-clock
Clock with Weather for ESP8266/Arduino with MD_MAX72xx for display

It displays the current time in 24 hour format followed by the temperature in fahrenheit and the current weather condition as an icon.

Weather code based on https://randomnerdtutorials.com/esp8266-weather-forecaster/

Clock (NTP) code based on https://lastminuteengineers.com/esp8266-ntp-server-date-time-tutorial/

Obtain an Open Weather Map API key at https://home.openweathermap.org/users/sign_up

To display the temperature in Celsius instead of Fahrenheit change "units=imperial" to "units=metric" on line 513

Tested with ESP8266 and MAX7219 4 8x8 Dot Matrix Module - LED 

Built with the Arduino IDE

Use a separate file named secrets.h for your API key and WiFi login
e.g.
#define SSIDW "WIFI_LOGIN_HERE"

#define WIFI_PASSWORD "WIFI_PASSWORD_HERE"

#define API_KEY "OPEN_WEATHER_API_KEY_HERE"


Be sure to use the older 5.13.5 version of ArduinoJson as the newer versions are not compatible.
