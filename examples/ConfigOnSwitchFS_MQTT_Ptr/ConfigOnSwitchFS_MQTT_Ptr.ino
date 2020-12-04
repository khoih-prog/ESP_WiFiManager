/****************************************************************************************************************************
  ConfigOnSwitchFS_MQTT_Ptr.ino
  For ESP8266 / ESP32 boards
  
  ESP_WiFiManager is a library for the ESP8266/ESP32 platform (https://github.com/esp8266/Arduino) to enable easy
  configuration and reconfiguration of WiFi credentials using a Captive Portal. Inspired by:
  http://www.esp8266.com/viewtopic.php?f=29&t=2520
  https://github.com/chriscook8/esp-arduino-apboot
  https://github.com/esp8266/Arduino/blob/master/libraries/DNSServer/examples/CaptivePortalAdvanced/
  
  Modified from Tzapu https://github.com/tzapu/WiFiManager
  and from Ken Taylor https://github.com/kentaylor
  
  Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager
  Licensed under MIT license
  Version: 1.3.0

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
  1.0.11  K Hoang      17/08/2020 Add CORS feature. Fix bug in softAP, autoConnect, resetSettings.
  1.1.0   K Hoang      28/08/2020 Add MultiWiFi feature to autoconnect to best WiFi at runtime
  1.1.1   K Hoang      30/08/2020 Add setCORSHeader function to allow flexible CORS. Fix typo and minor improvement.
  1.1.2   K Hoang      17/08/2020 Fix bug. Add example.
  1.2.0   K Hoang      09/10/2020 Restore cpp code besides Impl.h code to use if linker error. Fix bug.
  1.3.0   K Hoang      04/12/2020 Add LittleFS support to ESP32 using LITTLEFS Library
 *****************************************************************************************************************************/
/****************************************************************************************************************************
   This example will open a configuration portal when no WiFi configuration has been previously entered or when a button is pushed.
   It is the easiest scenario for configuration but requires a pin and a button on the ESP8266/ESP32 device.
   The Flash button is convenient for this on NodeMCU devices.

   Also in this example a password is required to connect to the configuration portal
   network. This is inconvenient but means that only those who know the password or those
   already connected to the target WiFi network can access the configuration portal and
   the WiFi network credentials will be sent from the browser over an encrypted connection and
   can not be read by observers.

   The Credentials, being input via Config Portal, will then be saved into LittleFS / SPIFFS file,
   and be used to connect to Adafruit MQTT Server at "io.adafruit.com" and publish a Temperature Topic

   Based on original sketch posted by user "wackoo" on https://forum.arduino.cc/index.php?topic=692108
 *****************************************************************************************************************************/

#if !( defined(ESP8266) ||  defined(ESP32) )
  #error This code is intended to run on the ESP8266 or ESP32 platform! Please check your Tools->Board setting.
#endif

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _WIFIMGR_LOGLEVEL_    3

#include <Arduino.h>            // for button
#include <OneButton.h>          // for button

#include <FS.h>

// Now support ArduinoJson 6.0.0+ ( tested with v6.15.2 to v6.16.1 )
#include <ArduinoJson.h>        // get it from https://arduinojson.org/ or install via Arduino library manager

//For ESP32, To use ESP32 Dev Module, QIO, Flash 4MB/80MHz, Upload 921600
//Ported to ESP32
#ifdef ESP32
  #include <esp_wifi.h>
  #include <WiFi.h>
  #include <WiFiClient.h>

  // From v1.1.0
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;

  // LittleFS has higher priority than SPIFFS
  #define USE_LITTLEFS    true
  #define USE_SPIFFS      false

  #if USE_LITTLEFS
    // Use LittleFS
    #include "FS.h"

    // The library will be depreciated after being merged to future major Arduino esp32 core release 2.x
    // At that time, just remove this library inclusion
    #include <LITTLEFS.h>             // https://github.com/lorol/LITTLEFS
    
    FS* filesystem =      &LITTLEFS;
    #define FileFS        LITTLEFS
    #define FS_Name       "LittleFS"
  #elif USE_SPIFFS
    #include <SPIFFS.h>
    FS* filesystem =      &SPIFFS;
    #define FileFS        SPIFFS
    #define FS_Name       "SPIFFS"
  #else
    // Use FFat
    #include <FFat.h>
    FS* filesystem =      &FFat;
    #define FileFS        FFat
    #define FS_Name       "FFat"
  #endif
  //////

  #define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

  #define LED_BUILTIN       2
  #define LED_ON            HIGH
  #define LED_OFF           LOW

#else
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
  //needed for library
  #include <DNSServer.h>
  #include <ESP8266WebServer.h>

  // From v1.1.0
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;

  #define USE_LITTLEFS      true
  
  #if USE_LITTLEFS
    #include <LittleFS.h>
    FS* filesystem = &LittleFS;
    #define FileFS    LittleFS
    #define FS_Name       "LittleFS"
  #else
    FS* filesystem = &SPIFFS;
    #define FileFS    SPIFFS
    #define FS_Name       "SPIFFS"
  #endif
  //////
  
  #define ESP_getChipId()   (ESP.getChipId())
  
  #define LED_ON      LOW
  #define LED_OFF     HIGH
#endif

#include "Adafruit_MQTT.h"                //https://github.com/adafruit/Adafruit_MQTT_Library
#include "Adafruit_MQTT_Client.h"         //https://github.com/adafruit/Adafruit_MQTT_Library

#if ESP32

  //See file .../hardware/espressif/esp32/variants/(esp32|doitESP32devkitV1)/pins_arduino.h
  #define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
  #define PIN_LED           2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
  
  #define PIN_D0            0         // Pin D0 mapped to pin GPIO0/BOOT/ADC11/TOUCH1 of ESP32
  #define PIN_D1            1         // Pin D1 mapped to pin GPIO1/TX0 of ESP32
  #define PIN_D2            2         // Pin D2 mapped to pin GPIO2/ADC12/TOUCH2 of ESP32
  #define PIN_D3            3         // Pin D3 mapped to pin GPIO3/RX0 of ESP32
  #define PIN_D4            4         // Pin D4 mapped to pin GPIO4/ADC10/TOUCH0 of ESP32
  #define PIN_D5            5         // Pin D5 mapped to pin GPIO5/SPISS/VSPI_SS of ESP32
  #define PIN_D6            6         // Pin D6 mapped to pin GPIO6/FLASH_SCK of ESP32
  #define PIN_D7            7         // Pin D7 mapped to pin GPIO7/FLASH_D0 of ESP32
  #define PIN_D8            8         // Pin D8 mapped to pin GPIO8/FLASH_D1 of ESP32
  #define PIN_D9            9         // Pin D9 mapped to pin GPIO9/FLASH_D2 of ESP32
  
  #define PIN_D10           10        // Pin D10 mapped to pin GPIO10/FLASH_D3 of ESP32
  #define PIN_D11           11        // Pin D11 mapped to pin GPIO11/FLASH_CMD of ESP32
  #define PIN_D12           12        // Pin D12 mapped to pin GPIO12/HSPI_MISO/ADC15/TOUCH5/TDI of ESP32
  #define PIN_D13           13        // Pin D13 mapped to pin GPIO13/HSPI_MOSI/ADC14/TOUCH4/TCK of ESP32
  #define PIN_D14           14        // Pin D14 mapped to pin GPIO14/HSPI_SCK/ADC16/TOUCH6/TMS of ESP32
  #define PIN_D15           15        // Pin D15 mapped to pin GPIO15/HSPI_SS/ADC13/TOUCH3/TDO of ESP32
  #define PIN_D16           16        // Pin D16 mapped to pin GPIO16/TX2 of ESP32
  #define PIN_D17           17        // Pin D17 mapped to pin GPIO17/RX2 of ESP32
  #define PIN_D18           18        // Pin D18 mapped to pin GPIO18/VSPI_SCK of ESP32
  #define PIN_D19           19        // Pin D19 mapped to pin GPIO19/VSPI_MISO of ESP32
  
  #define PIN_D21           21        // Pin D21 mapped to pin GPIO21/SDA of ESP32
  #define PIN_D22           22        // Pin D22 mapped to pin GPIO22/SCL of ESP32
  #define PIN_D23           23        // Pin D23 mapped to pin GPIO23/VSPI_MOSI of ESP32
  #define PIN_D24           24        // Pin D24 mapped to pin GPIO24 of ESP32
  #define PIN_D25           25        // Pin D25 mapped to pin GPIO25/ADC18/DAC1 of ESP32
  #define PIN_D26           26        // Pin D26 mapped to pin GPIO26/ADC19/DAC2 of ESP32
  #define PIN_D27           27        // Pin D27 mapped to pin GPIO27/ADC17/TOUCH7 of ESP32
  
  #define PIN_D32           32        // Pin D32 mapped to pin GPIO32/ADC4/TOUCH9 of ESP32
  #define PIN_D33           33        // Pin D33 mapped to pin GPIO33/ADC5/TOUCH8 of ESP32
  #define PIN_D34           34        // Pin D34 mapped to pin GPIO34/ADC6 of ESP32
  
  //Only GPIO pin < 34 can be used as output. Pins >= 34 can be only inputs
  //See .../cores/esp32/esp32-hal-gpio.h/c
  //#define digitalPinIsValid(pin)          ((pin) < 40 && esp32_gpioMux[(pin)].reg)
  //#define digitalPinCanOutput(pin)        ((pin) < 34 && esp32_gpioMux[(pin)].reg)
  //#define digitalPinToRtcPin(pin)         (((pin) < 40)?esp32_gpioMux[(pin)].rtc:-1)
  //#define digitalPinToAnalogChannel(pin)  (((pin) < 40)?esp32_gpioMux[(pin)].adc:-1)
  //#define digitalPinToTouchChannel(pin)   (((pin) < 40)?esp32_gpioMux[(pin)].touch:-1)
  //#define digitalPinToDacChannel(pin)     (((pin) == 25)?0:((pin) == 26)?1:-1)
  
  #define PIN_D35           35        // Pin D35 mapped to pin GPIO35/ADC7 of ESP32
  #define PIN_D36           36        // Pin D36 mapped to pin GPIO36/ADC0/SVP of ESP32
  #define PIN_D39           39        // Pin D39 mapped to pin GPIO39/ADC3/SVN of ESP32
  
  #define PIN_RX0            3        // Pin RX0 mapped to pin GPIO3/RX0 of ESP32
  #define PIN_TX0            1        // Pin TX0 mapped to pin GPIO1/TX0 of ESP32
  
  #define PIN_SCL           22        // Pin SCL mapped to pin GPIO22/SCL of ESP32
  #define PIN_SDA           21        // Pin SDA mapped to pin GPIO21/SDA of ESP32
  
#else

  //PIN_D0 can't be used for PWM/I2C
  #define PIN_D0            16        // Pin D0 mapped to pin GPIO16/USER/WAKE of ESP8266. This pin is also used for Onboard-Blue LED. PIN_D0 = 0 => LED ON
  #define PIN_D1            5         // Pin D1 mapped to pin GPIO5 of ESP8266
  #define PIN_D2            4         // Pin D2 mapped to pin GPIO4 of ESP8266
  #define PIN_D3            0         // Pin D3 mapped to pin GPIO0/FLASH of ESP8266
  #define PIN_D4            2         // Pin D4 mapped to pin GPIO2/TXD1 of ESP8266
  #define PIN_LED           2         // Pin D4 mapped to pin GPIO2/TXD1 of ESP8266, NodeMCU and WeMoS, control on-board LED
  #define PIN_D5            14        // Pin D5 mapped to pin GPIO14/HSCLK of ESP8266
  #define PIN_D6            12        // Pin D6 mapped to pin GPIO12/HMISO of ESP8266
  #define PIN_D7            13        // Pin D7 mapped to pin GPIO13/RXD2/HMOSI of ESP8266
  #define PIN_D8            15        // Pin D8 mapped to pin GPIO15/TXD2/HCS of ESP8266
  
  //Don't use pins GPIO6 to GPIO11 as already connected to flash, etc. Use them can crash the program
  //GPIO9(D11/SD2) and GPIO11 can be used only if flash in DIO mode ( not the default QIO mode)
  #define PIN_D11           9         // Pin D11/SD2 mapped to pin GPIO9/SDD2 of ESP8266
  #define PIN_D12           10        // Pin D12/SD3 mapped to pin GPIO10/SDD3 of ESP8266
  #define PIN_SD2           9         // Pin SD2 mapped to pin GPIO9/SDD2 of ESP8266
  #define PIN_SD3           10        // Pin SD3 mapped to pin GPIO10/SDD3 of ESP8266
  
  #define PIN_D9            3         // Pin D9 /RX mapped to pin GPIO3/RXD0 of ESP8266
  #define PIN_D10           1         // Pin D10/TX mapped to pin GPIO1/TXD0 of ESP8266
  #define PIN_RX            3         // Pin RX mapped to pin GPIO3/RXD0 of ESP8266
  #define PIN_TX            1         // Pin RX mapped to pin GPIO1/TXD0 of ESP8266
  
  #define LED_PIN           16        // Pin D0 mapped to pin GPIO16 of ESP8266. This pin is also used for Onboard-Blue LED. PIN_D0 = 0 => LED ON

#endif    //ESP32

#if ESP32
  const int BUTTON_PIN  = PIN_D27;
  const int RED_LED     = PIN_D26;
  const int BLUE_LED    = PIN_D25;
#else
  const int BUTTON_PIN  = PIN_D1;
  const int RED_LED     = PIN_D2;
  const int BLUE_LED    = PIN_D5;
#endif    //ESP32

uint32_t timer = millis();

const char* CONFIG_FILE = "/ConfigMQTT.json";

// Indicates whether ESP has WiFi credentials saved from previous session
bool initialConfig = true; //default false

// Default configuration values for Adafruit IO MQTT
// This actually works
#define AIO_SERVER              "io.adafruit.com"
#define AIO_SERVERPORT          "1883" //1883, or 8883 for SSL
#define AIO_USERNAME            "private" //Adafruit IO
#define AIO_KEY                 "private"

// Labels for custom parameters in WiFi manager
#define AIO_SERVER_Label             "AIO_SERVER_Label"
#define AIO_SERVERPORT_Label         "AIO_SERVERPORT_Label"
#define AIO_USERNAME_Label           "AIO_USERNAME_Label"
#define AIO_KEY_Label                "AIO_KEY_Label"

// Just dummy topics. To be updated later when got valid data from FS or Config Portal
String MQTT_Pub_Topic   = "private/feeds/Temperature";

// Variables to save custom parameters to...
// I would like to use these instead of #defines
// Variables to save custom parameters to...
// I would like to use these instead of #defines
#define custom_AIO_SERVER_LEN       20
#define custom_AIO_PORT_LEN          5
#define custom_AIO_USERNAME_LEN     20
#define custom_AIO_KEY_LEN          40

char custom_AIO_SERVER[custom_AIO_SERVER_LEN];
char custom_AIO_SERVERPORT[custom_AIO_PORT_LEN];
char custom_AIO_USERNAME[custom_AIO_USERNAME_LEN];
char custom_AIO_KEY[custom_AIO_KEY_LEN];

// Function Prototypes
void MQTT_connect();
bool readConfigFile();
bool writeConfigFile();

// For Config Portal
// SSID and PW for Config Portal
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
const char* password = "your_password";

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

// From v1.1.0
// You only need to format the filesystem once
//#define FORMAT_FILESYSTEM       true
#define FORMAT_FILESYSTEM         false

#define MIN_AP_PASSWORD_SIZE    8

#define SSID_MAX_LEN            32
//From v1.0.10, WPA2 passwords can be up to 63 characters long.
#define PASS_MAX_LEN            64

typedef struct
{
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw  [PASS_MAX_LEN];
}  WiFi_Credentials;

typedef struct
{
  String wifi_ssid;
  String wifi_pw;
}  WiFi_Credentials_String;

#define NUM_WIFI_CREDENTIALS      2

typedef struct
{
  WiFi_Credentials  WiFi_Creds [NUM_WIFI_CREDENTIALS];
} WM_Config;

WM_Config         WM_config;

#define  CONFIG_FILENAME              F("/wifi_cred.dat")
//////

// Indicates whether ESP has WiFi credentials saved from previous session
//bool initialConfig = false;

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

// New in v1.0.11
#define USING_CORS_FEATURE          true
//////

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


//Button config
OneButton btn = OneButton(
                  BUTTON_PIN,  // Input pin for the button
                  true,        // Button is active LOW
                  true         // Enable internal pull-up resistor
                );

// Create an ESP32 WiFiClient class to connect to the MQTT server
WiFiClient *client                    = NULL;

Adafruit_MQTT_Client    *mqtt         = NULL;
Adafruit_MQTT_Publish   *Temperature  = NULL;

uint8_t connectMultiWiFi(void)
{
#if ESP32
  // For ESP32, this better be 0 to shorten the connect time
  #define WIFI_MULTI_1ST_CONNECT_WAITING_MS       0
#else
  // For ESP8266, this better be 2200 to enable connect the 1st time
  #define WIFI_MULTI_1ST_CONNECT_WAITING_MS       2200L
#endif

#define WIFI_MULTI_CONNECT_WAITING_MS           100L
  
  uint8_t status;

  LOGERROR(F("ConnectMultiWiFi with :"));
  
  if ( (Router_SSID != "") && (Router_Pass != "") )
  {
    LOGERROR3(F("* Flash-stored Router_SSID = "), Router_SSID, F(", Router_Pass = "), Router_Pass );
  }

  for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
  {
    // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
    if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
    {
      LOGERROR3(F("* Additional SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
    }
  }
  
  LOGERROR(F("Connecting MultiWifi..."));

  WiFi.mode(WIFI_STA);

#if !USE_DHCP_IP    
  #if USE_CONFIGURABLE_DNS  
    // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
    WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);  
  #else
    // Set static IP, Gateway, Subnetmask, Use auto DNS1 and DNS2.
    WiFi.config(stationIP, gatewayIP, netMask);
  #endif 
#endif

  int i = 0;
  status = wifiMulti.run();
  delay(WIFI_MULTI_1ST_CONNECT_WAITING_MS);

  while ( ( i++ < 10 ) && ( status != WL_CONNECTED ) )
  {
    status = wifiMulti.run();

    if ( status == WL_CONNECTED )
      break;
    else
      delay(WIFI_MULTI_CONNECT_WAITING_MS);
  }

  if ( status == WL_CONNECTED )
  {
    LOGERROR1(F("WiFi connected after time: "), i);
    LOGERROR3(F("SSID:"), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
    LOGERROR3(F("Channel:"), WiFi.channel(), F(",IP address:"), WiFi.localIP() );
  }
  else
    LOGERROR(F("WiFi not connected"));

  return status;
}

void toggleLED()
{
  //toggle state
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void heartBeatPrint(void)
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
    Serial.print("W");        // W means connected to WiFi
  else
    Serial.print("N");        // N means not connected to WiFi

  if (num == 40)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 5 == 0)
  {
    Serial.print(" ");
  }
}

void publishMQTT(void)
{
  float some_number = 25.0 + (float) ( millis() % 100 ) /  100;

  // For debug only
  //Serial.print(F("Published Temp = "));
  //Serial.println(some_number);
    
  MQTT_connect();
  
  if (Temperature->publish(some_number)) 
  {
    //Serial.println(F("Failed to send value to Temperature feed!"));
    Serial.print("T");        // T means publishing OK
  }
  else 
  {
    //Serial.println(F("Value to Temperature feed sucessfully sent!"));
    Serial.print("F");        // F means publishing failure
  }
}

void check_WiFi(void)
{
  if ( (WiFi.status() != WL_CONNECTED) )
  {
    Serial.println("\nWiFi lost. Call connectMultiWiFi in loop");
    connectMultiWiFi();
  }
}

void check_status(void)
{
  static ulong checkstatus_timeout  = 0;
  static ulong LEDstatus_timeout    = 0;
  static ulong checkwifi_timeout    = 0;
  static ulong mqtt_publish_timeout = 0;
  
  ulong current_millis = millis();

#define WIFICHECK_INTERVAL    1000L
#define LED_INTERVAL          2000L
#define HEARTBEAT_INTERVAL    10000L
#define PUBLISH_INTERVAL      60000L

  // Check WiFi every WIFICHECK_INTERVAL (1) seconds.
  if ((current_millis > checkwifi_timeout) || (checkwifi_timeout == 0))
  {
    check_WiFi();
    checkwifi_timeout = current_millis + WIFICHECK_INTERVAL;
  }

  if ((current_millis > LEDstatus_timeout) || (LEDstatus_timeout == 0))
  {
    // Toggle LED at LED_INTERVAL = 2s
    toggleLED();
    LEDstatus_timeout = current_millis + LED_INTERVAL;
  }

  // Print hearbeat every HEARTBEAT_INTERVAL (10) seconds.
  if ((current_millis > checkstatus_timeout) || (checkstatus_timeout == 0))
  { 
    heartBeatPrint();
    checkstatus_timeout = current_millis + HEARTBEAT_INTERVAL;
  }

  // Check every PUBLISH_INTERVAL (60) seconds.
  if ((current_millis > mqtt_publish_timeout) || (mqtt_publish_timeout == 0))
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      publishMQTT();
    }
    
    mqtt_publish_timeout = current_millis + PUBLISH_INTERVAL;
  }
}

void loadConfigData(void)
{
  File file = FileFS.open(CONFIG_FILENAME, "r");
  LOGERROR(F("LoadWiFiCfgFile "));

  if (file)
  {
    file.readBytes((char *) &WM_config, sizeof(WM_config));
    file.close();
    LOGERROR(F("OK"));
  }
  else
  {
    LOGERROR(F("failed"));
  }
}
    
void saveConfigData(void)
{
  File file = FileFS.open(CONFIG_FILENAME, "w");
  LOGERROR(F("SaveWiFiCfgFile "));

  if (file)
  {
    file.write((uint8_t*) &WM_config, sizeof(WM_config));
    file.close();
    LOGERROR(F("OK"));
  }
  else
  {
    LOGERROR(F("failed"));
  }
}

void deleteOldInstances(void)
{
  // Delete previous instances
  if (mqtt)
  {
    delete mqtt;
    mqtt = NULL;
    
    Serial.println("Deleting old MQTT object");
  }

  if (Temperature)
  {
    delete Temperature;
    Temperature = NULL;
    
    Serial.println("Deleting old Temperature object");
  }
}

void createNewInstances(void)
{
  if (!client)
  {
    client = new WiFiClient;
    
    Serial.print(F("\nCreating new WiFi client object : "));
    Serial.println(client? F("OK") : F("failed"));
  }
  
  // Create new instances from new data
  if (!mqtt)
  {
    // Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
    mqtt = new Adafruit_MQTT_Client(client, custom_AIO_SERVER, atoi(custom_AIO_SERVERPORT), custom_AIO_USERNAME, custom_AIO_KEY);
    
    Serial.print(F("Creating new MQTT object : "));
    
    if (mqtt)
    {
      Serial.println(F("OK"));
      Serial.println(String("AIO_SERVER = ")    + custom_AIO_SERVER    + ", AIO_SERVERPORT = "  + custom_AIO_SERVERPORT);
      Serial.println(String("AIO_USERNAME = ")  + custom_AIO_USERNAME  + ", AIO_KEY = "         + custom_AIO_KEY);
    }
    else
      Serial.println(F("Failed"));
  }
  
  if (!Temperature)
  {
    Serial.print(F("Creating new MQTT_Pub_Topic,  Temperature = "));
    Serial.println(MQTT_Pub_Topic);
    
    Temperature = new Adafruit_MQTT_Publish(mqtt, MQTT_Pub_Topic.c_str());
 
    Serial.print(F("Creating new Temperature object : "));
    
    if (Temperature)
    {
      Serial.println(F("OK"));
      Serial.println(String("Temperature MQTT_Pub_Topic = ")  + MQTT_Pub_Topic);
    }
    else
      Serial.println(F("Failed"));
    }
}

//event handler functions for button
static void handleClick() 
{
  Serial.println("Button clicked!");
  wifi_manager();
}

static void handleDoubleClick() 
{
  Serial.println("Button double clicked!");
}

static void handleLongPressStop() 
{
  Serial.println("Button pressed for long time and then released!");
  newConfigData();
}

void wifi_manager()
{
  Serial.println(F("\nConfig Portal requested."));
  digitalWrite(LED_BUILTIN, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

  //Local intialization. Once its business is done, there is no need to keep it around
  ESP_WiFiManager ESP_wifiManager("ConfigOnSwitchFS-MQTT");

  //Check if there is stored WiFi router/password credentials.
  //If not found, device will remain in configuration mode until switched off via webserver.
  Serial.print("Opening Configuration Portal. ");
  
  Router_SSID = ESP_wifiManager.WiFi_SSID();
  Router_Pass = ESP_wifiManager.WiFi_Pass();
  
  // From v1.1.0, Don't permit NULL password
  if ( (Router_SSID != "") && (Router_Pass != "") )
  {
    ESP_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
    Serial.println("Got stored Credentials. Timeout 120s");
  }
  else
    Serial.println("No stored Credentials. No timeout");

  //Local intialization. Once its business is done, there is no need to keep it around

  // Extra parameters to be configured
  // After connecting, parameter.getValue() will get you the configured value
  // Format: <ID> <Placeholder text> <default value> <length> <custom HTML> <label placement>
  // (*** we are not using <custom HTML> and <label placement> ***)

  // AIO_SERVER
  ESP_WMParameter AIO_SERVER_FIELD(AIO_SERVER_Label, "AIO SERVER", custom_AIO_SERVER, custom_AIO_SERVER_LEN /*20*/);

  // AIO_SERVERPORT
  ESP_WMParameter AIO_SERVERPORT_FIELD(AIO_SERVERPORT_Label, "AIO SERVER PORT", custom_AIO_SERVERPORT, custom_AIO_PORT_LEN + 1);

  // AIO_USERNAME
  ESP_WMParameter AIO_USERNAME_FIELD(AIO_USERNAME_Label, "AIO USERNAME", custom_AIO_USERNAME, custom_AIO_USERNAME_LEN /*20*/);

  // AIO_KEY
  ESP_WMParameter AIO_KEY_FIELD(AIO_KEY_Label, "AIO KEY", custom_AIO_KEY, custom_AIO_KEY_LEN /*40*/);

  ESP_wifiManager.addParameter(&AIO_SERVER_FIELD);
  ESP_wifiManager.addParameter(&AIO_SERVERPORT_FIELD);
  ESP_wifiManager.addParameter(&AIO_USERNAME_FIELD);
  ESP_wifiManager.addParameter(&AIO_KEY_FIELD);

  // Sets timeout in seconds until configuration portal gets turned off.
  // If not specified device will remain in configuration mode until
  // switched off via webserver or device is restarted.
  //ESP_wifiManager.setConfigPortalTimeout(120);

  ESP_wifiManager.setMinimumSignalQuality(-1);

  // From v1.0.10 only
  // Set config portal channel, default = 1. Use 0 => random channel from 1-13
  ESP_wifiManager.setConfigPortalChannel(0);
  //////
  
  //set custom ip for portal
  //ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));
  
#if !USE_DHCP_IP    
  #if USE_CONFIGURABLE_DNS
    // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
    ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);
  #else
    // Set static IP, Gateway, Subnetmask, Use auto DNS1 and DNS2.
    ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask);
  #endif 
#endif  

  // New from v1.1.1
#if USING_CORS_FEATURE
  ESP_wifiManager.setCORSHeader("Your Access-Control-Allow-Origin");
#endif

  // Start an access point
  // and goes into a blocking loop awaiting configuration.
  // Once the user leaves the portal with the exit button
  // processing will continue
  if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
  {
    Serial.println("Not connected to WiFi but continuing anyway.");
  }
  else
  {
    // If you get here you have connected to the WiFi
    Serial.println("Connected...yeey :)");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
  }

  // Only clear then save data if CP entered and with new valid Credentials
  // No CP => stored getSSID() = ""
  if ( String(ESP_wifiManager.getSSID(0)) != "" && String(ESP_wifiManager.getSSID(1)) != "" )
  {
    // Stored  for later usage, from v1.1.0, but clear first
    memset(&WM_config, 0, sizeof(WM_config));
    
    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
      String tempSSID = ESP_wifiManager.getSSID(i);
      String tempPW   = ESP_wifiManager.getPW(i);
  
      if (strlen(tempSSID.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1);
  
      if (strlen(tempPW.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1);  
  
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
      {
        LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
    }
  
    saveConfigData();
  }

  // Getting posted form values and overriding local variables parameters
  // Config file is written regardless the connection state
  strcpy(custom_AIO_SERVER, AIO_SERVER_FIELD.getValue());
  strcpy(custom_AIO_SERVERPORT, AIO_SERVERPORT_FIELD.getValue());
  strcpy(custom_AIO_USERNAME, AIO_USERNAME_FIELD.getValue());
  strcpy(custom_AIO_KEY, AIO_KEY_FIELD.getValue());

  // Writing JSON config file to flash for next boot
  writeConfigFile();

  digitalWrite(LED_BUILTIN, LED_OFF); // Turn LED off as we are not in configuration mode.

  deleteOldInstances();

  MQTT_Pub_Topic = String(custom_AIO_USERNAME) + "/feeds/Temperature";
  createNewInstances();
}

bool readConfigFile() 
{
  // this opens the config file in read-mode
  File f = FileFS.open(CONFIG_FILE, "r");

  if (!f)
  {
    Serial.println("Configuration file not found");
    return false;
  }
  else
  {
    // we could open the file
    size_t size = f.size();
    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size + 1]);

    // Read and store file contents in buf
    f.readBytes(buf.get(), size);
    // Closing file
    f.close();
    // Using dynamic JSON buffer which is not the recommended memory model, but anyway
    // See https://github.com/bblanchon/ArduinoJson/wiki/Memory%20model

#if (ARDUINOJSON_VERSION_MAJOR >= 6)

    DynamicJsonDocument json(1024);
    auto deserializeError = deserializeJson(json, buf.get());
    
    if ( deserializeError )
    {
      Serial.println("JSON parseObject() failed");
      return false;
    }
    serializeJson(json, Serial);
    
#else

    DynamicJsonBuffer jsonBuffer;
    // Parse JSON string
    JsonObject& json = jsonBuffer.parseObject(buf.get());
    // Test if parsing succeeds.
    if (!json.success())
    {
      Serial.println("JSON parseObject() failed");
      return false;
    }
    json.printTo(Serial);
    
#endif

    // Parse all config file parameters, override
    // local config variables with parsed values
    if (json.containsKey(AIO_SERVER_Label))
    {
      strcpy(custom_AIO_SERVER, json[AIO_SERVER_Label]);
    }

    if (json.containsKey(AIO_SERVERPORT_Label))
    {
      strcpy(custom_AIO_SERVERPORT, json[AIO_SERVERPORT_Label]);
    }

    if (json.containsKey(AIO_USERNAME_Label))
    {
      strcpy(custom_AIO_USERNAME, json[AIO_USERNAME_Label]);
    }

    if (json.containsKey(AIO_KEY_Label))
    {
      strcpy(custom_AIO_KEY, json[AIO_KEY_Label]);
    }
  }
  
  Serial.println("\nConfig file was successfully parsed");
  return true;
}

bool writeConfigFile() 
{
  Serial.println("Saving config file");

#if (ARDUINOJSON_VERSION_MAJOR >= 6)
  DynamicJsonDocument json(1024);
#else
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
#endif

  // JSONify local configuration parameters
  json[AIO_SERVER_Label]      = custom_AIO_SERVER;
  json[AIO_SERVERPORT_Label]  = custom_AIO_SERVERPORT;
  json[AIO_USERNAME_Label]    = custom_AIO_USERNAME;
  json[AIO_KEY_Label]         = custom_AIO_KEY;

  // Open file for writing
  File f = FileFS.open(CONFIG_FILE, "w");

  if (!f)
  {
    Serial.println("Failed to open config file for writing");
    return false;
  }

#if (ARDUINOJSON_VERSION_MAJOR >= 6)
  serializeJsonPretty(json, Serial);
  // Write data to file and close it
  serializeJson(json, f);
#else
  json.prettyPrintTo(Serial);
  // Write data to file and close it
  json.printTo(f);
#endif

  f.close();

  Serial.println("\nConfig file was successfully saved");
  return true;
}

// this function is just to display newly saved data,
// it is not necessary though, because data is displayed
// after WiFi manager resets ESP32
void newConfigData() 
{
  Serial.println();
  Serial.print(F("custom_AIO_SERVER: ")); 
  Serial.println(custom_AIO_SERVER);
  Serial.print(F("custom_SERVERPORT: ")); 
  Serial.println(custom_AIO_SERVERPORT);
  Serial.print(F("custom_USERNAME_KEY: ")); 
  Serial.println(custom_AIO_USERNAME);
  Serial.print(F("custom_KEY: ")); 
  Serial.println(custom_AIO_KEY);
  Serial.println();
}

void MQTT_connect() 
{
  int8_t ret;

  MQTT_Pub_Topic = String(custom_AIO_USERNAME) + "/feeds/Temperature";

  createNewInstances();

  // Return if already connected
  if (mqtt->connected()) 
  {
    return;
  }

  Serial.println("Connecting to WiFi MQTT (3 attempts)...");

  uint8_t attempt = 3;
  
  while ((ret = mqtt->connect()) != 0) 
  { 
    // connect will return 0 for connected
    Serial.println(mqtt->connectErrorString(ret));
    Serial.println("Another attemtpt to connect to MQTT in 2 seconds...");
    mqtt->disconnect();
    delay(2000);  // wait 2 seconds
    attempt--;
    
    if (attempt == 0) 
    {
      Serial.println("WiFi MQTT connection failed. Continuing with program...");
      return;
    }
  }
  
  Serial.println("WiFi MQTT connection successful!");
}

// Setup function
void setup()
{
  // Put your setup code here, to run once
  Serial.begin(115200);
  while (!Serial);

  Serial.print("\nStarting ConfigOnSwichFS_MQTT_Ptr using " + String(FS_Name));
  Serial.println(" on " + String(ARDUINO_BOARD));
  Serial.println("ESP_WiFiManager Version " + String(ESP_WIFIMANAGER_VERSION));

  btn.attachClick(handleClick);
  btn.attachDoubleClick(handleDoubleClick);
  btn.attachLongPressStop(handleLongPressStop);

  // Initialize the LED digital pin as an output.
  pinMode(BLUE_LED, OUTPUT);

  // Mount the filesystem
  if (FORMAT_FILESYSTEM)
  {
    Serial.println(F("Forced Formatting."));
    FileFS.format();
  }

  // Format FileFS if not yet
#ifdef ESP32
  if (!FileFS.begin(true))
#else
  if (!FileFS.begin())
#endif  
  {
    Serial.print(FS_Name);
    Serial.println(F(" failed! AutoFormatting."));
    
#ifdef ESP8266
    FileFS.format();
#endif
  }

  if (!readConfigFile())
  {
    Serial.println("Failed to read configuration file, using default values");
  }

  unsigned long startedAt = millis();

  //Here starts the WiFi Manager initialization
  //Local intialization. Once its business is done, there is no need to keep it around
  ESP_WiFiManager ESP_wifiManager("ConfigOnSwichFS-MQTT");

  ESP_wifiManager.setDebugOutput(true);

  // Use only to erase stored WiFi Credentials
  //resetSettings();
  //ESP_wifiManager.resetSettings();

  //set custom ip for portal
  //ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));

  ESP_wifiManager.setMinimumSignalQuality(-1);

  // From v1.0.10 only
  // Set config portal channel, default = 1. Use 0 => random channel from 1-13
  ESP_wifiManager.setConfigPortalChannel(0);
  //////

  //set custom ip for portal
  //ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));
    
#if !USE_DHCP_IP    
  #if USE_CONFIGURABLE_DNS
    // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
    ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);
  #else
    // Set static IP, Gateway, Subnetmask, Use auto DNS1 and DNS2.
    ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask);
  #endif 
#endif

  // New from v1.1.1
#if USING_CORS_FEATURE
  ESP_wifiManager.setCORSHeader("Your Access-Control-Allow-Origin");
#endif

  // We can't use WiFi.SSID() in ESP32 as it's only valid after connected.
  // SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
  // Have to create a new function to store in EEPROM/SPIFFS for this purpose
  Router_SSID = ESP_wifiManager.WiFi_SSID();
  Router_Pass = ESP_wifiManager.WiFi_Pass();

  //Remove this line if you do not want to see WiFi password printed
  Serial.println("Stored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);

  // SSID to uppercase
  ssid.toUpperCase();

  if (Router_SSID == "")
  {
    Serial.println("We haven't got any access point credentials, so get them now");

    digitalWrite(LED_BUILTIN, LED_ON); // Turn led on as we are in configuration mode.

    //it starts an access point
    //and goes into a blocking loop awaiting configuration
    if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
      Serial.println("Not connected to WiFi but continuing anyway.");
    else
      Serial.println("WiFi connected...yeey :)");
  }

  // From v1.1.0, Don't permit NULL password
  if ( (Router_SSID != "") && (Router_Pass != "") )
  {
    LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass);
    wifiMulti.addAP(Router_SSID.c_str(), Router_Pass.c_str());
    
    ESP_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
    Serial.println("Got stored Credentials. Timeout 120s for Config Portal");
  }
  else
  {
    Serial.println("Open Config Portal without Timeout: No stored Credentials.");
    
    initialConfig = true;
  }

  if (initialConfig)
  {
    Serial.println("Starting configuration portal.");
    digitalWrite(LED_BUILTIN, LED_ON); // Turn led on as we are in configuration mode.

    //sets timeout in seconds until configuration portal gets turned off.
    //If not specified device will remain in configuration mode until
    //switched off via webserver or device is restarted.
    //ESP_wifiManager.setConfigPortalTimeout(600);

    // Starts an access point
    if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
      Serial.println("Not connected to WiFi but continuing anyway.");
    else
    {
      Serial.println("WiFi connected...yeey :)");
    }

    // Stored  for later usage, from v1.1.0, but clear first
    memset(&WM_config, 0, sizeof(WM_config));
    
    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
      String tempSSID = ESP_wifiManager.getSSID(i);
      String tempPW   = ESP_wifiManager.getPW(i);
  
      if (strlen(tempSSID.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1);

      if (strlen(tempPW.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1);  

      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
      {
        LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
    }

    saveConfigData();
  }

  digitalWrite(LED_BUILTIN, LED_OFF); // Turn led off as we are not in configuration mode.

  startedAt = millis();

  if (!initialConfig)
  {
    // Load stored data, the addAP ready for MultiWiFi reconnection
    loadConfigData();

    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
      {
        LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
    }

    if ( WiFi.status() != WL_CONNECTED ) 
    {
      Serial.println("ConnectMultiWiFi in setup");
     
      connectMultiWiFi();
    }
  }

  Serial.print("After waiting ");
  Serial.print((float) (millis() - startedAt) / 1000L);
  Serial.print(" secs more in setup(), connection result is ");

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("connected. Local IP: ");
    Serial.println(WiFi.localIP());
  }
  else
    Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
}

// Loop function
void loop()
{
  // checking button state all the time
  btn.tick();

  // this is just for checking if we are connected to WiFi
  check_status();
}
