#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MD_MAX72xx.h> 
#include <SPI.h>
#include <string.h>
#include <unistd.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define PRINT(s, v) { Serial.print(F(s)); Serial.print(v); }

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
//MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 4 // ?? 

#define CLK_PIN   13  // or SCK
#define DATA_PIN  14  // or MOSI  (DIn)
#define CS_PIN    15  // or SS

char minutesRealDisplay[3];  

char s1 =  '+'; // icon for sunny/clear in custom font - was too wide - set limit to width of 5 dot ( was @ and before that } character)
char empty1 = ' '; // TODO - create an icon for "other" weather conditions?
char r1 = '{'; // icon for rain in custom font
char t1 = ';'; // was '|'; // icon for thunderstorms in custom font
char c1 = '<'; // was '~'; // icon for clouds in custom font

// SPI hardware interface
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary pins
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// custom font built using https://pjrp.github.io/MDParolaFontEditor 
MD_MAX72XX::fontType_t newFont[] PROGMEM = 
{
  0,  // 0   
  0,  // 1   
  0,  // 2   
  0,  // 3   
  0,  // 4   
  0,  // 5   
  0,  // 6   
  0,  // 7   
  0,  // 8   
  0,  // 9   
  0,  // 10   
  0,  // 11   
  0,  // 12   
  0,  // 13   
  0,  // 14   
  0,  // 15   
  0,  // 16   
  0,  // 17   
  0,  // 18   
  0,  // 19   
  0,  // 20   
  0,  // 21   
  0,  // 22   
  0,  // 23   
  0,  // 24   
  0,  // 25   
  0,  // 26   
  0,  // 27   
  0,  // 28   
  0,  // 29   
  0,  // 30   
  0,  // 31   
  1, 0,   // 32    
  0,  // 33   
  0,  // 34   
  0,  // 35   
  0,  // 36   
  0,  // 37   
  0,  // 38   
  0,  // 39   
  0,  // 40   
  0,  // 41   
  0,  // 42   
  5, 96, 144, 144, 96, 0,   // 43    
  0,  // 44   
  0,  // 45   
  0,  // 46   
  0,  // 47   
  3, 254, 130, 254,   // 48    
  1, 254,   // 49    
  3, 226, 146, 158,   // 50    
  3, 146, 146, 254,   // 51    
  3, 30, 16, 254,   // 52    
  3, 142, 146, 242,   // 53    
  3, 254, 146, 242,   // 54    
  3, 14, 2, 254,  // 55   
  3, 254, 146, 254,   // 56    
  3, 30, 18, 254,   // 57    
  1, 40,  // 58   
  8, 76, 186, 81, 177, 14, 0, 0, 0,   // 59  
  8, 12, 26, 17, 17, 14, 0, 0, 0,   // 60  
  0,  // 61   
  0,  // 62   
  0,  // 63   
  5, 32, 112, 216, 112, 32,   // 64    
  0,  // 65   
  0,  // 66   
  3, 254, 130, 130,   // 67    
  0,  // 68   
  0,  // 69   
  0,  // 70   
  0,  // 71   
  0,  // 72   
  0,  // 73   
  0,  // 74   
  0,  // 75   
  0,  // 76   
  0,  // 77   
  0,  // 78   
  0,  // 79   
  0,  // 80   
  0,  // 81   
  3, 254, 50, 220,  // 82   
  3, 156, 146, 114,   // 83    
  3, 2, 254, 2,   // 84    
  0,  // 85   
  0,  // 86   
  0,  // 87   
  0,  // 88   
  0,  // 89   
  0,  // 90   
  0,  // 91   
  0,  // 92   
  0,  // 93   
  0,  // 94   
  0,  // 95   
  0,  // 96   
  0,  // 97   
  0,  // 98   
  0,  // 99   
  0,  // 100   
  0,  // 101   
  0,  // 102   
  0,  // 103   
  0,  // 104   
  0,  // 105   
  0,  // 106   
  0,  // 107   
  0,  // 108   
  0,  // 109   
  0,  // 110   
  0,  // 111   
  0,  // 112   
  0,  // 113   
  0,  // 114   
  0,  // 115   
  0,  // 116   
  0,  // 117   
  0,  // 118   
  0,  // 119   
  0,  // 120   
  0,  // 121   
  0,  // 122   
  8, 3, 96, 12, 0, 51, 0, 204, 0,   // 123    
  8, 12, 90, 180, 19, 81, 178, 20, 8,   // 124    
  8, 68, 56, 70, 196, 68, 58, 72, 8,  // 125   
  8, 48, 88, 80, 76, 68, 72, 80, 32,  // 126   
  0,  // 127   
  0,  // 128   
  0,  // 129   
  0,  // 130   
  0,  // 131   
  0,  // 132   
  0,  // 133   
  0,  // 134   
  0,  // 135   
  0,  // 136   
  0,  // 137   
  0,  // 138   
  0,  // 139   
  0,  // 140   
  0,  // 141   
  0,  // 142   
  0,  // 143   
  0,  // 144   
  0,  // 145   
  0,  // 146   
  0,  // 147   
  0,  // 148   
  0,  // 149   
  0,  // 150   
  0,  // 151   
  0,  // 152   
  0,  // 153   
  0,  // 154   
  0,  // 155   
  0,  // 156   
  0,  // 157   
  0,  // 158   
  0,  // 159   
  0,  // 160   
  0,  // 161   
  0,  // 162   
  0,  // 163   
  0,  // 164   
  0,  // 165   
  0,  // 166   
  0,  // 167   
  0,  // 168   
  0,  // 169   
  0,  // 170   
  0,  // 171   
  0,  // 172   
  0,  // 173   
  0,  // 174   
  0,  // 175   
  0,  // 176   
  0,  // 177   
  0,  // 178   
  0,  // 179   
  0,  // 180   
  0,  // 181   
  0,  // 182   
  0,  // 183   
  0,  // 184   
  0,  // 185   
  0,  // 186   
  0,  // 187   
  0,  // 188   
  0,  // 189   
  0,  // 190   
  0,  // 191   
  0,  // 192   
  0,  // 193   
  0,  // 194   
  0,  // 195   
  0,  // 196   
  0,  // 197   
  0,  // 198   
  0,  // 199   
  0,  // 200   
  0,  // 201   
  0,  // 202   
  0,  // 203   
  0,  // 204   
  0,  // 205   
  0,  // 206   
  0,  // 207   
  0,  // 208   
  0,  // 209   
  0,  // 210   
  0,  // 211   
  0,  // 212   
  0,  // 213   
  0,  // 214   
  0,  // 215   
  0,  // 216   
  0,  // 217   
  0,  // 218   
  0,  // 219   
  0,  // 220   
  0,  // 221   
  0,  // 222   
  0,  // 223   
  0,  // 224   
  0,  // 225   
  0,  // 226   
  0,  // 227   
  0,  // 228   
  0,  // 229   
  0,  // 230   
  0,  // 231   
  0,  // 232   
  0,  // 233   
  0,  // 234   
  0,  // 235   
  0,  // 236   
  0,  // 237   
  0,  // 238   
  0,  // 239   
  0,  // 240   
  0,  // 241   
  0,  // 242   
  0,  // 243   
  0,  // 244   
  0,  // 245   
  0,  // 246   
  0,  // 247   
  0,  // 248   
  0,  // 249   
  0,  // 250   
  0,  // 251   
  0,  // 252   
  0,  // 253   
  0,  // 254   
  0,  // 255
};

// Text parameters
#define CHAR_SPACING 1 // ? pixels between characters

// Global message buffers shared by Serial and Scrolling functions - is 5 large enough?? try 6??
#define BUF_SIZE  8
char message[BUF_SIZE] = {"Hi"};

int hours;
int minutes;
char weatherNowChar;
int tempNowInt;

// weather based on https://randomnerdtutorials.com/esp8266-weather-forecaster/
// clock (NTP) code based on https://lastminuteengineers.com/esp8266-ntp-server-date-time-tutorial/

// Replace with your SSID and password details
#include "secrets.h"
const char *pass = WIFI_PASSWORD;
const char *ssid = SSIDW; 
String apiKey = API_KEY; 


const long utcOffsetInSeconds = -5 * 3600; // US Central Time (not daylight savings though??)

WiFiClient client;
// Open Weather Map API server name
const char server[] = "api.openweathermap.org";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int jsonend = 0;
boolean startJson = false;
int status = WL_IDLE_STATUS;

#define JSON_BUFF_DIMENSION 2000

unsigned long lastConnectionTime = 0;     // last time you connected to the server, in milliseconds
const unsigned long postInterval = 10 * 60 * 1000;  // posting interval of 10 minutes  (10L * 1000L; 10 seconds delay for testing)


String text;

void newMessage(void)
{
  static uint8_t  msgIndex = 0;
  MD_MAX72XX::moduleType_t mod;

  mod = MD_MAX72XX::FC16_HW; //MD_MAX72XX::PAROLA_HW;  
  mx.setModuleType(mod);    // change the module type
}


void printText(uint8_t modStart, uint8_t modEnd, char *pMsg)
// Print the text string to the LED matrix modules specified.
// Message area is padded with blank columns after printing.
{
  uint8_t   state = 0;
  uint8_t   curLen;
  uint16_t  showLen;
  uint8_t   cBuf[5]; // was 8
  int16_t   col = ((modEnd + 1) * COL_SIZE) - 1;

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  do     // finite state machine to print the characters in the space available
  {
    switch(state)
    {
      case 0: // Load the next character from the font table
        // if we reached end of message, reset the message pointer
        if (*pMsg == '\0')
        {
          showLen = col - (modEnd * COL_SIZE);  // padding characters
          state = 2;
          break;
        }

        // retrieve the next character form the font file
        showLen = mx.getChar(*pMsg++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
        curLen = 0;
        state++;
        // !! deliberately fall through to next state to start displaying

      case 1: // display the next part of the character
        mx.setColumn(col--, cBuf[curLen++]);

        // done with font character, now display the space between chars
        if (curLen == showLen)
        {
          showLen = CHAR_SPACING;
          state = 2;
        }
        break;

      case 2: // initialize state for displaying empty columns
        curLen = 0;
        state++;
        // fall through

      case 3:  // display inter-character spacing or end of message padding (blank columns)
        mx.setColumn(col--, 0);
        curLen++;
        if (curLen == showLen)
          state = 0;
        break;

      default:
        col = -1;   // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  text.reserve(JSON_BUFF_DIMENSION);
  Serial.println("break 1");
  WiFi.begin(ssid,pass);
  Serial.println("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");
  printWiFiStatus();

  timeClient.begin();
  
  mx.begin();

  mx.setFont(newFont); // new!
 
  mx.control(MD_MAX72XX::INTENSITY, 2); // Use a value between 0 and 15 for brightness 
  
  newMessage();
}

void printMessage() {

  sprintf(minutesRealDisplay, "%d", minutes);   
  if (minutes < 10) {
    sprintf(minutesRealDisplay, "0%d", minutes); 
  } 

  // add time (hours and minutes) and tempNowInt and weatherNowChar to message
  sprintf(message, "%d:%s%d%c",
                            hours,
                            minutesRealDisplay, 
                            tempNowInt,
                            weatherNowChar);
  Serial.println(message);
  printText(0, MAX_DEVICES-1, message); 
  
}

void loop() {

  timeClient.update();

  hours = timeClient.getHours();
  minutes = timeClient.getMinutes();
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.println(" ");

  printMessage();
  
  // put your main code here, to run repeatedly:
  //OWM requires 10 mins between request intervals
  //check if 10 mins has passed then conect again and pull
  if ( (lastConnectionTime == 0) || (millis() - lastConnectionTime > postInterval) ) {
    // note the time that the connection was made:
    lastConnectionTime = millis();
    makehttpRequest();
  }

  delay(15000); // delay 15 seconds
  
}

// print Wifi status
void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// to request data from OWM
void makehttpRequest() {
  // close any connection before send a new request to allow client make connection to server
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting to server..");
    //Serial.println("relURL=/data/2.5/weather?lat=36.623462&lon=-87.3825136&units=imperial&appid=" + apiKey + "&mode=json HTTP/1.1");
    // send the HTTP GET request:
    client.println("GET /data/2.5/weather?lat=36.623462&lon=-87.3825136&units=imperial&appid=" + apiKey + "&mode=json HTTP/1.1");
    //Serial.println("break A");
    client.println("Host: api.openweathermap.org");
    //Serial.println("break B");
    client.println("User-Agent: ArduinoWiFi/1.1");
    //Serial.println("break C");
    client.println("Connection: close");
    //Serial.println("break D");
    client.println();
    //Serial.println("break E");
    
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    Serial.println("Reading from server..");
    
    char c = 0;
    while (client.available()) {
      c = client.read();
      // since json contains equal number of open and close curly brackets, this means we can determine when a json is completely received  by counting
      // the open and close occurences,
      //Serial.print(c);
      if (c == '{') {
        startJson = true;         // set startJson true to indicate json message has started
        jsonend++;
      }
      if (c == '}') {
        jsonend--;
      }
      if (startJson == true) {
        text += c;
      }
      // if jsonend = 0 then we have have received equal number of curly braces 
      if (jsonend == 0 && startJson == true) {
        Serial.println(text);
        parseJson(text.c_str());  // parse c string text in parseJson function
        text = "";                // clear text string for the next time
        startJson = false;        // set startJson to false to indicate that a new message has not yet started
      }
      delay(0);
      
    }
    //parseJson(text.c_str());
    //text = "";
    //startJson = false;
  }
  else {
    // if no connction was made:
    Serial.println("connection failed");
    return;
  }
}

//to parse json data recieved from OWM
void parseJson(const char * jsonString) {
  //StaticJsonBuffer<4000> jsonBuffer;
  const size_t bufferSize = 2*JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(2) + 4*JSON_OBJECT_SIZE(1) + 3*JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 2*JSON_OBJECT_SIZE(7) + 2*JSON_OBJECT_SIZE(8) + 720;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  //Serial.println(" parseJson() ");
  //Serial.println(jsonString);

  // FIND FIELDS IN JSON TREE
  JsonObject& root = jsonBuffer.parseObject(jsonString);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  JsonArray& list = root["main"];
  JsonArray& weath = root["weather"];
  JsonObject& nowT = list[0];
  JsonObject& nowW = weath[0];

  // including temperature and humidity for those who may wish to hack it in
  
  //String city = root["city"]["name"];
  
  String tempNow = root["main"]["temp"];
  String humidityNow = nowT["humidity"];
  String weatherNow = nowW["main"]; // working

  
  Serial.println("Weather Now: "+weatherNow);
  Serial.println(tempNow);

  weatherNowChar = stringToChar(weatherNow); 

  tempNowInt = stringToInt(tempNow);
  
  // USE LED MATRIX INSTEAD

  printMessage();
}

char stringToChar(String s) {
   char arr[10];
   char ret;   

   if (s == "Clear")
     ret = s1; // use "S" for "Sunny" instead
   else if (s == "Snow")
     ret = empty1; // ignore snow
   else if (s == "Drizzle")
     ret = r1; // "Rain"    
   else {
     s.toCharArray(arr, sizeof(arr));

     if (arr[0] == 'C') 
       ret = c1; // cloudy icon
     else if (arr[0] == 'R') 
       ret = r1; // rain icon
     else if (arr[0] == 'T')
       ret = t1; // thunderstorm icon
     else
       ret = empty1;
   }    

   Serial.println(ret);

   return ret;
}

int stringToInt(String s) {
   char arr[8];
   s.toCharArray(arr, sizeof(arr));
   return atoi(arr);
}
