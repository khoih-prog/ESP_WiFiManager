# ESP_WiFiManager

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_WiFiManager.svg?)](https://www.ardu-badge.com/ESP_WiFiManager)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP_WiFiManager.svg)](https://github.com/khoih-prog/ESP_WiFiManager/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP_WiFiManager/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP_WiFiManager.svg)](http://github.com/khoih-prog/ESP_WiFiManager/issues)

---

### Releases 1.0.10

1. Don't need to reinput already working SSID in Config Port to update other parameters, such as StaticIP.
2. Disable/Enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
3. Change HTTP_XYZ constants to WM_HTTP_XYZ to avoid conflicts with future releases of ESP32 and ESP8266 cores.
4. Add feature to change WiFi AP channel (fixed or random) to avoid conflict in AP-overcrowded environments.
5. Enhance Config Portal GUI and get the item out of the bucket list.
6. Enhance README.md with more instructions and illustrations.

#### Releases 1.0.9

1. Fix ESP32 STAstaticIP bug. 
2. Enable changing from DHCP <-> static IP using Config Portal.
3. Enable NTP configuration from sketch (USE_ESP_WIFIMANAGER_NTP, USE_CLOUDFLARE_NTP). See Issue #21: [CloudFlare link in the default portal](https://github.com/khoih-prog/ESP_WiFiManager/issues/21).
4. Add, enhance examples (fix MDNS for ESP32 examples, add DRD feature).

#### Releases 1.0.8

1. Fix setSTAStaticIPConfig issue. See [Static Station IP doesn't work](https://github.com/khoih-prog/ESP_WiFiManager/issues/17)
2. Add LittleFS support for ESP8266 core 2.7.1+ in examples to replace deprecated SPIFFS.
3. Restructure code.

#### Releases 1.0.7

1. Use `just-in-time` scanWiFiNetworks() to reduce connection time necessary for battery-operated DeepSleep application. Thanks to [CrispinP](https://github.com/CrispinP) for identifying, requesting and testing. See [Starting WiFIManger is very slow (2000ms)](https://github.com/khoih-prog/ESP_WiFiManager/issues/6)
2. Fix bug relating SPIFFS in examples :
 - [ConfigOnSwitchFS](examples/ConfigOnSwitchFS)
 - [ConfigPortalParamsOnSwitch](examples/ConfigPortalParamsOnSwitch)  (now support ArduinoJson 6.0.0+ as well as 5.13.5-)
 - [AutoConnectWithFSParameters](examples/AutoConnectWithFSParameters)
 See [Having issue to read the SPIFF file](https://github.com/khoih-prog/ESP_WiFiManager/issues/14), Thanks to [OttoKlaasen](https://github.com/OttoKlaasen) to report.
3. Fix [README](README.md). See [Accessing manager after connection](https://github.com/khoih-prog/ESP_WiFiManager/issues/15) 

#### Releases 1.0.6

1. Add function getConfigPortalPW()
2. Add 4 new complicated examples compatible with ArduinoJson 6.0.0+ :[AutoConnect](examples/AutoConnect), [AutoConnectWithFeedback](examples/AutoConnectWithFeedback), [AutoConnectWithFeedbackLED](examples/AutoConnectWithFeedbackLED) and [AutoConnectWithFSParameters](examples/AutoConnectWithFSParameters)

---

This library is based on, modified, bug-fixed and improved from:

1. [`Tzapu WiFiManager`](https://github.com/tzapu/WiFiManager)
2. [`Ken Taylor WiFiManager`](https://github.com/kentaylor/WiFiManager)

to add support to `ESP32` besides `ESP8266`.

This is an `ESP32 / ESP8266` WiFi Connection manager with fallback web ConfigPortal.
It's using a web ConfigPortal, served from the `ESP32 / ESP8266`, and operating as an access point.

---

## Prerequisite

 1. [`Arduino IDE 1.8.12+` for Arduino](https://www.arduino.cc/en/Main/Software)
 2. [`ESP8266 Core 2.7.3+`](https://github.com/esp8266/Arduino) for ESP8266-based boards.
 3. [`ESP32 Core 1.0.4+`](https://github.com/espressif/arduino-esp32) for ESP32-based boards

---

## Installation

### Use Arduino Library Manager
The best and easiest way is to use `Arduino Library Manager`. Search for `ESP_WiFiManager`, then select / install the latest version. You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_WiFiManager.svg?)](https://www.ardu-badge.com/ESP_WiFiManager) for more detailed instructions.

### Manual Install

1. Navigate to [ESP_WiFiManager](https://github.com/khoih-prog/ESP_WiFiManager) page.
2. Download the latest release `ESP_WiFiManager-master.zip`.
3. Extract the zip file to `ESP_WiFiManager-master` directory 
4. Copy the whole `ESP_WiFiManager-master` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO:
1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install **ESP_WiFiManager** library by using [Library Manager](https://docs.platformio.org/en/latest/librarymanager/). Search for ***ESP_WiFiManager*** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](examples/platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically.

---

## How It Works

- The [ConfigOnSwitch](examples/ConfigOnSwitch) example shows how it works and should be used as the basis for a sketch that uses this library.
- The concept of ConfigOnSwitch is that a new `ESP32 / ESP8266` will start a WiFi ConfigPortal when powered up and save the configuration data in non volatile memory. Thereafter, the ConfigPortal will only be started again if a button is pushed on the `ESP32 / ESP8266` module.
- Using any WiFi enabled device with a browser (computer, phone, tablet) connect to the newly created Access Point (AP) using configurable SSID and Password (specified in sketch)

```cpp
// SSID and PW for Config Portal
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
const char* password = "your_password";
``` 
then connect WebBrowser to configurable ConfigPortal IP address, default is 192.168.4.1

- Choose one of the access points scanned, enter password, click ***Save***.
- ESP will restart, then try to connect to the WiFi netwotk using STA-only mode, ***without running the ConfigPortal WebServer and WiFi AP***. See [Accessing manager after connection](https://github.com/khoih-prog/ESP_WiFiManager/issues/15).

---
---

### HOWTO Basic configurations

#### 1. Using default for every configurable parameter

- Include in your sketch
```cpp
#ifdef ESP32
  #include <esp_wifi.h>
  #include <WiFi.h>
  #include <WiFiClient.h>

  #define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

  #define LED_ON      HIGH
  #define LED_OFF     LOW
#else
  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
  //needed for library
  #include <DNSServer.h>
  #include <ESP8266WebServer.h>

  #define ESP_getChipId()   (ESP.getChipId())

  #define LED_ON      LOW
  #define LED_OFF     HIGH
#endif

// SSID and PW for Config Portal
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
const char* password = "your_password";

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

#include <ESP_WiFiManager.h>              //https://github.com/khoih-prog/ESP_WiFiManager
```
---

#### 2. Using many configurable parameters

- Include in your sketch

```cpp
#ifdef ESP32
  #include <esp_wifi.h>
  #include <WiFi.h>
  #include <WiFiClient.h>

  #define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

  #define LED_ON      HIGH
  #define LED_OFF     LOW
#else
  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
  //needed for library
  #include <DNSServer.h>
  #include <ESP8266WebServer.h>

  #define ESP_getChipId()   (ESP.getChipId())

  #define LED_ON      LOW
  #define LED_OFF     HIGH
#endif

// SSID and PW for Config Portal
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
const char* password = "your_password";

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

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
```

---

#### 3. Using STA-mode DHCP, but don't like to change to static IP or display in Config Portal

```cpp
// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
#define USE_STATIC_IP_CONFIG_IN_CP          false

```

---

#### 4. Using STA-mode DHCP, but permit to change to static IP and display in Config Portal

```cpp
// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
// Force DHCP to be true
#if defined(USE_DHCP_IP)
#undef USE_DHCP_IP
#endif
#define USE_DHCP_IP     true
#else
// You can select DHCP or Static IP here
#define USE_DHCP_IP     true
#endif

```

---

#### 5. Using STA-mode StaticIP, and be able to change to DHCP IP and display in Config Portal

```cpp
// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
// Force DHCP to be true
#if defined(USE_DHCP_IP)
#undef USE_DHCP_IP
#endif
#define USE_DHCP_IP     true
#else
// You can select DHCP or Static IP here
#define USE_DHCP_IP     false
#endif

```

---

#### 6. Using STA-mode StaticIP and configurable DNS, and be able to change to DHCP IP and display in Config Portal

```cpp
// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
// Force DHCP to be true
#if defined(USE_DHCP_IP)
#undef USE_DHCP_IP
#endif
#define USE_DHCP_IP     true
#else
// You can select DHCP or Static IP here
#define USE_DHCP_IP     false
#endif

#define USE_CONFIGURABLE_DNS      true

IPAddress dns1IP      = gatewayIP;
IPAddress dns2IP      = IPAddress(8, 8, 8, 8);

```

---

#### 7. Using STA-mode StaticIP and auto DNS, and be able to change to DHCP IP and display in Config Portal

```cpp
// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
// Force DHCP to be true
#if defined(USE_DHCP_IP)
#undef USE_DHCP_IP
#endif
#define USE_DHCP_IP     true
#else
// You can select DHCP or Static IP here
#define USE_DHCP_IP     false
#endif

#define USE_CONFIGURABLE_DNS      false

```

---

#### 8. Not using NTP to avoid issue with some WebBrowsers, especially in CellPhone or Tablets.


```cpp
// Use false to disable NTP config. Advisable when using Cellphone, Tablet to access Config Portal.
// See Issue 23: On Android phone ConfigPortal is unresponsive (https://github.com/khoih-prog/ESP_WiFiManager/issues/23)
#define USE_ESP_WIFIMANAGER_NTP     false

```

---

#### 9. Using NTP feature with CloudFlare. System can hang until you have Internet access for CloudFlare.


```cpp
// Use false to disable NTP config. Advisable when using Cellphone, Tablet to access Config Portal.
// See Issue 23: On Android phone ConfigPortal is unresponsive (https://github.com/khoih-prog/ESP_WiFiManager/issues/23)
#define USE_ESP_WIFIMANAGER_NTP     true

// Use true to enable CloudFlare NTP service. System can hang if you don't have Internet access while accessing CloudFlare
// See Issue #21: CloudFlare link in the default portal (https://github.com/khoih-prog/ESP_WiFiManager/issues/21)
#define USE_CLOUDFLARE_NTP          true

```

---

#### 10. Using NTP feature without CloudFlare to avoid system hang if no Internet access for CloudFlare.


```cpp
// Use false to disable NTP config. Advisable when using Cellphone, Tablet to access Config Portal.
// See Issue 23: On Android phone ConfigPortal is unresponsive (https://github.com/khoih-prog/ESP_WiFiManager/issues/23)
#define USE_ESP_WIFIMANAGER_NTP     true

// Use true to enable CloudFlare NTP service. System can hang if you don't have Internet access while accessing CloudFlare
// See Issue #21: CloudFlare link in the default portal (https://github.com/khoih-prog/ESP_WiFiManager/issues/21)
#define USE_CLOUDFLARE_NTP          false

```

---

#### 11. Using random AP-mode channel to avoid conflict


```cpp
// From v1.0.10 only
// Set config portal channel, default = 1. Use 0 => random channel from 1-13
ESP_wifiManager.setConfigPortalChannel(0);
//////
```

#### 12. Using fixed AP-mode channel, for example channel 3


```cpp
// From v1.0.10 only
// Set config portal channel, default = 1. Use 0 => random channel from 1-13
ESP_wifiManager.setConfigPortalChannel(3);
//////
```

#### 11. Setting STA-mode static IP


```cpp
// Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);
```

---
---

### HOWTO Open Config Portal

- When you want to open a config portal, with default DHCP hostname `ESP8266-XXXXXX` or `ESP32-XXXXXX`, just add

```cpp
ESP_WiFiManager ESP_wifiManager;
```
If you'd like to have a personalized hostname 
`(RFC952-conformed,- 24 chars max,- only a..z A..Z 0..9 '-' and no '-' as last char)`

add

```cpp
ESP_WiFiManager ESP_wifiManager("Personalized-HostName");
```

then later call

```cpp
ESP_wifiManager.startConfigPortal()
```

While in AP mode, connect to it using its `SSID` (ESP_XXXXXX) / `Password` ("your_password"), then open a browser to the AP IP, default `192.168.4.1`, configure wifi then save. The WiFi connection information will be saved in non volatile memory. It will then reboot and autoconnect.

You can also change the AP IP by:

```cpp
//set custom ip for portal
ESP_wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
```

and use fixed / dynamic / random AP channel by:

```cpp
// From v1.0.10 only
// Set config portal channel, default = 1. Use 0 => random channel from 1-13
ESP_wifiManager.setConfigPortalChannel(0);
//////
```

Once WiFi network information is saved in the `ESP32 / ESP8266`, it will try to autoconnect to WiFi every time it is started, without requiring any function calls in the sketch.

---

### Examples

 1. [ConfigOnSwitch](examples/ConfigOnSwitch)
 2. [ConfigOnSwitchFS](examples/ConfigOnSwitchFS)
 3. [ConfigOnStartup](examples/ConfigOnStartup) 
 4. [ConfigOnDoubleReset](examples/ConfigOnDoubleReset)  (now support ArduinoJson 6.0.0+ as well as 5.13.5-)
 5. [ConfigPortalParamsOnSwitch](examples/ConfigPortalParamsOnSwitch)  (now support ArduinoJson 6.0.0+ as well as 5.13.5-)
 6. [ESP_FSWebServer](examples/ESP_FSWebServer)
 7. [ESP_FSWebServer_DRD](examples/ESP_FSWebServer_DRD)
 8. [ESP32_FSWebServer](examples/ESP32_FSWebServer)
 9. [ESP32_FSWebServer_DRD](examples/ESP32_FSWebServer_DRD)
10. [AutoConnect](examples/AutoConnect)
11. [AutoConnectWithFeedback](examples/AutoConnectWithFeedback)
12. [AutoConnectWithFeedbackLED](examples/AutoConnectWithFeedbackLED)
13. [AutoConnectWithFSParameters](examples/AutoConnectWithFSParameters)
14. [ConfigOnSwitchFS_MQTT_Ptr](examples/ConfigOnSwitchFS_MQTT_Ptr)

---

## So, how it works?
In `ConfigPortal Mode`, it starts an access point called `ESP_XXXXXX`. Connect to it using the `configurable password` you can define in the code. For example, `your_password` (see examples):

```cpp
// SSID and PW for Config Portal
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
const char* password = "your_password";
```
After you connected, please, go to http://192.168.4.1, you'll see this `Main` page:

<p align="center">
    <img src="https://github.com/khoih-prog/ESP_WiFiManager/blob/master/Images/Main.png">
</p>

Select `Information` to enter the Info page where the board info will be shown (long page)

<p align="center">
    <img src="https://github.com/khoih-prog/ESP_WiFiManager/blob/master/Images/Info.png">
</p>

or short page (default)

<p align="center">
    <img src="https://github.com/khoih-prog/ESP_WiFiManager/blob/master/Images/Info_Short.png">
</p>

Select `Configuration` to enter this page where you can select an AP and specify its WiFi Credentials

<p align="center">
    <img src="https://github.com/khoih-prog/ESP_WiFiManager/blob/master/Images/Configuration.png">
</p>

Enter your credentials, then click ***Save***. The WiFi Credentials will be saved and the board reboots to connect to the selected WiFi AP.

If you're already connected to a listed WiFi AP and don't want to change anything, just select ***Exit Portal*** from the `Main` page to reboot the board and connect to the previously-stored AP. The WiFi Credentials are still intact.

---

## Documentation

#### Password protect the configuration Access Point
You can password protect the ConfigPortal AP.  Simply add an SSID as the first parameter and the password as a second parameter to `startConfigPortal`. See the above examples.
A short password seems to have unpredictable results so use one that's around 8 characters or more in length.
The guidelines are that a wifi password must consist of 8 to 63 ASCII-encoded characters in the range of 32 to 126 (decimal)

```cpp
ESP_wifiManager.startConfigPortal( SSID , password )
```

#### Callbacks

##### Save settings
This gets called when custom parameters have been set **AND** a connection has been established. Use it to set a flag, so when all the configuration finishes, you can save the extra parameters somewhere.

See [ConfigOnSwitchFS Example](examples/ConfigOnSwitchFS).
```cpp
ESP_wifiManager.setSaveConfigCallback(saveConfigCallback);
```
saveConfigCallback declaration and example
```cpp
//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
```

#### ConfigPortal Timeout
If you need to set a timeout so the `ESP32 / ESP8266` doesn't hang waiting to be configured for ever. 
```cpp
ESP_wifiManager.setConfigPortalTimeout(120);
```
which will wait 2 minutes (120 seconds). When the time passes, the startConfigPortal function will return and continue the sketch, 
unless you're accessing the Config Portal. In this case, the `startConfigPortal` function will stay until you save config data or exit 
the Config Portal.


#### On Demand ConfigPortal

Example usage

```cpp
void loop() 
{
  // is ConfigPortal requested?
  if ((digitalRead(TRIGGER_PIN) == LOW) || (digitalRead(TRIGGER_PIN2) == LOW)) 
  {
    Serial.println("\nConfigPortal requested.");
    digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.
    
		//Local intialization. Once its business is done, there is no need to keep it around
		// Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX		
    // ESP_WiFiManager ESP_wifiManager;
		// Use this to personalize DHCP hostname (RFC952 conformed)
    ESP_WiFiManager ESP_wifiManager("Personalized-HostName");
		
	  ESP_wifiManager.setMinimumSignalQuality(-1);
	  
	  //set custom ip for portal
    ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));
    
	  // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
	  ESP_wifiManager.setSTAStaticIPConfig(IPAddress(192,168,2,114), IPAddress(192,168,2,1), IPAddress(255,255,255,0), 
                                        IPAddress(192,168,2,1), IPAddress(8,8,8,8));
		  
    //Check if there is stored WiFi router/password credentials.
    //If not found, device will remain in configuration mode until switched off via webserver.
    Serial.print("Opening ConfigPortal. ");
    Router_SSID = ESP_wifiManager.WiFi_SSID();
    if (Router_SSID != "")
    {
      ESP_wifiManager.setConfigPortalTimeout(60); //If no access point name has been previously entered disable timeout.
      Serial.println("Got stored Credentials. Timeout 60s");
    }
    else
      Serial.println("No stored Credentials. No timeout");
    
    //it starts an access point 
    //and goes into a blocking loop awaiting configuration
    if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password)) 
    {
      Serial.println("Not connected to WiFi but continuing anyway.");
    } 
    else 
    {
      //if you get here you have connected to the WiFi
      Serial.println("connected...yeey :)");
    }
    
    digitalWrite(PIN_LED, LED_OFF); // Turn led off as we are not in configuration mode.
  }
   
  // put your main code here, to run repeatedly
  check_status();

}
```

See  [ConfigOnSwitch](examples/ConfigOnSwitch) example for a more complex version.

---

#### Custom Parameters

Many applications need configuration parameters like `MQTT host and port`, [Blynk](http://www.blynk.cc) or [emoncms](http://emoncms.org) tokens, etc. While it is possible to use `ESP_WiFiManager` to collect additional parameters it is better to read these parameters from a web service once `ESP_WiFiManager` has been used to connect to the internet.

To capture other parameters with `ESP_WiFiManager` is a lot more involved than all the other features and requires adding custom HTML to your form. If you want to do it with `ESP_WiFiManager` see the example [ConfigOnSwitchFS](examples/ConfigOnSwitchFS)

#### Custom IP Configuration
You can set a custom IP for both AP (access point, config mode) and STA (station mode, client mode, normal project state)

##### Custom Access Point IP Configuration
This will set your captive portal to a specific IP should you need/want such a feature. Add the following snippet before `startConfigPortal()`
```cpp
//set custom ip for portal
ESP_wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
```

##### Custom Station (client) Static IP Configuration
This will use the specified IP configuration instead of using DHCP in station mode.
```cpp
ESP_wifiManager.setSTAStaticIPConfig(IPAddress(192,168,0,99), IPAddress(192,168,0,1), IPAddress(255,255,255,0));
```

#### Custom HTML, CSS, Javascript
There are various ways in which you can inject custom HTML, CSS or Javascript into the ConfigPortal.
The options are:
- inject custom head element
You can use this to any html bit to the head of the ConfigPortal. If you add a `<style>` element, bare in mind it overwrites the included css, not replaces.

```cpp
ESP_wifiManager.setCustomHeadElement("<style>html{filter: invert(100%); -webkit-filter: invert(100%);}</style>");
```

- inject a custom bit of html in the configuration form

```cpp
ESP_WMParameter custom_text("<p>This is just a text paragraph</p>");
ESP_wifiManager.addParameter(&custom_text);
```

- inject a custom bit of html in a configuration form element
Just add the bit you want added as the last parameter to the custom parameter constructor.

```cpp
ESP_WMParameter custom_mqtt_server("server", "mqtt server", "iot.eclipse", 40, " readonly");
```

#### Filter Networks
You can filter networks based on signal quality and show/hide duplicate networks.

- If you would like to filter low signal quality networks you can tell WiFiManager to not show networks below an arbitrary quality %;

```cpp
ESP_wifiManager.setMinimumSignalQuality(10);
```
will not show networks under 10% signal quality. If you omit the parameter it defaults to 8%;

- You can also remove or show duplicate networks (default is remove).
Use this function to show (or hide) all networks.

```cpp
ESP_wifiManager.setRemoveDuplicateAPs(false);
```
---

### Example [ConfigOnSwitchFS_MQTT_Ptr](examples/ConfigOnSwitchFS_MQTT_Ptr)

```cpp
#if !( defined(ESP8266) ||  defined(ESP32) )
  #error This code is intended to run on the ESP8266 or ESP32 platform! Please check your Tools->Board setting.
#endif

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _WIFIMGR_LOGLEVEL_    3

#include <Arduino.h>            // for button
#include <OneButton.h>          // for button

#include <FS.h>

// Now support ArduinoJson 6.0.0+ ( tested with v6.15.2 to v6.16.1 ))
#include <ArduinoJson.h>        // get it from https://arduinojson.org/ or install via Arduino library manager

//Ported to ESP32
//For ESP32, To use ESP32 Dev Module, QIO, Flash 4MB/80MHz, Upload 921600
#if ESP32
  #include "SPIFFS.h"
  #include <esp_wifi.h>
  #include <WiFi.h>
  #include <WiFiClient.h>
  
  #define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())
  
  #define LED_ON      HIGH
  #define LED_OFF     LOW

  #define FileFS      SPIFFS
  
#else

  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
  //needed for library
  #include <DNSServer.h>
  #include <ESP8266WebServer.h>
  
  #define ESP_getChipId()   (ESP.getChipId())
  
  #define LED_ON      LOW
  #define LED_OFF     HIGH

  #define USE_LITTLEFS      true

  #if USE_LITTLEFS
    #define FileFS    LittleFS
  #else
    #define FileFS    SPIFFS
  #endif

  #include <LittleFS.h>

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
int some_number = 5;

const char* CONFIG_FILE = "/ConfigMQTT.json";

// Indicates whether ESP has WiFi credentials saved from previous session
bool initialConfig = true; //default false

// Default configuration values for Adafruit IO MQTT
// This actually works
#define AIO_SERVER              "io.adafruit.com"
#define AIO_SERVERPORT          1883 //1883, or 8883 for SSL
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
#define custom_AIO_SERVER_LEN       20
#define custom_AIO_USERNAME_LEN     20
#define custom_AIO_KEY_LEN          40

char custom_AIO_SERVER[custom_AIO_SERVER_LEN];
int  custom_AIO_SERVERPORT;
char custom_AIO_USERNAME[custom_AIO_USERNAME_LEN];
char custom_AIO_KEY[custom_AIO_KEY_LEN];

// Function Prototypes

bool readConfigFile();
bool writeConfigFile();

// For Config Portal
// SSID and PW for Config Portal
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
const char* password = "your_password";

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

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

void check_status()
{
  static ulong checkstatus_timeout = 0;

#define HEARTBEAT_INTERVAL    10000L
  // Print hearbeat every HEARTBEAT_INTERVAL (10) seconds.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      publishMQTT();
    }
    
    heartBeatPrint();
    checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
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

    if (client)
    {
      Serial.println("\nCreating new WiFi client object OK");
    }
    else
      Serial.println("\nCreating new WiFi client object failed");
  }

  // Create new instances from new data
  if (!mqtt)
  {
    // Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
    mqtt = new Adafruit_MQTT_Client(client, custom_AIO_SERVER, custom_AIO_SERVERPORT, custom_AIO_USERNAME, custom_AIO_KEY);

    if (mqtt)
    {
      Serial.println("Creating new MQTT object OK");
      Serial.println(String("AIO_SERVER = ")    + custom_AIO_SERVER    + ", AIO_SERVERPORT = "  + custom_AIO_SERVERPORT);
      Serial.println(String("AIO_USERNAME = ")  + custom_AIO_USERNAME  + ", AIO_KEY = "         + custom_AIO_KEY);
    }
    else
      Serial.println("Creating new MQTT object failed");
  }

  if (!Temperature)
  {
    Temperature = new Adafruit_MQTT_Publish(mqtt, MQTT_Pub_Topic.c_str());
    Serial.println("Creating new MQTT_Pub_Topic,  Temperature = " + MQTT_Pub_Topic);

     if (Temperature)
     {
      Serial.println("Creating new Temperature object OK");
      Serial.println(String("Temperature MQTT_Pub_Topic = ")  + MQTT_Pub_Topic);
      
     }
    else
      Serial.println("Creating new Temperature object failed");
  }
}

// Setup function
void setup()
{
  // Put your setup code here, to run once
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nStarting ConfigOnSwichFS_MQTT_Ptr on " + String(ARDUINO_BOARD));

  btn.attachClick(handleClick);
  btn.attachDoubleClick(handleDoubleClick);
  btn.attachLongPressStop(handleLongPressStop);

  // Initialize the LED digital pin as an output.
  pinMode(BLUE_LED, OUTPUT);

  // Mount the filesystem
  bool result = FileFS.begin();

  #if USE_LITTLEFS
  Serial.print("\nLittleFS opened: ");
  #else
  Serial.print("\nSPIFFS opened: ");
  #endif
  
  Serial.println(result? "OK" : "Failed");

  if (!readConfigFile())
  {
    Serial.println("Failed to read configuration file, using default values");
  }

  unsigned long startedAt = millis();

  //Here starts the WiFi Manager initialization
  //Local intialization. Once its business is done, there is no need to keep it around
  ESP_WiFiManager ESP_wifiManager("ConfigOnSwichFS");

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

  // We can't use WiFi.SSID() in ESP32as it's only valid after connected.
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

    digitalWrite(BLUE_LED, LED_ON); // Turn led on as we are in configuration mode.

    //it starts an access point
    //and goes into a blocking loop awaiting configuration
    if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
      Serial.println("Not connected to WiFi but continuing anyway.");
    else
      Serial.println("WiFi connected...yeey :)");
  }


  digitalWrite(BLUE_LED, LED_OFF); // Turn led off as we are not in configuration mode.

#define WIFI_CONNECT_TIMEOUT        30000L
#define WHILE_LOOP_DELAY            200L
#define WHILE_LOOP_STEPS            (WIFI_CONNECT_TIMEOUT / ( 3 * WHILE_LOOP_DELAY ))

  startedAt = millis();

  while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
  {
    WiFi.mode(WIFI_STA);
    WiFi.persistent (true);

    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(Router_SSID);

    //WiFi.config(stationIP, gatewayIP, netMask);
    //WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);

    WiFi.begin(Router_SSID.c_str(), Router_Pass.c_str());

    int i = 0;
    while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
    {
      delay(WHILE_LOOP_DELAY);
    }
  }

  Serial.print("After waiting ");
  Serial.print((millis() - startedAt) / 1000);
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
  Serial.println("\nConfiguration Portal requested.");
  digitalWrite(BLUE_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

  //Local intialization. Once its business is done, there is no need to keep it around
  ESP_WiFiManager ESP_wifiManager;

  //Check if there is stored WiFi router/password credentials.
  //If not found, device will remain in configuration mode until switched off via webserver.
  Serial.print("Opening Configuration Portal. ");
  Router_SSID = ESP_wifiManager.WiFi_SSID();
  
  if (Router_SSID != "")
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

  // AIO_SERVERPORT (because it is int, it needs to be converted to string)
  char convertedValue[5];
  sprintf(convertedValue, "%d", custom_AIO_SERVERPORT);
  ESP_WMParameter AIO_SERVERPORT_FIELD(AIO_SERVERPORT_Label, "AIO SERVER PORT", convertedValue, 5);

  // AIO_USERNAME
  ESP_WMParameter AIO_USERNAME_FIELD(AIO_USERNAME_Label, "AIO USERNAME", custom_AIO_USERNAME, custom_AIO_USERNAME_LEN /*20*/);

  // AIO_KEY
  ESP_WMParameter AIO_KEY_FIELD(AIO_KEY_Label, "AIO KEY", custom_AIO_KEY, custom_AIO_KEY_LEN /*40*/);

  // Just a quick hint
  ESP_WMParameter hint_text("<small>*If you want to connect to already connected AP, leave SSID and password fields empty</small>");

  // add all parameters here
  // order of adding is important
  ESP_wifiManager.addParameter(&hint_text);
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

  // Getting posted form values and overriding local variables parameters
  // Config file is written regardless the connection state
  strcpy(custom_AIO_SERVER, AIO_SERVER_FIELD.getValue());
  custom_AIO_SERVERPORT = atoi(AIO_SERVERPORT_FIELD.getValue());
  strcpy(custom_AIO_USERNAME, AIO_USERNAME_FIELD.getValue());
  strcpy(custom_AIO_KEY, AIO_KEY_FIELD.getValue());

  // Writing JSON config file to flash for next boot
  writeConfigFile();

  digitalWrite(BLUE_LED, LED_OFF); // Turn LED off as we are not in configuration mode.

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
      custom_AIO_SERVERPORT = json[AIO_SERVERPORT_Label];
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
  json[AIO_SERVER_Label] = custom_AIO_SERVER;
  json[AIO_SERVERPORT_Label] = custom_AIO_SERVERPORT;
  json[AIO_USERNAME_Label] = custom_AIO_USERNAME;
  json[AIO_KEY_Label] = custom_AIO_KEY;

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
  Serial.print("custom_AIO_SERVER: "); Serial.println(custom_AIO_SERVER);
  Serial.print("custom_SERVERPORT: "); Serial.println(custom_AIO_SERVERPORT);
  Serial.print("custom_USERNAME_KEY: "); Serial.println(custom_AIO_USERNAME);
  Serial.print("custom_KEY: "); Serial.println(custom_AIO_KEY);
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

```

---

### Debug Termimal Output Samples

1. This is terminal debug output when running [ConfigOnSwitchFS_MQTT_Ptr](examples/ConfigOnSwitchFS_MQTT_Ptr) on  ***ESP8266 NodeMCU 1.0.***. Config Portal was requested to input and save MQTT Credentials. The boards then connected to Adafruit MQTT Server successfully.

```
Starting ConfigOnSwichFS_MQTT_Ptr on ESP8266_NODEMCU

LittleFS opened: OK
{"AIO_SERVER_Label":"","AIO_SERVERPORT_Label":0,"AIO_USERNAME_Label":"","AIO_KEY_Label":""}
Config file was successfully parsed
[WM] RFC925 Hostname = ConfigOnSwichFS
[WM] setSTAStaticIPConfig for USE_CONFIGURABLE_DNS
Stored: SSID = HueNet1, Pass = password
Connecting to HueNet1
After waiting 6 secs more in setup(), connection result is connected. Local IP: 192.168.2.178
[WM] freeing allocated params!

Creating new WiFi client object OK
Creating new MQTT object OK
AIO_SERVER = , AIO_SERVERPORT = 0
AIO_USERNAME = , AIO_KEY = 
Creating new MQTT_Pub_Topic,  Temperature = /feeds/Temperature
Creating new Temperature object OK
Temperature MQTT_Pub_Topic = /feeds/Temperature
Connecting to WiFi MQTT (3 attempts)...
Connection failed
Another attemtpt to connect to MQTT in 2 seconds...
Connection failed
Another attemtpt to connect to MQTT in 2 seconds...
Connection failed
Another attemtpt to connect to MQTT in 2 seconds...
WiFi MQTT connection failed. Continuing with program...
FW

Button clicked!

Configuration Portal requested.
[WM] RFC925 Hostname = ESP8266-1C0A56
Opening Configuration Portal. Got stored Credentials. Timeout 120s
[WM] Adding parameter 
[WM] Adding parameter AIO_SERVER_Label
[WM] Adding parameter AIO_SERVERPORT_Label
[WM] Adding parameter AIO_USERNAME_Label
[WM] Adding parameter AIO_KEY_Label
[WM] setSTAStaticIPConfig for USE_CONFIGURABLE_DNS
[WM] WiFi.waitForConnectResult Done
[WM] SET AP_STA
[WM] 
Configuring AP SSID = ESP_1C0A56
[WM] AP PWD = your_password
[WM] AP Channel = 1
[WM] AP IP address = 192.168.4.1
[WM] HTTP server started
[WM] ESP_WiFiManager::startConfigPortal : Enter loop
[WM] Connecting to new AP
[WM] Custom STA IP/GW/Subnet
[WM] DNS1 and DNS2 set
[WM] setWifiStaticIP IP = 192.168.2.186
[WM] Already connected. Bailing out.
Connected...yeey :)
Local IP: 192.168.2.186
Saving config file
{
  "AIO_SERVER_Label": "io.adafruit.com",
  "AIO_SERVERPORT_Label": 1883,
  "AIO_USERNAME_Label": "AIO_User",
  "AIO_KEY_Label": "key"
}
Config file was successfully saved
Deleting old MQTT object
Deleting old Temperature object
Creating new MQTT object OK
AIO_SERVER = io.adafruit.com, AIO_SERVERPORT = 1883
AIO_USERNAME = AIO_User, AIO_KEY = key
Creating new MQTT_Pub_Topic,  Temperature = AIO_User/feeds/Temperature
Creating new Temperature object OK
Temperature MQTT_Pub_Topic = AIO_User/feeds/Temperature
[WM] freeing allocated params!
Connecting to WiFi MQTT (3 attempts)...
WiFi MQTT connection successful!
TWTWTWTWTWTW
```
---

2. This is terminal debug output when running [ConfigOnDoubleReset](examples/ConfigOnDoubleReset)  on  ***ESP32 ESP32_DEV.***. Config Portal was requested by DRD to input and save Credentials. The boards then connected to WiFi using new Static IP successfully.

```cpp
Starting ConfigOnDoubleReset on ESP32_DEV
[WM] RFC925 Hostname = ConfigOnDoubleReset
[WM] setSTAStaticIPConfig for USE_CONFIGURABLE_DNS
Stored: SSID = HueNet1, Pass = password
Got stored Credentials. Timeout 120s
SPIFFS Flag read = 0xd0d01234
doubleResetDetected
Saving config file...
Saving config file OK
Open Config Portal without Timeout: Double Reset Detected
Starting configuration portal.
[WM] WiFi.waitForConnectResult Done
[WM] SET AP
[WM] 
Configuring AP SSID = ESP_9ABF498
[WM] AP PWD = your_password
[WM] AP Channel = 11
[WM] AP IP address = 192.168.4.1
[WM] HTTP server started
[WM] ESP_WiFiManager::startConfigPortal : Enter loop
[WM] Connecting to new AP
[WM] Previous settings invalidated
[WM] Custom STA IP/GW/Subnet
[WM] DNS1 and DNS2 set
[WM] setWifiStaticIP IP = 192.168.2.222
[WM] Connect to new WiFi using new IP parameters
[WM] Connected after waiting (s) : 0.60
[WM] Local ip = 192.168.2.222
[WM] Connection result:  WL_CONNECTED
WiFi connected...yeey :)
After waiting 0 secs more in setup(), connection result is connected. Local IP: 192.168.2.222
[WM] freeing allocated params!
HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH
```

---

### Debug
Debug is enabled by default on Serial. To disable, add before `startConfigPortal()`

```cpp
ESP_wifiManager.setDebugOutput(false);
```

You can also change the debugging level from 0 to 4

```cpp
// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _WIFIMGR_LOGLEVEL_    3
```
---

### Troubleshooting
If you get compilation errors, more often than not, you may need to install a newer version of the `ESP32 / ESP8266` core for Arduino.

Sometimes, the library will only work if you update the `ESP32 / ESP8266` core to the latest version because I am using some newly added function.

If you connect to the created configuration Access Point but the ConfigPortal does not show up, just open a browser and type in the IP of the web portal, by default `192.168.4.1`.

---

### Issues ###

Submit issues to: [ESP_WiFiManager issues](https://github.com/khoih-prog/ESP_WiFiManager/issues)

---

### Releases 1.0.10

1. Don't need to reinput already working SSID in Config Port to update other parameters, such as StaticIP.
2. Disable/Enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
3. Change HTTP_XYZ constants to WM_HTTP_XYZ to avoid conflicts with future releases of ESP32 and ESP8266 cores.
4. Add feature to change WiFi AP channel (fixed or random) to avoid conflict in AP-overcrowded environments.
5. Enhance Config Portal GUI and get the item out of the bucket list.
6. Enhance README.md with more instructions and illustrations.

#### Releases 1.0.9

1. Fix ESP32 STAstaticIP bug. 
2. Enable changing from DHCP <-> static IP using Config Portal.
3. Enable NTP configuration from sketch (USE_ESP_WIFIMANAGER_NTP, USE_CLOUDFLARE_NTP). See Issue #21: [CloudFlare link in the default portal](https://github.com/khoih-prog/ESP_WiFiManager/issues/21).
4. Add, enhance examples (fix MDNS for ESP32 examples, add DRD feature).

#### Releases 1.0.8

1. Fix setSTAStaticIPConfig issue. See [Static Station IP doesn't work](https://github.com/khoih-prog/ESP_WiFiManager/issues/17)
2. Add LittleFS support for ESP8266 core 2.7.1+ in examples to replace deprecated SPIFFS.
3. Restructure code.

#### Releases 1.0.7

1. Use `just-in-time` scanWiFiNetworks() to reduce connection time necessary for battery-operated DeepSleep application. Thanks to [CrispinP](https://github.com/CrispinP) for identifying, requesting and testing. See [Starting WiFIManger is very slow (2000ms)](https://github.com/khoih-prog/ESP_WiFiManager/issues/6)
2. Fix bug relating SPIFFS in examples :
 - [ConfigOnSwitchFS](examples/ConfigOnSwitchFS)
 - [ConfigPortalParamsOnSwitch](examples/ConfigPortalParamsOnSwitch)  (now support ArduinoJson 6.0.0+ as well as 5.13.5-)
 - [AutoConnectWithFSParameters](examples/AutoConnectWithFSParameters)
 See [Having issue to read the SPIFF file](https://github.com/khoih-prog/ESP_WiFiManager/issues/14), Thanks to [OttoKlaasen](https://github.com/OttoKlaasen) to report.
 3. Fix [README](README.md). See [Accessing manager after connection](https://github.com/khoih-prog/ESP_WiFiManager/issues/15)
 
#### Releases 1.0.6

- Add function getConfigPortalPW()
- Add 4 new complicated examples compatible with ArduinoJson 6.0.0+ :[AutoConnect](examples/AutoConnect), [AutoConnectWithFeedback](examples/AutoConnectWithFeedback), [AutoConnectWithFeedbackLED](examples/AutoConnectWithFeedbackLED) and [AutoConnectWithFSParameters](examples/AutoConnectWithFSParameters)

#### Releases 1.0.6-beta

- Add NTP data
- Add support to ArduinoJson 6.0.0+ as well as 5.13.5- to examples

#### Releases 1.0.5

- Add option to specify static DNS servers, besides static IP, Gateway and Subnet Mask
- Modify and add examples. Enhance README.md

#### Releases 1.0.4

- Add ESP_WiFiManager setHostname feature
- Modify and add examples. Enhance README.md

### Releases 1.0.3

- Add option not displaying AvailablePages in Info page.
- Delete unnecessary files
- Modify examples, Images and enhance README.md

#### Releases 1.0.2

- Based on, modified, bug-fixed and improved from these versions of WiFiManager.

See [Tzapu's version](https://github.com/tzapu/WiFiManager) for previous release information.
See [KenTaylor's version](https://github.com/kentaylor/WiFiManager) for previous release information.

#### New in v1.0.2

- Fix bug that keeps ConfigPortal in endless loop if Portal/Router SSID or Password is NULL.
- Add example ConfigPortalParamsOnSwitch to enable ConfigPortal credentials to be reconfigurable using ConfigPortal.

---

### Contributions and Thanks

1. Based on and modified from [Tzapu](https://github.com/tzapu/WiFiManager) and [KenTaylor's version]( https://github.com/kentaylor/WiFiManager)
2. Thanks to [Amorphous](https://community.blynk.cc/t/esp-wifimanager-for-esp32-and-esp8266/42257/13) for the static DNS feature and code, included in v1.0.5
3. Thanks to [CrispinP](https://github.com/CrispinP) for idea to add HostName (v1.0.4) and request to reduce the unnecessary waiting time in ESP_WiFiManager constructor (v1.0.6+). See [Starting WiFIManger is very slow (2000ms)](https://github.com/khoih-prog/ESP_WiFiManager/issues/6)
4. Thanks to [OttoKlaasen](https://github.com/OttoKlaasen) for reporting [Having issue to read the SPIFF file](https://github.com/khoih-prog/ESP_WiFiManager/issues/14) bug in examples.
5. Thanks to [Giuseppe](https://github.com/Gius-8) for reporting [Static Station IP doesn't work](https://github.com/khoih-prog/ESP_WiFiManager/issues/17) bug.
6. Thanks to [AlesSt](https://github.com/AlesSt) for reporting [On Android phone ConfigPortal is unresponsive](https://github.com/khoih-prog/ESP_WiFiManager/issues/23) and request an enhancement (***HOWTO disable the static IP inputs on the config page***) leading to [ESP_WiFiManager v1.0.10](https://github.com/khoih-prog/ESP_WiFiManager/releases/tag/v1.0.10).


<table>
  <tr>
    <td align="center"><a href="https://github.com/Tzapu"><img src="https://github.com/Tzapu.png" width="100px;" alt="Tzapu"/><br /><sub><b>⭐️ Tzapu</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/kentaylor"><img src="https://github.com/kentaylor.png" width="100px;" alt="kentaylor"/><br /><sub><b>⭐️ kentaylor</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/Amorphous"><img src="https://github.com/Amorphous.png" width="100px;" alt="Amorphous"/><br /><sub><b>⭐️ Amorphous</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/CrispinP"><img src="https://github.com/CrispinP.png" width="100px;" alt="CrispinP"/><br /><sub><b>CrispinP</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/OttoKlaasen"><img src="https://github.com/OttoKlaasen.png" width="100px;" alt="OttoKlaasen"/><br /><sub><b>OttoKlaasen</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/Giuseppe"><img src="https://github.com/Giuseppe.png" width="100px;" alt="Giuseppe"/><br /><sub><b>Giuseppe</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/AlesSt"><img src="https://github.com/AlesSt.png" width="100px;" alt="AlesSt"/><br /><sub><b>AlesSt</b></sub></a><br /></td>
  </tr> 
</table>

### Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

## Copyright

Copyright 2019- Khoi Hoang


