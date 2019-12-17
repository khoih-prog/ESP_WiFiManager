# ESP_WiFiManager

This library is based on, modified, bug-fixed and improved from:

1. [`Tzapu WiFiManager`] (https://github.com/tzapu/WiFiManager)
2. [`Ken Taylor WiFiManager`] (https://github.com/kentaylor/WiFiManager)

to add support to `ESP32` besides `ESP8266`.

This is an `ESP32 / ESP8266` WiFi Connection manager with fallback web configuration portal.
It's using a web configuration portal, served from the `ESP32 / ESP8266`, and operating as an access point.

The configuration portal is captive, so it will present the configuration dialogue regardless of the web address selected, excluding https requests.

This works with 
1. The `ESP8266` Arduino platform with a recent stable release [`ESP8266 Core 2.6.2 or newer`] (https://github.com/esp8266/Arduino)
2. The `ESP32` Arduino platform with a recent stable release [`ESP32 Core 1.0.4 or newer`] (https://github.com/espressif/arduino-esp32)


## How It Works

- The [ConfigOnSwitch](examples/ConfigOnSwitch) example shows how it works and should be used as the basis for a sketch that uses this library.
- The concept of ConfigOnSwitch is that a new `ESP32 / ESP8266` will start a WiFi configuration portal when powered up and save the configuration data in non volatile memory. Thereafter, the configuration portal will only be started again if a button is pushed on the `ESP32 / ESP8266` module.
- Using any WiFi enabled device with a browser (computer, phone, tablet) connect to the newly created Access Point and type in any http address.
- Because of the Captive Portal and the DNS server you will either get a 'Join to network' type of popup or get any domain you try to access redirected to the configuration portal.
- All http web addresses will be redirected to wifi.urremote.com which will be at IP address `192.168.4.1` . This address is also a valid internet address where the user will see advice that they are connected to the wrong network.
- Choose one of the access points scanned, enter password, click save.
- ESP will try to connect. If successful, the IP address on the new network will be displayed in the configuration portal. 
- The configuration portal will now be visible on two networks, these being it's own network and the network to which it has connected.  On it's own network it will have two IP addresses, the original `192.168.4.1` and the same IP address it has on the network to which it connected.
- Selecting "close configuration portal" will shutdown the web server, shutdown the `ESP32 / ESP8266` WiFi network and return control to the following sketch code.

## Quick Start

### Installing
See ["Installing Arduino libraries"](https://www.arduino.cc/en/Guide/Libraries) and use the Importing a .zip Library method or preferably use manual installation as you can checkout the release from github and use that. This makes it easier to keep current with updates. Installing using the Library Manager currently does not work with this version of the library.

####  Compatibility
Github version `1.0.1` currently works with :
1. release `2.6.2` or newer of the [ESP8266 core for Arduino](https://github.com/esp8266/Arduino)
2. release `1.0.4` or newer of the [ESP32 core for Arduino](https://github.com/espressif/arduino-esp32)

- Checkout [library](https://github.com/khoih-prog/ESP_WiFiManager) to your Arduino libraries folder. Must be [https://github.com/khoih-prog/ESP_WiFiManager](https://github.com/khoih-prog/ESP_WiFiManager) version.

### Using
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

- When you want open a config portal initialize library, add
```cpp
ESP_WiFiManager ESP_wifiManager;
```

then call

```cpp
ESP_wifiManager.startConfigPortal()
```

While in AP mode, connect to it then open a browser to the gateway IP, default `192.168.4.1`, configure wifi, save and it should save WiFi connection information in non volatile memory, reboot and connect.


Once WiFi network information is saved in the `ESP32 / ESP8266`, it will try to connect to WiFi every time it is started without requiring any function calls in the sketch.


Also see examples: 
1. [ConfigOnSwitch](examples/ConfigOnSwitch)
2. [ConfigOnSwitchFS](examples/ConfigOnSwitchFS)
3. [ConfigOnStartup](examples/ConfigOnStartup) 
4. [ConfigOnDoubleReset](examples/ConfigOnDoubleReset) 

## Documentation

#### Password protect the configuration Access Point
You can password protect the configuration access point.  Simply add an SSID as the first parameter and the password as a second parameter to `startConfigPortal`. See the above examples.
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

#### Configuration Portal Timeout
If you need to set a timeout so the `ESP32 / ESP8266` doesn't hang waiting to be configured for ever. 
```cpp
ESP_wifiManager.setConfigPortalTimeout(60);
```
which will wait 1 minutes (60 seconds). When the time passes, the startConfigPortal function will return and continue the sketch, 
unless you're accessing the Config Portal. In this case, the `startConfigPortal` function will stay until you save config data or exit 
the Config Portal.


#### On Demand Configuration Portal

Example usage

```cpp
void loop() 
{
  // is configuration portal requested?
  if ((digitalRead(TRIGGER_PIN) == LOW) || (digitalRead(TRIGGER_PIN2) == LOW)) 
  {
    Serial.println("\nConfiguration portal requested.");
    digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.
    
    //Local intialization. Once its business is done, there is no need to keep it around
    ESP_WiFiManager ESP_wifiManager;
    
    //Check if there is stored WiFi router/password credentials.
    //If not found, device will remain in configuration mode until switched off via webserver.
    Serial.print("Opening configuration portal. ");
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

#### Custom Parameters
Many applications need configuration parameters like `MQTT host and port`, [Blynk](http://www.blynk.cc) or [emoncms](http://emoncms.org) tokens, etc. While it is possible to use `ESP_WiFiManager` to collect additional parameters it is better to read these parameters from a web service once `ESP_WiFiManager` has been used to connect to the internet. This makes `ESP_WiFiManager` simple to code and use, parameters can be edited on a regular web server and can be changed remotely after deployment. A web service that can provide these parameters is at [configure.urremote.com](http://configure.urremote.com/).

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
There are various ways in which you can inject custom HTML, CSS or Javascript into the configuration portal.
The options are:
- inject custom head element
You can use this to any html bit to the head of the configuration portal. If you add a `<style>` element, bare in mind it overwrites the included css, not replaces.

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

#### Debug
Debug is enabled by default on Serial. To disable, add before `startConfigPortal()`

```cpp
ESP_wifiManager.setDebugOutput(false);
```

## Troubleshooting
If you get compilation errors, more often than not, you may need to install a newer version of the `ESP32 / ESP8266` core for Arduino.

Sometimes, the library will only work if you update the `ESP32 / ESP8266` core to the latest version because I am using some newly added function.

If you connect to the created configuration Access Point but the configuration portal does not show up, just open a browser and type in the IP of the web portal, by default `192.168.4.1`.


## Releases
#### 1.0.1

- Forked, modified, bug-fixed and improved from these versions of WiFiManager.

See [Tzapu's version](https://github.com/tzapu/WiFiManager) for previous release information.
See [KenTaylor's version]( https://github.com/kentaylor/WiFiManager) for previous release information.


### Contributions and thanks
Forked from [Tzapu](https://github.com/tzapu/WiFiManager) and [KenTaylor's version]( https://github.com/kentaylor/WiFiManager)


