/****************************************************************************************************************************
  AutoConnectWithFSParameters.ino
  For ESP8266 / ESP32 boards
  
  ESP_WiFiManager is a library for the ESP8266/ESP32 platform (https://github.com/esp8266/Arduino) to enable easy
  configuration and reconfiguration of WiFi credentials using a Captive Portal.
  
  Modified from Tzapu https://github.com/tzapu/WiFiManager
  and from Ken Taylor https://github.com/kentaylor
  
  Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager
  Licensed under MIT license
  Version: 1.0.10
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      07/10/2019 Initial coding
  1.0.1   K Hoang      13/12/2019 Fix bug. Add features. Add support for ESP32
  1.0.2   K Hoang      19/12/2019 Fix bug thatkeeps ConfigPortal in endless loop if Portal/Router SSID or Password is NULL.
  1.0.3   K Hoang      05/01/2020 Option not displaying AvailablePages in Info page. Enhance README.md. Modify examples
  1.0.4   K Hoang      07/01/2020 Add RFC952 setHostname feature.
  1.0.5   K Hoang      15/01/2020 Add configurable DNS feature. Thanks to @Amorphous of https://community.blynk.cc
  1.0.6   K Hoang      03/02/2020 Add support for ArduinoJson version 6.0.0+ ( tested with v6.14.1 )
  1.0.7   K Hoang      13/04/2020 Reduce start time, fix SPIFFS bug in examples, update README.md
  1.0.8   K Hoang      10/06/2020 Fix STAstaticIP issue. Restructure code. Add LittleFS support for ESP8266 core 2.7.1+
  1.0.9   K Hoang      29/07/2020 Fix ESP32 STAstaticIP bug. Permit changing from DHCP <-> static IP using Config Portal.
                                  Add, enhance examples (fix MDNS for ESP32)
  1.0.10  K Hoang      08/08/2020 Add more features to Config Portal. Use random WiFi AP channel to avoid conflict.
 *****************************************************************************************************************************/
#if !( defined(ESP8266) ||  defined(ESP32) )
  #error This code is intended to run on the ESP8266 or ESP32 platform! Please check your Tools->Board setting.
#endif

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _WIFIMGR_LOGLEVEL_    3

#include <FS.h>
  
//Ported to ESP32
#ifdef ESP32
  #include "SPIFFS.h"
  #include <esp_wifi.h>
  #include <WiFi.h>
  #include <WiFiClient.h>
  
  #define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())
  
  #define LED_BUILTIN       2
  #define LED_ON            HIGH
  #define LED_OFF           LOW

  #define FileFS            SPIFFS

#else

  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
  //needed for library
  #include <DNSServer.h>
  #include <ESP8266WebServer.h>
  
  #define ESP_getChipId()   (ESP.getChipId())
  
  #define LED_ON            LOW
  #define LED_OFF           HIGH

  #define USE_LITTLEFS      true

  #if USE_LITTLEFS
    #define FileFS          LittleFS
  #else
    #define FileFS          SPIFFS
  #endif

  #include <LittleFS.h>
#endif

// Pin D2 mapped to pin GPIO2/ADC12 of ESP32, or GPIO2/TXD1 of NodeMCU control on-board LED
#define PIN_LED       LED_BUILTIN

// Now support ArduinoJson 6.0.0+ ( tested with v6.14.1 )
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

char configFileName[] = "/config.json";

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

// SSID and PW for Config Portal
String AP_SSID;
String AP_PASS;

// Use false if you don't like to display Available Pages in Information Page of Config Portal
// Comment out or use true to display Available Pages in Information Page of Config Portal
// Must be placed before #include <ESP_WiFiManager.h>
#define USE_AVAILABLE_PAGES     false

// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use false to disable NTP config. Advisable when using Cellphone, Tablet to access Config Portal.
// See Issue 23: On Android phone ConfigPortal is unresponsive (https://github.com/khoih-prog/ESP_WiFiManager/issues/23)
#define USE_ESP_WIFIMANAGER_NTP     false

// Use true to enable CloudFlare NTP service. System can hang if you don't have Internet access while accessing CloudFlare
// See Issue #21: CloudFlare link in the default portal (https://github.com/khoih-prog/ESP_WiFiManager/issues/21)
#define USE_CLOUDFLARE_NTP          false

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
// Force DHCP to be true
#if defined(USE_DHCP_IP)
#undef USE_DHCP_IP
#endif
#define USE_DHCP_IP     true
#else
// You can select DHCP or Static IP here
//#define USE_DHCP_IP     true
#define USE_DHCP_IP     false
#endif

#if ( USE_DHCP_IP || ( defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP ) )
// Use DHCP
#warning Using DHCP IP
IPAddress stationIP   = IPAddress(0, 0, 0, 0);
IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
IPAddress netMask     = IPAddress(255, 255, 255, 0);
#else
// Use static IP
#warning Using static IP
#ifdef ESP32
IPAddress stationIP   = IPAddress(192, 168, 2, 232);
#else
IPAddress stationIP   = IPAddress(192, 168, 2, 186);
#endif

IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
IPAddress netMask     = IPAddress(255, 255, 255, 0);
#endif

#define USE_CONFIGURABLE_DNS      true

IPAddress dns1IP      = gatewayIP;
IPAddress dns2IP      = IPAddress(8, 8, 8, 8);

#include <ESP_WiFiManager.h>              //https://github.com/khoih-prog/ESP_WiFiManager

//define your default values here, if there are different values in configFileName (config.json), they are overwritten.
#define BLYNK_SERVER_LEN                64
#define BLYNK_TOKEN_LEN                 32
#define BLYNK_SERVER_PORT_LEN           6

#define MQTT_SERVER_MAX_LEN             40
#define MQTT_SERVER_PORT_LEN            6

char blynk_server [BLYNK_SERVER_LEN]        = "account.duckdns.org";
char blynk_port   [BLYNK_SERVER_PORT_LEN]   = "8080";
char blynk_token  [BLYNK_TOKEN_LEN]         = "YOUR_BLYNK_TOKEN";

char mqtt_server  [MQTT_SERVER_MAX_LEN];
char mqtt_port    [MQTT_SERVER_PORT_LEN]    = "8080";

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback(void)
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

bool loadFileFSConfigFile(void)
{
  //clean FS, for testing
  //FileFS.format();

  //read configuration from FS json
  Serial.println("Mounting FS...");

  if (FileFS.begin())
  {
    Serial.println("Mounted file system");

    if (FileFS.exists(configFileName))
    {
      //file exists, reading and loading
      Serial.println("Reading config file");
      File configFile = FileFS.open(configFileName, "r");

      if (configFile)
      {
        Serial.print("Opened config file, size = ");
        size_t configFileSize = configFile.size();
        Serial.println(configFileSize);

        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[configFileSize + 1]);

        configFile.readBytes(buf.get(), configFileSize);

        Serial.print("\nJSON parseObject() result : ");

#if (ARDUINOJSON_VERSION_MAJOR >= 6)
        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get(), configFileSize);

        if ( deserializeError )
        {
          Serial.println("failed");
          return false;
        }
        else
        {
          Serial.println("OK");

          if (json["blynk_server"])
            strncpy(blynk_server, json["blynk_server"], sizeof(blynk_server));
         
          if (json["blynk_port"])
            strncpy(blynk_port, json["blynk_port"], sizeof(blynk_port));
 
          if (json["blynk_token"])
            strncpy(blynk_token,  json["blynk_token"], sizeof(blynk_token));

          if (json["mqtt_server"])
            strncpy(mqtt_server, json["mqtt_server"], sizeof(mqtt_server));

          if (json["mqtt_port"])  
            strncpy(mqtt_port,   json["mqtt_port"], sizeof(mqtt_port));
        }

        //serializeJson(json, Serial);
        serializeJsonPretty(json, Serial);
#else
        DynamicJsonBuffer jsonBuffer;
        // Parse JSON string
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        // Test if parsing succeeds.

        if (json.success())
        {
          Serial.println("OK");

          if (json["blynk_server"])
            strncpy(blynk_server, json["blynk_server"], sizeof(blynk_server));
         
          if (json["blynk_port"])
            strncpy(blynk_port, json["blynk_port"], sizeof(blynk_port));
 
          if (json["blynk_token"])
            strncpy(blynk_token,  json["blynk_token"], sizeof(blynk_token));

          if (json["mqtt_server"])
            strncpy(mqtt_server, json["mqtt_server"], sizeof(mqtt_server));

          if (json["mqtt_port"])  
            strncpy(mqtt_port,   json["mqtt_port"], sizeof(mqtt_port));
        }
        else
        {
          Serial.println("failed");
          return false;
        }
        //json.printTo(Serial);
        json.prettyPrintTo(Serial);
#endif

        configFile.close();
      }
    }
  }
  else
  {
    Serial.println("failed to mount FS");
    return false;
  }
  return true;
}

bool saveFileFSConfigFile(void)
{
  Serial.println("Saving config");

#if (ARDUINOJSON_VERSION_MAJOR >= 6)
  DynamicJsonDocument json(1024);
#else
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
#endif

  json["blynk_server"] = blynk_server;
  json["blynk_port"]   = blynk_port;
  json["blynk_token"]  = blynk_token;

  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"]   = mqtt_port;

  File configFile = FileFS.open(configFileName, "w");

  if (!configFile)
  {
    Serial.println("Failed to open config file for writing");
  }

#if (ARDUINOJSON_VERSION_MAJOR >= 6)
  //serializeJson(json, Serial);
  serializeJsonPretty(json, Serial);
  // Write data to file and close it
  serializeJson(json, configFile);
#else
  //json.printTo(Serial);
  json.prettyPrintTo(Serial);
  // Write data to file and close it
  json.printTo(configFile);
#endif

  configFile.close();
  //end save
}

void heartBeatPrint(void)
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
    Serial.print("H");        // H means connected to WiFi
  else
    Serial.print("F");        // F means not connected to WiFi

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(" ");
  }
}

void toggleLED()
{
  //toggle state
  digitalWrite(PIN_LED, !digitalRead(PIN_LED));
}

void check_status()
{
  static ulong checkstatus_timeout  = 0;
  static ulong LEDstatus_timeout    = 0;
  static ulong currentMillis;

#define HEARTBEAT_INTERVAL    10000L
#define LED_INTERVAL          2000L

  currentMillis = millis();

  if ((currentMillis > LEDstatus_timeout) || (LEDstatus_timeout == 0))
  {
    // Toggle LED at LED_INTERVAL = 2s
    toggleLED();
    LEDstatus_timeout = currentMillis + LED_INTERVAL;
  }

  // Print hearbeat every HEARTBEAT_INTERVAL (10) seconds.
  if ((currentMillis > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = currentMillis + HEARTBEAT_INTERVAL;
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("\nStarting AutoConnectWithFSParams on " + String(ARDUINO_BOARD));

  loadFileFSConfigFile();

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  ESP_WMParameter custom_blynk_server("blynk_server", "blynk_server", blynk_server, BLYNK_SERVER_LEN + 1);
  ESP_WMParameter custom_blynk_port  ("blynk_port",   "blynk_port",   blynk_port,   BLYNK_SERVER_PORT_LEN + 1);
  ESP_WMParameter custom_blynk_token ("blynk_token",  "blynk_token",  blynk_token,  BLYNK_TOKEN_LEN + 1 );

  ESP_WMParameter custom_mqtt_server("mqtt_server", "mqtt_server", mqtt_server, MQTT_SERVER_MAX_LEN + 1);
  ESP_WMParameter custom_mqtt_port  ("mqtt_port",   "mqtt_port",   mqtt_port,   MQTT_SERVER_PORT_LEN + 1);

  // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
  //ESP_WiFiManager ESP_wifiManager;
  // Use this to personalize DHCP hostname (RFC952 conformed)
  ESP_WiFiManager ESP_wifiManager("AutoConnect-FSParams");

  //set config save notify callback
  ESP_wifiManager.setSaveConfigCallback(saveConfigCallback);

  //add all your parameters here
  ESP_wifiManager.addParameter(&custom_blynk_server);
  ESP_wifiManager.addParameter(&custom_blynk_port);
  ESP_wifiManager.addParameter(&custom_blynk_token);

  ESP_wifiManager.addParameter(&custom_mqtt_server);
  ESP_wifiManager.addParameter(&custom_mqtt_port);

  //reset settings - for testing
  //ESP_wifiManager.resetSettings();

  ESP_wifiManager.setDebugOutput(true);

  //set minimum quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //ESP_wifiManager.setMinimumSignalQuality();
  ESP_wifiManager.setMinimumSignalQuality(-1);

  // From v1.0.10 only
  // Set config portal channel, default = 1. Use 0 => random channel from 1-13
  ESP_wifiManager.setConfigPortalChannel(0);
  //////

  //set custom ip for portal
  ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));
  
#if !USE_DHCP_IP    
  #if USE_CONFIGURABLE_DNS  
    // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
    ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);  
  #else
    // Set static IP, Gateway, Subnetmask, Use auto DNS1 and DNS2.
    ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask);
  #endif 
#endif  

  // We can't use WiFi.SSID() in ESP32 as it's only valid after connected.
  // SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
  // Have to create a new function to store in EEPROM/SPIFFS/LittleFS for this purpose
  Router_SSID = ESP_wifiManager.WiFi_SSID();
  Router_Pass = ESP_wifiManager.WiFi_Pass();

  //Remove this line if you do not want to see WiFi password printed
  Serial.println("\nStored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);

  if (Router_SSID != "")
  {
    ESP_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
    Serial.println("Got stored Credentials. Timeout 120s");
  }
  else
  {
    Serial.println("No stored Credentials. No timeout");
  }

  String chipID = String(ESP_getChipId(), HEX);
  chipID.toUpperCase();

  // SSID and PW for Config Portal
  AP_SSID = "ESP_" + chipID + "_AutoConnectAP";
  AP_PASS = "MyESP_" + chipID;

  // Get Router SSID and PASS from EEPROM, then open Config portal AP named "ESP_XXXXXX_AutoConnectAP" and PW "MyESP_XXXXXX"
  // 1) If got stored Credentials, Config portal timeout is 60s
  // 2) If no stored Credentials, stay in Config portal until get WiFi Credentials
  if (!ESP_wifiManager.autoConnect(AP_SSID.c_str(), AP_PASS.c_str()))
  {
    Serial.println("failed to connect and hit timeout");

    //reset and try again, or maybe put it to deep sleep
#ifdef ESP8266
    ESP.reset();
#else   //ESP32
    ESP.restart();
#endif
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("WiFi connected");

  //read updated parameters
  strncpy(blynk_server, custom_blynk_server.getValue(), sizeof(blynk_server));
  strncpy(blynk_port,   custom_blynk_port.getValue(),   sizeof(blynk_port));
  strncpy(blynk_token,  custom_blynk_token.getValue(),  sizeof(blynk_token));

  strncpy(mqtt_server, custom_mqtt_server.getValue(), sizeof(mqtt_server));
  strncpy(mqtt_port, custom_mqtt_port.getValue(),     sizeof(mqtt_port));

  //save the custom parameters to FS
  if (shouldSaveConfig)
  {
    saveFileFSConfigFile();
  }

  Serial.print("Local IP = ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  // put your main code here, to run repeatedly:
  check_status();
}
