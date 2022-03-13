# ESP_WiFiManager

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_WiFiManager.svg?)](https://www.ardu-badge.com/ESP_WiFiManager)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP_WiFiManager.svg)](https://github.com/khoih-prog/ESP_WiFiManager/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP_WiFiManager/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP_WiFiManager.svg)](http://github.com/khoih-prog/ESP_WiFiManager/issues)
[![star this repo](https://githubbadges.com/star.svg?user=khoih-prog&repo=ESP_WiFiManager&style=default)](https://github.com/khoih-prog/ESP_WiFiManager)


<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>

---
---

## Table of Contents

* [Important Breaking Change from v1.8.0](#Important-Breaking-Change-from-v180)
  * [For v1.9.0 and up](#For-v190-and-up)
  * [For v1.8.0 only](#For-v180-only)
* [Important Note](#important-note)
  * [Why do we need the new Async ESPAsync_WiFiManager library](#why-do-we-need-the-new-async-espasync_wifimanager-library)
* [Why do we need this ESP_WiFiManager library](#why-do-we-need-this-esp_wifimanager-library)
  * [Features](#features)
  * [Currently supported Boards](#currently-supported-boards)
* [Changelog](changelog.md) 
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [Note for Platform IO using ESP32 LittleFS](#note-for-platform-io-using-esp32-littlefs)
* [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)
* [HOWTO Use analogRead() with ESP32 running WiFi and/or BlueTooth (BT/BLE)](#howto-use-analogread-with-esp32-running-wifi-andor-bluetooth-btble)
  * [1. ESP32 has 2 ADCs, named ADC1 and ADC2](#1--esp32-has-2-adcs-named-adc1-and-adc2)
  * [2. ESP32 ADCs functions](#2-esp32-adcs-functions)
  * [3. ESP32 WiFi uses ADC2 for WiFi functions](#3-esp32-wifi-uses-adc2-for-wifi-functions)
* [How It Works](#how-it-works)
* [HOWTO Basic configurations](#howto-basic-configurations)
  * [1. Using default for every configurable parameter](#1-using-default-for-every-configurable-parameter)
  * [2. Using many configurable parameters](#2-using-many-configurable-parameters)
  * [3. Using STA-mode DHCP, but don't like to change to static IP or display in Config Portal](#3-using-sta-mode-dhcp-but-dont-like-to-change-to-static-ip-or-display-in-config-portal) 
  * [4. Using STA-mode DHCP, but permit to change to static IP and display in Config Portal](#4-using-sta-mode-dhcp-but-permit-to-change-to-static-ip-and-display-in-config-portal)
  * [5. Using STA-mode StaticIP, and be able to change to DHCP IP and display in Config Portal](#5-using-sta-mode-staticip-and-be-able-to-change-to-dhcp-ip-and-display-in-config-portal)
  * [6. Using STA-mode StaticIP and configurable DNS, and be able to change to DHCP IP and display in Config Portal](#6-using-sta-mode-staticip-and-configurable-dns-and-be-able-to-change-to-dhcp-ip-and-display-in-config-portal) 
  * [7. Using STA-mode StaticIP and auto DNS, and be able to change to DHCP IP and display in Config Portal](#7-using-sta-mode-staticip-and-auto-dns-and-be-able-to-change-to-dhcp-ip-and-display-in-config-portal)
  * [8. Not using NTP to avoid issue with some WebBrowsers, especially in CellPhone or Tablets.](#8-not-using-ntp-to-avoid-issue-with-some-webbrowsers-especially-in-cellphone-or-tablets)
  * [9. Using NTP feature with CloudFlare. System can hang until you have Internet access for CloudFlare.](#9-using-ntp-feature-with-cloudflare-system-can-hang-until-you-have-internet-access-for-cloudflare) 
  * [10. Using NTP feature without CloudFlare to avoid system hang if no Internet access for CloudFlare.](#10-using-ntp-feature-without-cloudflare-to-avoid-system-hang-if-no-internet-access-for-cloudflare)
  * [11. Using random AP-mode channel to avoid conflict](#11-using-random-ap-mode-channel-to-avoid-conflict)
  * [12. Using fixed AP-mode channel, for example channel 3](#12-using-fixed-ap-mode-channel-for-example-channel-3) 
  * [13. Setting STA-mode static IP](#13-setting-sta-mode-static-ip)
  * [14. Using AUTOCONNECT_NO_INVALIDATE feature](#14-using-autoconnect_no_invalidate-feature)
  * [15. Using CORS (Cross-Origin Resource Sharing) feature](#15-using-cors-cross-origin-resource-sharing-feature) 
  * [16. Using MultiWiFi auto(Re)connect feature](#16-using-multiwifi-autoreconnect-feature)
  * [17. How to auto getting _timezoneName](#17-how-to-auto-getting-_timezonename)
  * [18. How to get TZ variable to configure Timezone](#18-how-to-get-tz-variable-to-configure-timezone) 
  * [19. How to use the TZ variable to configure Timezone](#19-how-to-use-the-tz-variable-to-configure-timezone)
* [HOWTO Open Config Portal](#howto-open-config-portal)
* [HOWTO Add Dynamic Parameters](#howto-add-dynamic-parameters) 
  * [1. Determine the variables to be configured via Config Portal (CP)](#1-determine-the-variables-to-be-configured-via-config-portal-cp)
  * [2. Initialize the variables to prepare for Config Portal (CP)](#2-initialize-the-variables-to-prepare-for-config-portal-cp)
    * [2.1 Use the following simple constructor for simple variables such as `thingspeakApiKey`, `pinSda` and `pinScl`](#21-use-the-following-simple-constructor-for-simple-variables-such-as-thingspeakapikey-pinsda-and-pinscl-) 
    * [2.2 For example, to create a new `ESP_WMParameter` object `p_thingspeakApiKey` for `thingspeakApiKey`](#22-for-example-to-create-a-new-esp_wmparameter-object-p_thingspeakapikey-for-thingspeakapikey)
    * [2.3 Use the more complex following constructor for variables such as `sensorDht22`](#23-use-the-more-complex-following-constructor-for-variables-such-as-sensordht22)
    * [2.4 For example, to create a new `ESP_WMParameter` object `p_sensorDht22` for `sensorDht22`](#24-for-example-to-create-a-new-esp_wmparameter-object-p_sensordht22-for-sensordht22)
  * [3. Add the variables to Config Portal (CP)](#3-add-the-variables-to-config-portal-cp) 
    * [3.1 addParameter() function Prototype:](#31-addparameter-function-prototype)
    * [3.2 Code to add variables to CP](#32-code-to-add-variables-to-cp)
  * [4. Save the variables configured in Config Portal (CP)](#4-save-the-variables-configured-in-config-portal-cp) 
    * [4.1 Getting variables' data from CP](#41-getting-variables-data-from-cp)
  * [5. Write to FS (SPIFFS, LittleFS, etc.) using JSON format](#5-write-to-fs-spiffs-littlefs-etc-using-json-format)
    * [5.1 Create a DynamicJsonDocument Object](#51-create-a-dynamicjsondocument-object) 
    * [5.2 Fill the DynamicJsonDocument Object with data got from Config Portal](#52-fill-the-dynamicjsondocument-object-with-data-got-from-config-portal)
    * [5.3 Open file to write the Jsonified data](#53-open-file-to-write-the-jsonified-data)
    * [5.4 Write the Jsonified data to CONFIG_FILE](#54-write-the-jsonified-data-to-config_file) 
    * [5.5 Close CONFIG_FILE to flush and save the data](#55-close-config_file-to-flush-and-save-the-data)
  * [6. Read from FS using JSON format](#6-read-from-fs-using-json-format) 
    * [6.1 Open CONFIG_FILE to read](#61-open-config_file-to-read) 
    * [6.2 Open CONFIG_FILE to read](62-open-config_file-to-read)
    * [6.3 Populate the just-read Jsonified data into the DynamicJsonDocument json object](#63-populate-the-just-read-jsonified-data-into-the-dynamicjsondocument-json-object)
    * [6.4 Parse the Jsonified data from the DynamicJsonDocument json object to store into corresponding parameters](#64-parse-the-jsonified-data-from-the-dynamicjsondocument-json-object-to-store-into-corresponding-parameters) 
    * [6.5 Then what to do now](#65-then-what-to-do-now)
* [So, how it works?](#so-how-it-works)
* [Documentation](#documentation)
  * [Password protect the configuration Access Point](#password-protect-the-configuration-access-point)
  * [Callbacks](#callbacks)
    * [Save settings](#save-settings) 
  * [ConfigPortal Timeout](#configportal-timeout)
  * [On Demand ConfigPortal](#on-demand-configportal)
  * [Custom Parameters](#custom-parameters)
  * [Custom IP Configuration](#custom-ip-configuration) 
    * [Custom Access Point IP Configuration](#custom-access-point-ip-configuration)
    * [Custom Station (client) Static IP Configuration](#custom-station-client-static-ip-configuration)
  * [Custom HTML, CSS, Javascript](#custom-html-css-javascript) 
  * [Filter Networks](#filter-networks)
* [Examples](#examples)
  * [Medium Complexity](#medium-complexity)
    * [AutoConnect](examples/AutoConnect)
    * [AutoConnectWithFeedback](examples/AutoConnectWithFeedback)
    * [AutoConnectWithFeedbackLED](examples/AutoConnectWithFeedbackLED)
    * [AutoConnectWithFSParameters](examples/AutoConnectWithFSParameters)
    * [ConfigOnDoubleReset](examples/ConfigOnDoubleReset)
    * [ConfigOnDRD_FS_MQTT_Ptr](examples/ConfigOnDRD_FS_MQTT_Ptr)
    * [ConfigOnStartup](examples/ConfigOnStartup) 
    * [ConfigOnSwitch](examples/ConfigOnSwitch)
    * [ConfigOnSwitchFS](examples/ConfigOnSwitchFS)
    * [ConfigOnSwitchFS_MQTT_Ptr](examples/ConfigOnSwitchFS_MQTT_Ptr)
    * [ConfigPortalParamsOnSwitch](examples/ConfigPortalParamsOnSwitch)
    * [ESP32_FSWebServer](examples/ESP32_FSWebServer)
    * [ESP32_FSWebServer_DRD](examples/ESP32_FSWebServer_DRD)
    * [ESP_FSWebServer](examples/ESP_FSWebServer)
    * [ESP_FSWebServer_DRD](examples/ESP_FSWebServer_DRD)
  * [High Complexity](#high-complexity)
    * [ConfigOnDRD_FS_MQTT_Ptr_Complex](examples/ConfigOnDRD_FS_MQTT_Ptr_Complex)
    * [ConfigOnDRD_FS_MQTT_Ptr_Medium](examples/ConfigOnDRD_FS_MQTT_Ptr_Medium)
  * [Multiple-Definitions-Linker-Error demo](#Multiple-Definitions-Linker-Error-demo)
    * [ConfigOnDoubleReset_Multi](examples/ConfigOnDoubleReset_Multi)
* [Example ConfigOnDRD_FS_MQTT_Ptr](#example-configondrd_fs_mqtt_ptr)
* [Debug Terminal Output Samples](#debug-termimal-output-samples)
  * [1. ConfigOnSwitchFS_MQTT_Ptr on ESP8266_NODEMCU_ESP12E](#1-configonswitchfs_mqtt_ptr-on-esp8266_nodemcu_esp12e)
  * [2. ESP32_FSWebServer_DRD on ESP32_DEV](#2-esp32_fswebserver_drd-on-esp32_dev)
  * [3. ESP32_FSWebServer_DRD on ESP32_DEV using newly-supported LittleFS](#3-esp32_fswebserver_drd-on-esp32_dev-using-newly-supported-littlefs)
  * [4. ConfigOnDRD_FS_MQTT_Ptr_Complex on ESP32_DEV](#4-configondrd_fs_mqtt_ptr_complex-on-esp32_dev)
    * [4.1 With Config Data => Run normally](#41-with-config-data--run-normally)
    * [4.2 DRD => Config Portal](#42-drd--config-portal)
    * [4.3 Config Portal Done](#43-config-portal-done)
  * [5. ConfigOnDRD_FS_MQTT_Ptr_Medium on ESP8266_NODEMCU_ESP12E](#5-configondrd_fs_mqtt_ptr_medium-on-esp8266_nodemcu_esp12e)
    * [5.1 With Config Data => Run normally](#51-with-config-data--run-normally)
    * [5.2 DRD => Config Portal](#52-drd--config-portal)
    * [5.3 Config Portal Done](#53-config-portal-done)
  * [6. ConfigOnDoubleReset on ESP32S2_DEV](#6-configondoublereset-on-esp32s2_dev)
  * [7. ConfigOnDoubleReset on ESP32_DEV](#7-configondoublereset-on-esp32_dev)
    * [7.1 DRD => Config Portal](#71-drd--config-portal)
    * [7.2 Data Saved => Connect to WiFi with correct local time, TZ set and using NTP](#72-data-saved--connect-to-wifi-with-correct-local-time-tz-set-and-using-ntp)
    * [7.3 Normal running with correct local time, TZ set and using NTP](#73-normal-running-with-correct-local-time-tz-set-and-using-ntp)
  * [8. ConfigOnDoubleReset on ESP32S2_DEV](#8-configondoublereset-on-esp32s2_dev)
    * [8.1 No Data => Config Portal](#81-no-data--config-portal)
    * [8.2 Data Saved => Connect to WiFi with correct local time, TZ set and using NTP](#82-data-saved--connect-to-wifi-with-correct-local-time-tz-set-and-using-ntp)
    * [8.3 Normal running with correct local time, TZ set and using NTP](#83-normal-running-with-correct-local-time-tz-set-and-using-ntp)
  * [9. ESP32_FSWebServer_DRD on ESP32C3_DEV using SPIFFS](#9-esp32_fswebserver_drd-on-esp32c3_dev-using-spiffs)
  * [10. ConfigOnDoubleReset on ESP32S3_DEV](#10-configondoublereset-on-esp32s3_dev)
  * [11. ConfigOnDoubleReset using LittleFS on ESP32C3_DEV](#11-configondoublereset-using-LittleFS-on-ESP32C3_DEV)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License and credits](#license-and-credits)
* [Copyright](#copyright)

---
---

### Important Breaking Change from v1.8.0

#### For v1.9.0 and up

Please have a look at [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)

From v1.9.0, you just use

```
#include <ESP_WiFiManager.h>               //https://github.com/khoih-prog/ESP_WiFiManager
```

instead of both

```
#include <ESP_WiFiManager.h>               //https://github.com/khoih-prog/ESP_WiFiManager

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include <ESP_WiFiManager-Impl.h>          //https://github.com/khoih-prog/ESP_WiFiManager
```


For complex project having `Multiple Definitions Linker Error` issue, you can use in many files (**Be careful**: `.hpp`, not `.h`)

```
#include <ESP_WiFiManager.hpp>             //https://github.com/khoih-prog/ESP_WiFiManager
```

but only in main(), .ino with setup() to avoid `Multiple Definitions Linker Error`


```
#include <ESP_WiFiManager.h>               //https://github.com/khoih-prog/ESP_WiFiManager
```

---


#### For v1.8.0 only

It's advisable to use v1.9.0+

Please have a look at [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)

From v1.8.0, you must use

```
#include <ESP_WiFiManager.h>               //https://github.com/khoih-prog/ESP_WiFiManager

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include <ESP_WiFiManager-Impl.h>          //https://github.com/khoih-prog/ESP_WiFiManager
```

instead of only

```
#include <ESP_WiFiManager.h>               //https://github.com/khoih-prog/ESP_WiFiManager
```

---
---

### Important Note

This [**ESP_WiFiManager**](https://github.com/khoih-prog/ESP_WiFiManager) has just been modified to create the new [**ESPAsync_WiFiManager Library**](https://github.com/khoih-prog/ESPAsync_WiFiManager) in order to use the better and more efficient [**ESPAsyncWebServer Library**](https://github.com/me-no-dev/ESPAsyncWebServer), instead of the (ESP8266)WebServer library.

The new [**ESPAsync_WiFiManager** library](https://github.com/khoih-prog/ESPAsync_WiFiManager) is based on and sync'ed with [**ESP_WiFiManager**](https://github.com/khoih-prog/ESP_WiFiManager). Therefore, all the features currently supported by this [**ESP_WiFiManager**](https://github.com/khoih-prog/ESP_WiFiManager) will be available to the new library. The code change is to port to the new library is also very negligible, **mostly just changing the Class name from ESP_WiFiManager to ESPAsync_WiFiManager.**

### Why do we need the new Async [ESPAsync_WiFiManager library](https://github.com/khoih-prog/ESPAsync_WiFiManager)

- Using asynchronous network means that you can handle **more than one connection at the same time**
- **You are called once the request is ready and parsed**
- When you send the response, you are **immediately ready** to handle other connections while the server is taking care of sending the response in the background
- **Speed is OMG**
- **Easy to use API, HTTP Basic and Digest MD5 Authentication (default), ChunkedResponse**
- Easily extendible to handle **any type of content**
- Supports Continue 100
- **Async WebSocket plugin offering different locations without extra servers or ports**
- Async EventSource (Server-Sent Events) plugin to send events to the browser
- URL Rewrite plugin for conditional and permanent url rewrites
- ServeStatic plugin that supports cache, Last-Modified, default index and more
- Simple template processing engine to handle templates

To appreciate the power of the [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) and underlying Async libraries, please compare the more efficient [Async_ESP32_FSWebServer example](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ESP32_FSWebServer) example code against the code of its complicated twin [ESP32_FSWebServer](examples/ESP32_FSWebServer).

Thanks to [cancodr](https://github.com/cancodr) for requesting an enhancement in [Issue #29: Is it possible to use AsyncWebServer.h instead of WebServer.h?](https://github.com/khoih-prog/ESP_WiFiManager/issues/29), leading to the new [ESPAsync_WiFiManager Library](https://github.com/khoih-prog/ESPAsync_WiFiManager).

---
---

### Why do we need this [ESP_WiFiManager library](https://github.com/khoih-prog/ESP_WiFiManager)

#### Features

This is a WiFiManager Library for configuring/auto(re)connecting **ESP8266/ESP32** modules to the best or available MultiWiFi APs at runtime. Configuration data to be saved in either LittleFS, SPIFFS or EEPROM. Default Credentials as well as Dynamic custom parameters can be added and modified easily. DoubleResetDetector is used to force Config Portal opening even if the Credentials are still valid.
 
This library is designed to help you to eliminate `hardcoding` your Wifi credentials for ESP8266 and ESP32, and updating/reflashing every time you need to change them.

With version `v1.3.0` or later, you can use:

1. `LittleFS for ESP32`

With version `v1.1.0` or later, you can configure:

1. `Multiple WiFi Credentials (SSID, Password) and system will autoconnect to the best and available WiFi SSID.`


#### Currently supported Boards

This [**ESP_WiFiManager** library](https://github.com/khoih-prog/ESP_WiFiManager) currently supports these following boards:

 1. **ESP8266 and ESP32-based boards using EEPROM, SPIFFS or LittleFS**.
 2. **ESP32-S2 (ESP32-S2 Saola, AI-Thinker ESP-12K, etc.) using EEPROM, SPIFFS or LittleFS**.
 3. **ESP32-C3 (ARDUINO_ESP32C3_DEV) using EEPROM, SPIFFS or LittleFS**.
 4. **ESP32-S3 (ESP32S3_DEV, ESP32_S3_BOX, UM TINYS3, UM PROS3, UM FEATHERS3, etc.) using EEPROM, SPIFFS or LittleFS**.
 
---

This library is based on, modified, bug-fixed and improved from:

1. [`Tzapu WiFiManager`](https://github.com/tzapu/WiFiManager)
2. [`Ken Taylor WiFiManager`](https://github.com/kentaylor/WiFiManager)

to add support to `ESP32` besides `ESP8266`.

This is an `ESP32 / ESP8266` WiFi Connection manager with fallback web ConfigPortal.
It's using a web ConfigPortal, served from the `ESP32 / ESP8266`, and operating as an access point.


---
---

## Prerequisites

 1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
 2. [`ESP8266 Core 3.0.2+`](https://github.com/esp8266/Arduino) for ESP8266-based boards. [![Latest release](https://img.shields.io/github/release/esp8266/Arduino.svg)](https://github.com/esp8266/Arduino/releases/latest/). To use ESP8266 core 2.7.1+ for LittleFS.
 3. [`ESP32 Core 2.0.2+`](https://github.com/espressif/arduino-esp32) for ESP32-based boards. [![Latest release](https://img.shields.io/github/release/espressif/arduino-esp32.svg)](https://github.com/espressif/arduino-esp32/releases/latest/)
 4. [`ESP_DoubleResetDetector v1.3.1+`](https://github.com/khoih-prog/ESP_DoubleResetDetector) if using DRD feature. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_DoubleResetDetector.svg?)](https://www.ardu-badge.com/ESP_DoubleResetDetector). Use v1.1.0+ if using LittleFS for ESP32.
 5. [`ESP_MultiResetDetector v1.3.1+`](https://github.com/khoih-prog/ESP_MultiResetDetector) if using MRD feature. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_MultiResetDetector.svg?)](https://www.ardu-badge.com/ESP_MultiResetDetector).
 6. [`LittleFS_esp32 v1.0.6+`](https://github.com/lorol/LITTLEFS) for ESP32-based boards using LittleFS with ESP32 core v1.0.5-. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/LittleFS_esp32.svg?)](https://www.ardu-badge.com/LittleFS_esp32). **Notice**: This [`LittleFS_esp32 library`](https://github.com/lorol/LITTLEFS) has been integrated to Arduino [ESP32 core v1.0.6+](https://github.com/espressif/arduino-esp32/tree/master/libraries/LITTLEFS) and you don't need to install it if using ESP32 core v1.0.6+

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
3. Install [**Latest ESP_WiFiManager** library](https://registry.platformio.org/libraries/khoih-prog/ESP_WiFiManager) by using [Library Manager](https://registry.platformio.org/libraries/khoih-prog/ESP_WiFiManager/installation). Search for **ESP_WiFiManager** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---
---


### Note for Platform IO using ESP32 LittleFS

#### Necessary only for esp32 core v1.0.6-

From esp32 core v1.0.6+, [`LittleFS_esp32 v1.0.6`](https://github.com/lorol/LITTLEFS) has been included and this step is not necessary anymore.

In Platform IO, to fix the error when using [`LittleFS_esp32 v1.0`](https://github.com/lorol/LITTLEFS) for ESP32-based boards with ESP32 core v1.0.4- (ESP-IDF v3.2-), uncomment the following line

from

```
//#define CONFIG_LITTLEFS_FOR_IDF_3_2   /* For old IDF - like in release 1.0.4 */
```

to

```
#define CONFIG_LITTLEFS_FOR_IDF_3_2   /* For old IDF - like in release 1.0.4 */
```

It's advisable to use the latest [`LittleFS_esp32 v1.0.5+`](https://github.com/lorol/LITTLEFS) to avoid the issue.

Thanks to [Roshan](https://github.com/solroshan) to report the issue in [Error esp_littlefs.c 'utime_p'](https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/28) 

---
---

### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can use

```
#include <ESP_WiFiManager.hpp>               //https://github.com/khoih-prog/ESP_WiFiManager
```

in many files. But be sure to use the following `#include <ESP_WiFiManager.h>` **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include <ESP_WiFiManager.h>                //https://github.com/khoih-prog/ESP_WiFiManager
```

Check [ConfigOnDoubleReset_Multi](examples/ConfigOnDoubleReset_Multi) for an example how and where to do so.

Have a look at the discussion in [Different behaviour using the src_cpp or src_h lib #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/discussions/80)


---
---

### HOWTO Use analogRead() with ESP32 running WiFi and/or BlueTooth (BT/BLE)

Please have a look at [**ESP_WiFiManager Issue 39: Not able to read analog port when using the autoconnect example**](https://github.com/khoih-prog/ESP_WiFiManager/issues/39) to have more detailed description and solution of the issue.

#### 1.  ESP32 has 2 ADCs, named ADC1 and ADC2

#### 2. ESP32 ADCs functions

- ADC1 controls ADC function for pins **GPIO32-GPIO39**
- ADC2 controls ADC function for pins **GPIO0, 2, 4, 12-15, 25-27**

#### 3.. ESP32 WiFi uses ADC2 for WiFi functions

Look in file [**adc_common.c**](https://github.com/espressif/esp-idf/blob/master/components/driver/adc_common.c#L61)

> In ADC2, there're two locks used for different cases:
> 1. lock shared with app and Wi-Fi:
>    ESP32:
>         When Wi-Fi using the ADC2, we assume it will never stop, so app checks the lock and returns immediately if failed.
>    ESP32S2:
>         The controller's control over the ADC is determined by the arbiter. There is no need to control by lock.
> 
> 2. lock shared between tasks:
>    when several tasks sharing the ADC2, we want to guarantee
>    all the requests will be handled.
>    Since conversions are short (about 31us), app returns the lock very soon,
>    we use a spinlock to stand there waiting to do conversions one by one.
> 
> adc2_spinlock should be acquired first, then adc2_wifi_lock or rtc_spinlock.


- In order to use ADC2 for other functions, we have to **acquire complicated firmware locks and very difficult to do**
- So, it's not advisable to use ADC2 with WiFi/BlueTooth (BT/BLE).
- Use ADC1, and pins GPIO32-GPIO39
- If somehow it's a must to use those pins serviced by ADC2 (**GPIO0, 2, 4, 12, 13, 14, 15, 25, 26 and 27**), use the **fix mentioned at the end** of [**ESP_WiFiManager Issue 39: Not able to read analog port when using the autoconnect example**](https://github.com/khoih-prog/ESP_WiFiManager/issues/39) to work with ESP32 WiFi/BlueTooth (BT/BLE).


---
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

- Choose one of the access points scanned, enter password, click **Save**.
- ESP will restart, then try to connect to the WiFi netwotk using STA-only mode, **without running the ConfigPortal WebServer and WiFi AP**. See [Accessing manager after connection](https://github.com/khoih-prog/ESP_WiFiManager/issues/15).

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

  // From v1.1.0
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;

  // LittleFS has higher priority than SPIFFS
  #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
    #define USE_LITTLEFS    true
    #define USE_SPIFFS      false
  #elif defined(ARDUINO_ESP32C3_DEV)
    // For core v1.0.6-, ESP32-C3 only supporting SPIFFS and EEPROM. To use v2.0.0+ for LittleFS
    #define USE_LITTLEFS          false
    #define USE_SPIFFS            true
  #endif

  #if USE_LITTLEFS
    // Use LittleFS
    #include "FS.h"

    // Check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
    //#if ( ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0) )  //(ESP_ARDUINO_VERSION_MAJOR >= 2)
    #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
      #warning Using ESP32 Core 1.0.6 or 2.0.0+
      // The library has been merged into esp32 core from release 1.0.6
      #include <LittleFS.h>       // https://github.com/espressif/arduino-esp32/tree/master/libraries/LittleFS
      
      FS* filesystem =      &LittleFS;
      #define FileFS        LittleFS
      #define FS_Name       "LittleFS"
    #else
      #warning Using ESP32 Core 1.0.5-. You must install LITTLEFS library
      // The library has been merged into esp32 core from release 1.0.6
      #include <LITTLEFS.h>       // https://github.com/lorol/LITTLEFS
      
      FS* filesystem =      &LITTLEFS;
      #define FileFS        LITTLEFS
      #define FS_Name       "LittleFS"
    #endif
    
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
    FS* filesystem =      &LittleFS;
    #define FileFS        LittleFS
    #define FS_Name       "LittleFS"
  #else
    FS* filesystem =      &SPIFFS;
    #define FileFS        SPIFFS
    #define FS_Name       "SPIFFS"
  #endif
  //////
  
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

  // From v1.1.0
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;

  // LittleFS has higher priority than SPIFFS
  #define USE_LITTLEFS    true
  #define USE_SPIFFS      false

  #if USE_LITTLEFS
    // Use LittleFS
    #include "FS.h"

    // Check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
    //#if ( ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0) )  //(ESP_ARDUINO_VERSION_MAJOR >= 2)
    #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
      #warning Using ESP32 Core 1.0.6 or 2.0.0+
      // The library has been merged into esp32 core from release 1.0.6
      #include <LittleFS.h>
      
      FS* filesystem =      &LittleFS;
      #define FileFS        LittleFS
      #define FS_Name       "LittleFS"
    #else
      #warning Using ESP32 Core 1.0.5-. You must install LITTLEFS library
      // The library has been merged into esp32 core from release 1.0.6
      #include <LITTLEFS.h>             // https://github.com/lorol/LITTLEFS
      
      FS* filesystem =      &LITTLEFS;
      #define FileFS        LITTLEFS
      #define FS_Name       "LittleFS"
    #endif
    
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
bool initialConfig = false;

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

#if ( USE_DHCP_IP )
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

// New in v1.4.0
IPAddress APStaticIP  = IPAddress(192, 168, 100, 1);
IPAddress APStaticGW  = IPAddress(192, 168, 100, 1);
IPAddress APStaticSN  = IPAddress(255, 255, 255, 0);

#include <ESP_WiFiManager.h>              //https://github.com/khoih-prog/ESP_WiFiManager

///////////////////////////////////////////
// New in v1.4.0
/******************************************
 * // Defined in ESPAsync_WiFiManager.h
typedef struct
{
  IPAddress _ap_static_ip;
  IPAddress _ap_static_gw;
  IPAddress _ap_static_sn;

}  WiFi_AP_IPConfig;

typedef struct
{
  IPAddress _sta_static_ip;
  IPAddress _sta_static_gw;
  IPAddress _sta_static_sn;
#if USE_CONFIGURABLE_DNS  
  IPAddress _sta_static_dns1;
  IPAddress _sta_static_dns2;
#endif
}  WiFi_STA_IPConfig;
******************************************/

WiFi_AP_IPConfig  WM_AP_IPconfig;
WiFi_STA_IPConfig WM_STA_IPconfig;
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

---

#### 12. Using fixed AP-mode channel, for example channel 3


```cpp
// From v1.0.10 only
// Set config portal channel, default = 1. Use 0 => random channel from 1-13
ESP_wifiManager.setConfigPortalChannel(3);
//////
```
---

#### 13. Setting STA-mode static IP


```cpp
// Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
//ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);
ESP_wifiManager.setSTAStaticIPConfig(WM_STA_IPconfig);
```
---

#### 14. Using AUTOCONNECT_NO_INVALIDATE feature

1. Don't invalidate WiFi SSID/PW when calling autoConnect()  (default)

```cpp
#define AUTOCONNECT_NO_INVALIDATE     true
```

2. To invalidate WiFi SSID/PW when calling autoConnect()

```cpp
#define AUTOCONNECT_NO_INVALIDATE     false
```
---

#### 15. Using CORS (Cross-Origin Resource Sharing) feature

1. To use CORS feature with **default** CORS Header "*". Some WebBrowsers won't accept this allowing-all "*" CORS Header.

```cpp
// Default false for using only whenever necessary to avoid security issue
#define USING_CORS_FEATURE     true
```

2. To use CORS feature with specific CORS Header "Your Access-Control-Allow-Origin". **To be modified** according to your specific Allowed-Origin.

```cpp
// Default false for using only whenever necessary to avoid security issue
#define USING_CORS_FEATURE     true

...

  // New from v1.1.1
#if USING_CORS_FEATURE
  ESP_wifiManager.setCORSHeader("Your Access-Control-Allow-Origin");
#endif
```

3. Not use CORS feature (default)

```cpp
// Default false for using only whenever necessary to avoid security issue
#define USING_CORS_FEATURE     false
```

---

#### 16. Using MultiWiFi auto(Re)connect feature

1. In loop()

```cpp
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
  static ulong checkwifi_timeout    = 0;

  static ulong current_millis;

#define WIFICHECK_INTERVAL    1000L

  current_millis = millis();
  
  // Check WiFi every WIFICHECK_INTERVAL (1) seconds.
  if ((current_millis > checkwifi_timeout) || (checkwifi_timeout == 0))
  {
    check_WiFi();
    checkwifi_timeout = current_millis + WIFICHECK_INTERVAL;
  }
}

void loop()
{
  // put your main code here, to run repeatedly
  check_status();
}
```

---

#### 17. How to auto getting _timezoneName


1. Turn on auto NTP configuration by

```
// Use false to disable NTP config. Advisable when using Cellphone, Tablet to access Config Portal.
// See Issue 23: On Android phone ConfigPortal is unresponsive (https://github.com/khoih-prog/ESP_WiFiManager/issues/23)
#define USE_ESP_WIFIMANAGER_NTP     true
```

2. The `_timezoneName`, in the format similar to **America/New_York, America/Toronto, Europe/London, etc.**, can be retrieved by using


```
String tempTZ = ESP_wifiManager.getTimezoneName();
```

---

#### 18. How to get TZ variable to configure Timezone


1. ESP32 and ESP8266 TZ can be configured, using the  similar to `EST5EDT,M3.2.0,M11.1.0` (for America/New_York) , as follows:

```
// EST5EDT,M3.2.0,M11.1.0 (for America/New_York)
// EST5EDT is the name of the time zone
// EST is the abbreviation used when DST is off
// 6 hours is the time difference from GMT
// EDT is the abbreviation used when DST is on
// ,M3 is the third month
// .2 is the second occurrence of the day in the month
// .0 is Sunday
// ,M11 is the eleventh month
// .1 is the first occurrence of the day in the month
// .0 is Sunday

#if ESP8266
      configTime(WM_config.TZ, "pool.ntp.org"); 
#else
      //configTzTime(WM_config.TZ, "pool.ntp.org" );
      configTzTime(WM_config.TZ, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
#endif
```

2. To convert from `_timezoneName` to TZ, use the function getTZ() as follows:

```
const char * TZ_Result = ESP_wifiManager.getTZ(_timezoneName);
```

The conversion depends on the stored TZs, which is using some memory, and can cause issue for ESP8266 in certain cases. Therefore, enable just the region you're interested.

For example, your application is used in America continent, you need just

```
#define USING_AMERICA       true
```

Hereafter is the regions' list


```
// Just use enough to save memory. On ESP8266, can cause blank ConfigPortal screen
// if using too much memory
#define USING_AFRICA        false
#define USING_AMERICA       true
#define USING_ANTARCTICA    false
#define USING_ASIA          false
#define USING_ATLANTIC      false
#define USING_AUSTRALIA     false
#define USING_EUROPE        false
#define USING_INDIAN        false
#define USING_PACIFIC       false
#define USING_ETC_GMT       false
```

---


#### 19. How to use the TZ variable to configure Timezone


```
#if ESP8266
      configTime(WM_config.TZ, "pool.ntp.org");
#else
      //configTzTime(WM_config.TZ, "pool.ntp.org" );
      configTzTime(WM_config.TZ, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
#endif
```

then to print local time


```
void printLocalTime()
{
#if ESP8266
  static time_t now;
  
  now = time(nullptr);
  
  if ( now > 1451602800 )
  {
    Serial.print("Local Date/Time: ");
    Serial.print(ctime(&now));
  }
#else
  struct tm timeinfo;

  getLocalTime( &timeinfo );

  // Valid only if year > 2000. 
  // You can get from timeinfo : tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec
  if (timeinfo.tm_year > 100 )
  {
    Serial.print("Local Date/Time: ");
    Serial.print( asctime( &timeinfo ) );
  }
#endif
}
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
//ESP_wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
ESP_wifiManager.setAPStaticIPConfig(WM_AP_IPconfig);
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
---

### HOWTO Add Dynamic Parameters


These illustrating steps is based on the example [ConfigOnSwitchFS](examples/ConfigOnSwitchFS)

###  1. Determine the variables to be configured via Config Portal (CP)

The application will:

- use DHT sensor (either DHT11 or DHT22) and 
- need to connect to ThingSpeak with unique user's API Key. 

The DHT sensor is connected to the ESP boards using SDA/SCL pins which also need to be configurable.

So this is the list of variables to be dynamically configured using CP

```
1. `thingspeakApiKey`,  type `char array`, max length 17 chars, and just arbitrarily selected default value to be "" or "ThingSpeak-APIKey"
2. `sensorDht22`,       type `bool`, default to be `true` (DHT22)
3. `pinSda`,            type `int`,  default to be `PIN_D2`
4. `pinScl`,            type `int`,  default to be `PIN_D1`
```

The Label can be any arbitrary string that help you identify the variable, but must be unique in your application

The initial code will be

```
#define API_KEY_LEN                 17

// Default configuration values
char thingspeakApiKey[API_KEY_LEN]  = "";
bool sensorDht22                    = true;
int pinSda                          = PIN_D2;     // Pin D2 mapped to pin GPIO4 of ESP8266
int pinScl                          = PIN_D1;     // Pin D1 mapped to pin GPIO5 of ESP8266

// Any unique string helping you identify the vars
#define ThingSpeakAPI_Label         "thingspeakApiKey"
#define SensorDht22_Label           "SensorDHT22"
#define PinSDA_Label                "PinSda"
#define PinSCL_Label                "PinScl"
```

---

###  2. Initialize the variables to prepare for Config Portal (CP)

The example [ConfigOnSwitchFS](examples/ConfigOnSwitchFS) will open the CP whenever a SW press is detected in loop(). So the code to add `dynamic variables` will be there, just after the CP `ESP_WiFiManager` class initialization to create `ESP_wifiManager` object.

```
void loop()
{
  // is configuration portal requested?
  if ((digitalRead(TRIGGER_PIN) == LOW) || (digitalRead(TRIGGER_PIN2) == LOW))
  {
    Serial.println("\nConfiguration portal requested.");
    digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

    //Local intialization. Once its business is done, there is no need to keep it around
    ESP_WiFiManager ESP_wifiManager("ConfigOnSwitchFS");

    //Check if there is stored WiFi router/password credentials.
    //If not found, device will remain in configuration mode until switched off via webserver.
    Serial.print("Opening configuration portal. ");
    
    ...
    
    // The addition of dynamic vars will be somewhere here
    
}    
```

The `ESP_WMParameter` class constructor will be used to initialize each newly-added parameter object.

#### 2.1 Use the following simple constructor for simple variables such as `thingspeakApiKey`, `pinSda` and `pinScl` :

```
ESP_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
```

#### 2.2 For example, to create a new `ESP_WMParameter` object `p_thingspeakApiKey` for `thingspeakApiKey`, 

The command to use will be 


```
ESP_WMParameter p_thingspeakApiKey(ThingSpeakAPI_Label, "Thingspeak API Key", thingspeakApiKey, API_KEY_LEN);

```

where

```
- p_thingspeakApiKey                  : ESP_WMParameter class object reference that stores the new Custom Parameter
- id => ThingSpeakAPI_Label           : var ref to Json associative name and HTML element ID for the new Custom Paramerter you just defined in step 1
- placeholder => "Thingspeak API Key" : HTML input placeholder and/or label element text the user sees in the configuration interface for this Custom Parameter
- defaultValue => thingspeakApiKey    : variable for storing the value of your Custom Parameter in the file system or default value when no data is entered
- length  => API_KEY_LEN              : max allowed length you want for this Custom Parameter to have
```

For `pinSda` and `pinScl`, the command will be similar

```
// I2C SCL and SDA parameters are integers so we need to convert them to char array but
// no other special considerations
char convertedValue[3];
sprintf(convertedValue, "%d", pinSda);
ESP_WMParameter p_pinSda(PinSDA_Label, "I2C SDA pin", convertedValue, 3);

sprintf(convertedValue, "%d", pinScl);
ESP_WMParameter p_pinScl(PinSCL_Label, "I2C SCL pin", convertedValue, 3);
```

where

```
- p_pinSda / p_pinScl                         : ESP_WMParameter class object reference that stores the new Custom Parameter
- id => PinSDA_Label/PinSCL_Label             : var ref to Json associative name and HTML element ID for the new Custom Paramerter you just defined in step 1
- placeholder => "I2C SDA pin"/"I2C SCL pin"  : HTML input placeholder and/or label element text the user sees in the configuration interface for this Custom Parameter
- defaultValue => convertedValue              : variable for storing the value of your Custom Parameter in the file system or default value when no data is entered
- length  => 3                                : max allowed length you want for this Custom Parameter to have
```

---

#### 2.3 Use the more complex following constructor for variables such as `sensorDht22`:

```
ESP_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom, int labelPlacement);
```

#### 2.4 For example, to create a new `ESP_WMParameter` object `p_sensorDht22` for `sensorDht22`, 

The command to use will be 


```
ESP_WMParameter p_sensorDht22(SensorDht22_Label, "DHT-22 Sensor", "T", 2, customhtml, WFM_LABEL_AFTER);
```

where

```
- p_sensorDht22                       : ESP_WMParameter class object reference that stores the new Custom Parameter
- id => SensorDht22_Label             : var ref to Json associative name and HTML element ID for the new Custom Paramerter you just defined in step 1
- placeholder => "DHT-22 Sensor"      : HTML input placeholder and/or label element text the user sees in the configuration interface for this Custom Parameter
- defaultValue => "T"                 : variable for storing the value of your Custom Parameter in the file system or default value when no data is entered ("T" means `true`)
- length  => 2                        : max allowed length you want for this Custom Parameter to have
- custom => customhtml                : custom HTML code to add element type, e.g. `checkbox`, and `checked` when `sensorDht22 == true`
- labelPlacement => WFM_LABEL_AFTER   : to place label after
```

and customhtml Code is:

```
char customhtml[24] = "type=\"checkbox\"";

if (sensorDht22)
{
  strcat(customhtml, " checked");
}
```

---

###  3. Add the variables to Config Portal (CP)

Adding those `ESP_WMParameter` objects created in Step 2 using the function `addParameter()` of object `ESP_wifiManager`

#### 3.1 addParameter() function Prototype:

```
//adds a custom parameter
bool addParameter(ESP_WMParameter *p);
```

#### 3.2 Code to add variables to CP


Add parameter objects, previously created in Step 2, such as : `p_thingspeakApiKey`, `p_sensorDht22`, `p_pinSda` and `p_pinScl`

```
//add all parameters here

ESP_wifiManager.addParameter(&p_thingspeakApiKey);
ESP_wifiManager.addParameter(&p_sensorDht22);
ESP_wifiManager.addParameter(&p_pinSda);
ESP_wifiManager.addParameter(&p_pinScl);
```

---

###  4. Save the variables configured in Config Portal (CP)

When the CP exits, we have to store the parameters' values that users input via CP to use later.

For ESP32, that can be `EEPROM` or `SPIFFS`. While on ESP8266, `LittleFS` can be used besides `EEPROM` or `deprecated SPIFFS`.

We can write directly to a **well-defined structure of our choice**, but the current example is using `JSON` to be portable but **much more complicated and not advised for new users**.


#### 4.1 Getting variables' data from CP

After users select `Save`, the CP `ESP_wifiManager` object will save the user input data into related `ESP_WMParameter` objects.

We can now retrieve the data, using `getValue()` function, for each `ESP_WMParameter` object. Then we can utilize the data for our purpose, such as `thingspeakApiKey` to log in, `sensorDht22` type to know how to handle the sensor, `pinSda` and `pinSda` to know which pins to use to communicate with the DHT sensor.


The code is as follows:

```
// Getting posted form values and overriding local variables parameters
// Config file is written regardless the connection state
strcpy(thingspeakApiKey, p_thingspeakApiKey.getValue());
sensorDht22 = (strncmp(p_sensorDht22.getValue(), "T", 1) == 0);
pinSda = atoi(p_pinSda.getValue());
pinScl = atoi(p_pinScl.getValue());
```

We can also save to FS file to use later in next boot.

```
// Writing JSON config file to flash for next boot
writeConfigFile();
```

---

### 5. Write to FS (SPIFFS, LittleFS, etc.) using JSON format

First, you have to familiarize yourself with `ArduinoJson` library, its functions, the disruptive differences between `ArduinoJson version 5.x.x-` and `v6.0.0+`. The best documentation can be found at [The best JSON library for embedded C++](https://arduinojson.org/).

This documentation will discuss only ArduinoJson v6.x.x+ (`ARDUINOJSON_VERSION_MAJOR >= 6`)


Then have a look at the code snippet of `writeConfigFile()` function and the following step-by-step explanations.


```
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
  json[ThingSpeakAPI_Label] = thingspeakApiKey;
  json[SensorDht22_Label] = sensorDht22;
  json[PinSDA_Label] = pinSda;
  json[PinSCL_Label] = pinScl;

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
```

#### 5.1 Create a DynamicJsonDocument Object

We'll create an object with size 1024 bytes, enough to hold our data:

```
DynamicJsonDocument json(1024);
```

#### 5.2 Fill the DynamicJsonDocument Object with data got from Config Portal

Then `JSONify` all local parameters we've just received from CP and wish to store into FS by using the function prototype:

```
json[Unique_Label] = Value_For_Unique_Label;
```

as follows:

```
// JSONify local configuration parameters
json[ThingSpeakAPI_Label] = thingspeakApiKey;
json[SensorDht22_Label]   = sensorDht22;
json[PinSDA_Label]        = pinSda;
json[PinSCL_Label]        = pinScl;
```


#### 5.3 Open file to write the Jsonified data


This is the `CONFIG_FILE` file name we already declared at the beginning of the sketch (for ESP32):

```
#include <SPIFFS.h>
FS* filesystem =      &SPIFFS;
#define FileFS        SPIFFS
    
const char* CONFIG_FILE = "/ConfigSW.json";
```

Now just open the file for writing, and abort if open-for-writing error:


```
// Open file for writing
File f = FileFS.open(CONFIG_FILE, "w");

if (!f)
{
  Serial.println("Failed to open config file for writing");
  return false;
}
```


#### 5.4 Write the Jsonified data to CONFIG_FILE

As simple as this single command to write the whole `json` object we declared then filled with data in steps 5.1 and 5.2

```
// Write data to file and close it
serializeJson(json, f);
```

#### 5.5 Close CONFIG_FILE to flush and save the data

Soooo simple !!! Now everybody can do it.

```
f.close();
```


But **HOWTO use the saved data in the next startup** ???? That's in next step 6.


### 6. Read from FS using JSON format


Now, you have familiarized yourself with ArduinoJson library, its functions. We'll discuss HOWTO read data from the CONFIG_FILE in Jsonified format, then HOWTO parse the to use.

The documentation will discuss only ArduinoJson v6.x.x+ (`ARDUINOJSON_VERSION_MAJOR >= 6`)


First, have a look at the code snippet of `readConfigFile()` function.


```
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
    if (json.containsKey(ThingSpeakAPI_Label))
    {
      strcpy(thingspeakApiKey, json[ThingSpeakAPI_Label]);
    }

    if (json.containsKey(SensorDht22_Label))
    {
      sensorDht22 = json[SensorDht22_Label];
    }

    if (json.containsKey(PinSDA_Label))
    {
      pinSda = json[PinSDA_Label];
    }

    if (json.containsKey(PinSCL_Label))
    {
      pinScl = json[PinSCL_Label];
    }
  }
  Serial.println("\nConfig file was successfully parsed");
  return true;
}

```

and the following step-by-step explanations. 
 
#### 6.1 Open CONFIG_FILE to read

As simple as this

```
// this opens the config file in read-mode
File f = FileFS.open(CONFIG_FILE, "r");
```

We'll inform and abort if the CONFIG_FILE can't be opened (file not found, can't be opened, etc.)

```
if (!f)
{
  Serial.println("Configuration file not found");
  return false;
}
```

#### 6.2 Open CONFIG_FILE to read

Now we have to determine the file size to create a buffer large enough to store the to-be-read data

```
// we could open the file
size_t size = f.size();
// Allocate a buffer to store contents of the file.
std::unique_ptr<char[]> buf(new char[size + 1]);
```

**Remember always add 1 to the buffer length to store the terminating `0`.**


Then just read the file into the buffer, and close the file to be safe

```
// Read and store file contents in buf
f.readBytes(buf.get(), size);
// Closing file
f.close();
```

#### 6.3 Populate the just-read Jsonified data into the DynamicJsonDocument json object

We again use the same `DynamicJsonDocument json` object to store the data we've just read fron `CONFIG_FILE`.

Why the same complicated `DynamicJsonDocument json` object ?? Because in steps 5, we did store `Jsonified data` using the same `DynamicJsonDocument json` object. It's much easier we now use it again to facilitate the parsing of `Jsonified data` back to the data we can use easily.


We first create the object with enough size

```
DynamicJsonDocument json(1024);
```

then populate it with data from buffer we read from CONFIG_FILE in step 6.2, pre-parse and check for error. All is done just by one command `deserializeJson()`

```
auto deserializeError = deserializeJson(json, buf.get());
```

Abort if there is any data error in the process of writing, storing, reading back. If OK, just nicely print out to the Debug Terminal

```
if ( deserializeError )
{
  Serial.println("JSON parseObject() failed");
  return false;
}

serializeJson(json, Serial);
```

#### 6.4 Parse the Jsonified data from the DynamicJsonDocument json object to store into corresponding parameters


This is as simple as in the step 5.2, but in reverse direction.

To be sure there is good corresponding data, not garbage, for each variable, we have to perform **sanity checks** by 
verifying the `DynamicJsonDocument json object` still contains the correct keys we passed to it when we wrote into CONFIG_FILE. 

For example:

```
if (json.containsKey(ThingSpeakAPI_Label))
```

Then proceed to get every parameter we know we stored there from last CP `Save`.


```
// Parse all config file parameters, override
// local config variables with parsed values
if (json.containsKey(ThingSpeakAPI_Label))
{
  strcpy(thingspeakApiKey, json[ThingSpeakAPI_Label]);
}

if (json.containsKey(SensorDht22_Label))
{
  sensorDht22 = json[SensorDht22_Label];
}

if (json.containsKey(PinSDA_Label))
{
  pinSda = json[PinSDA_Label];
}

if (json.containsKey(PinSCL_Label))
{
  pinScl = json[PinSCL_Label];
}
```


#### 6.5 Then what to do now

**Just use those parameters for whatever purpose you designed them for in step 1:**


```
The application will use DHT sensor (either DHT11 or DHT22) and need to connect to ThingSpeak with unique user's API Key. The DHT sensor is connected to the ESP boards using SDA/SCL pins which also need to be configurable.
```

---
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

Enter your credentials, then click **Save**. The WiFi Credentials will be saved and the board reboots to connect to the selected WiFi AP.

<p align="center">
    <img src="https://github.com/khoih-prog/ESP_WiFiManager/blob/master/Images/Saved.png">
</p>

If you're already connected to a listed WiFi AP and don't want to change anything, just select **Exit Portal** from the `Main` page to reboot the board and connect to the previously-stored AP. The WiFi Credentials are still intact.

---
---

## Documentation

### Password protect the configuration Access Point

You can password protect the ConfigPortal AP.  Simply add an SSID as the first parameter and the password as a second parameter to `startConfigPortal`. See the above examples.
A short password seems to have unpredictable results so use one that's around 8 characters or more in length.
The guidelines are that a wifi password must consist of 8 to 63 ASCII-encoded characters in the range of 32 to 126 (decimal)

```cpp
ESP_wifiManager.startConfigPortal( SSID , password )
```

### Callbacks

#### Save settings

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
  // is configuration portal requested?
  if ((digitalRead(TRIGGER_PIN) == LOW) || (digitalRead(TRIGGER_PIN2) == LOW))
  {
    Serial.println(F("\nConfiguration portal requested."));
    digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

    //Local intialization. Once its business is done, there is no need to keep it around
    ESP_WiFiManager ESP_wifiManager("ConfigOnSwitch");

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

    //Check if there is stored WiFi router/password credentials.
    //If not found, device will remain in configuration mode until switched off via webserver.
    Serial.println(F("Opening configuration portal. "));
    
    Router_SSID = ESP_wifiManager.WiFi_SSID();
    Router_Pass = ESP_wifiManager.WiFi_Pass();

    //Remove this line if you do not want to see WiFi password printed
    Serial.println("ESP Self-Stored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);
   
    // From v1.1.0, Don't permit NULL password
    if ( (Router_SSID != "") && (Router_Pass != "") )
    {
      LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass);
      wifiMulti.addAP(Router_SSID.c_str(), Router_Pass.c_str());
      
      ESP_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
      Serial.println(F("Got ESP Self-Stored Credentials. Timeout 120s for Config Portal"));
    }
    else if (loadConfigData())
    {      
      ESP_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
      Serial.println(F("Got stored Credentials. Timeout 120s for Config Portal")); 
    }
    else
    {
      // Enter CP only if no stored SSID on flash and file 
      Serial.println(F("Open Config Portal without Timeout: No stored Credentials."));
      initialConfig = true;
    }
    
    //Starts an access point
    //and goes into a blocking loop awaiting configuration
    if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
    {
      Serial.println(F("Not connected to WiFi but continuing anyway."));
    }
    else
    {
      //if you get here you have connected to the WiFi
      Serial.println(F("connected...yeey :)"));
      Serial.print(F("Local IP: "));
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
    
      // New in v1.4.0
      ESP_wifiManager.getSTAStaticIPConfig(WM_STA_IPconfig);
      //////
      
      saveConfigData();
    }

    digitalWrite(PIN_LED, LED_OFF); // Turn led off as we are not in configuration mode.
  }

  // put your main code here, to run repeatedly
  check_status();
}
```

See  [ConfigOnSwitch](examples/ConfigOnSwitch) example for a more complex version.

---
---

### Custom Parameters

Many applications need configuration parameters like `MQTT host and port`, [Blynk](http://www.blynk.cc) or [emoncms](http://emoncms.org) tokens, etc. While it is possible to use `ESP_WiFiManager` to collect additional parameters it is better to read these parameters from a web service once `ESP_WiFiManager` has been used to connect to the internet.

To capture other parameters with `ESP_WiFiManager` is a lot more involved than all the other features and requires adding custom HTML to your form. If you want to do it with `ESP_WiFiManager` see the example [ConfigOnSwitchFS](examples/ConfigOnSwitchFS)

### Custom IP Configuration

You can set a custom IP for both AP (access point, config mode) and STA (station mode, client mode, normal project state)

#### Custom Access Point IP Configuration

This will set your captive portal to a specific IP should you need/want such a feature. Add the following snippet before `startConfigPortal()`
```cpp
//set custom ip for portal
//ESP_wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
ESP_wifiManager.setAPStaticIPConfig(WM_AP_IPconfig);
```

#### Custom Station (client) Static IP Configuration

This will use the specified IP configuration instead of using DHCP in station mode.
```cpp
//ESP_wifiManager.setSTAStaticIPConfig(IPAddress(192,168,0,99), IPAddress(192,168,0,1), IPAddress(255,255,255,0));
ESP_wifiManager.setSTAStaticIPConfig(WM_STA_IPconfig);
```

### Custom HTML, CSS, Javascript

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

### Filter Networks

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
---

### Examples

#### Medium Complexity

 1. [AutoConnect](examples/AutoConnect)
 2. [AutoConnectWithFeedback](examples/AutoConnectWithFeedback)
 3. [AutoConnectWithFeedbackLED](examples/AutoConnectWithFeedbackLED)
 4. [AutoConnectWithFSParameters](examples/AutoConnectWithFSParameters)
 5. [ConfigOnDoubleReset](examples/ConfigOnDoubleReset)  (now support ArduinoJson 6.0.0+ as well as 5.13.5-)
 6. [ConfigOnDRD_FS_MQTT_Ptr](examples/ConfigOnDRD_FS_MQTT_Ptr)
 7. [ConfigOnStartup](examples/ConfigOnStartup)
 8. [ConfigOnSwitch](examples/ConfigOnSwitch)
 9. [ConfigOnSwitchFS](examples/ConfigOnSwitchFS)
10. [ConfigOnSwitchFS_MQTT_Ptr](examples/ConfigOnSwitchFS_MQTT_Ptr)
11. [ConfigPortalParamsOnSwitch](examples/ConfigPortalParamsOnSwitch)  (now support ArduinoJson 6.0.0+ as well as 5.13.5-)
12. [ESP32_FSWebServer](examples/ESP32_FSWebServer)
13. [ESP32_FSWebServer_DRD](examples/ESP32_FSWebServer_DRD)
14. [ESP_FSWebServer](examples/ESP_FSWebServer)
15. [ESP_FSWebServer_DRD](examples/ESP_FSWebServer_DRD)


#### High Complexity

 1. [ConfigOnDRD_FS_MQTT_Ptr_Complex](examples/ConfigOnDRD_FS_MQTT_Ptr_Complex)
 2. [ConfigOnDRD_FS_MQTT_Ptr_Medium](examples/ConfigOnDRD_FS_MQTT_Ptr_Medium)


#### Multiple-Definitions-Linker-Error demo

 1. [ConfigOnDoubleReset_Multi](examples/ConfigOnDoubleReset_Multi)
 
---
---

### Example [ConfigOnDRD_FS_MQTT_Ptr](examples/ConfigOnDRD_FS_MQTT_Ptr)

https://github.com/khoih-prog/ESP_WiFiManager/blob/a85a4cf3e346f3849678dce69f9fec9cf1288729/examples/ConfigOnDRD_FS_MQTT_Ptr/ConfigOnDRD_FS_MQTT_Ptr.ino#L17-L1410



---
---

### Debug Termimal Output Samples

#### 1. [ConfigOnSwitchFS_MQTT_Ptr](examples/ConfigOnSwitchFS_MQTT_Ptr) on ESP8266_NODEMCU_ESP12E

This is terminal debug output when running [ConfigOnSwitchFS_MQTT_Ptr](examples/ConfigOnSwitchFS_MQTT_Ptr) on **ESP8266_NODEMCU_ESP12E**. Config Portal was requested to input and save MQTT Credentials. The boards then connected to Adafruit MQTT Server successfully.

```
Starting ConfigOnSwichFS_MQTT_Ptr using LittleFS on ESP8266_NODEMCU_ESP12E
ESP_WiFiManager v1.10.2
Configuration file not found
Failed to read configuration file, using default values
[WM] RFC925 Hostname = ConfigOnSwichFS-MQTT
[WM] setSTAStaticIPConfig for USE_CONFIGURABLE_DNS
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
ESP Self-Stored: SSID = HueNet, Pass = 12345678
[WM] * Add SSID =  HueNet , PW =  12345678
Got stored Credentials. Timeout 120s for Config Portal
Starting configuration portal.
[WM] WiFi.waitForConnectResult Done
[WM] SET AP_STA
[WM] Configuring AP SSID = ESP_119055
[WM] AP PWD = your_password
[WM] AP Channel = 3
[WM] AP IP address = 192.168.4.1
[WM] HTTP server started
[WM] ESP_WiFiManager::startConfigPortal : Enter loop
[WM] Custom STA IP/GW/Subnet
[WM] DNS1 and DNS2 set
[WM] setWifiStaticIP IP = 192.168.2.186
[WM] Connected after waiting (s) : 0.12
[WM] Local ip = 192.168.2.186
[WM] Timed out connection result: WL_CONNECTED
WiFi connected...yeey :)
[WM] SaveWiFiCfgFile 
[WM] OK
After waiting 0.00 secs more in setup(), connection result is connected. Local IP: 192.168.2.186
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
...
FWConnecting to WiFi MQTT (3 attempts)...
Connection failed
Another attemtpt to connect to MQTT in 2 seconds...
Connection failed
Another attemtpt to connect to MQTT in 2 seconds...
Connection failed
Another attemtpt to connect to MQTT in 2 seconds...
WiFi MQTT connection failed. Continuing with program...
FWButton clicked!

Configuration Portal requested.
[WM] RFC925 Hostname = ConfigOnSwitchFS-MQTT
Opening Configuration Portal. Got stored Credentials. Timeout 120s
[WM] Adding parameter 
[WM] Adding parameter AIO_SERVER_Label
[WM] Adding parameter AIO_SERVERPORT_Label
[WM] Adding parameter AIO_USERNAME_Label
[WM] Adding parameter AIO_KEY_Label
[WM] setSTAStaticIPConfig for USE_CONFIGURABLE_DNS
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
[WM] WiFi.waitForConnectResult Done
[WM] SET AP_STA
[WM] Configuring AP SSID = ESP_119055
[WM] AP PWD = your_password
[WM] AP Channel = 6
[WM] AP IP address = 192.168.4.1
[WM] HTTP server started
[WM] ESP_WiFiManager::startConfigPortal : Enter loop
[WM] Connecting to new AP
[WM] Already connected. Bailing out.
Connected...yeey :)
Local IP: 192.168.2.186
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
[WM] SaveWiFiCfgFile 
[WM] OK
Saving config file
{
  "AIO_SERVER_Label": "io.adafruit.com",
  "AIO_SERVERPORT_Label": "1883",
  "AIO_USERNAME_Label": "account",
  "AIO_KEY_Label": "aio_key"
}
Config file was successfully saved
Deleting old MQTT object
Deleting old Temperature object
Creating new MQTT object OK
AIO_SERVER = io.adafruit.com, AIO_SERVERPORT = 1883
AIO_USERNAME = account, AIO_KEY = aio_key
Creating new MQTT_Pub_Topic,  Temperature = account/feeds/Temperature
Creating new Temperature object OK
Temperature MQTT_Pub_Topic = account/feeds/Temperature
[WM] freeing allocated params!
Connecting to WiFi MQTT (3 attempts)...
WiFi MQTT connection successful!
TWWWW WTWWWW WWTWWW WWWTWW WWWWTW 
```
---

#### 2. [ESP32_FSWebServer_DRD](examples/ESP32_FSWebServer_DRD) on ESP32_DEV

This is terminal debug output when running [ESP32_FSWebServer_DRD](examples/ESP32_FSWebServer_DRD) on **ESP32 ESP32_DEV using SPIFFS.**. Config Portal was requested by DRD to input and save Credentials. The boards then connected to WiFi AP **HueNet1** using new Static IP successfully. WiFi AP **HueNet1** is then lost, and board **autoreconnects** itself to backup WiFi AP **HueNet2**.

```cpp
Starting ESP32_FSWebServer_DRD with DoubleResetDetect using SPIFFS on ESP32_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
FS File: /ConfigSW.json, size: 150B
FS File: /CanadaFlag_1.png, size: 40.25KB
FS File: /CanadaFlag_2.png, size: 8.12KB
FS File: /CanadaFlag_3.jpg, size: 10.89KB
FS File: /edit.htm.gz, size: 4.02KB
FS File: /favicon.ico, size: 1.12KB
FS File: /graphs.js.gz, size: 1.92KB
FS File: /index.htm, size: 3.63KB
FS File: /drd.dat, size: 4B
FS File: /wifi_cred.dat, size: 192B

[WM] RFC925 Hostname = ESP32-FSWebServerDRD
[WM] setAPStaticIPConfig
[WM] setSTAStaticIPConfig for USE_CONFIGURABLE_DNS
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
ESP Self-Stored: SSID = HueNet2, Pass = 12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
Got stored Credentials. Timeout 120s for Config Portal
SPIFFS Flag read = 0xd0d04321
No doubleResetDetected
Saving config file...
Saving config file OK
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
ConnectMultiWiFi in setup
[WM] ConnectMultiWiFi with :
[WM] * Flash-stored Router_SSID =  HueNet2 , Router_Pass =  12345678
[WM] * Additional SSID =  HueNet1 , PW =  12345678
[WM] * Additional SSID =  HueNet2 , PW =  12345678
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  1
[WM] SSID: HueNet1 ,RSSI= -27
[WM] Channel: 2 ,IP address: 192.168.2.232
After waiting 3.16 secs more in setup(), connection result is connected. Local IP: 192.168.2.232
HTTP server started @ 192.168.2.232
Open http://esp32-fs-browser.local/edit to see the file browser
[WM] freeing allocated params!
Stop doubleResetDetecting
Saving config file...
Saving config file OK

WiFi lost. Call connectMultiWiFi in loop
[WM] ConnectMultiWiFi with :
[WM] * Flash-stored Router_SSID =  HueNet2 , Router_Pass =  12345678
[WM] * Additional SSID =  HueNet1 , PW =  12345678
[WM] * Additional SSID =  HueNet2 , PW =  12345678
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  3
[WM] SSID: HueNet2 ,RSSI= -59
[WM] Channel: 4 ,IP address: 192.168.2.232
HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH
```

---

#### 3. [ESP32_FSWebServer_DRD](examples/ESP32_FSWebServer_DRD) on ESP32_DEV using newly-supported LittleFS

This is terminal debug output when running [ESP32_FSWebServer_DRD](examples/ESP32_FSWebServer_DRD) on **ESP32 ESP32_DEV using newly-supported LittleFS.**. Config Portal was requested by DRD to input and save Credentials. The boards then connected to WiFi AP **HueNet1** using new Static IP successfully.

```
Starting ESP32_FSWebServer_DRD with DoubleResetDetect using LittleFS on ESP32_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
FS File: /CanadaFlag_1.png, size: 40.25KB
FS File: /CanadaFlag_2.png, size: 8.12KB
FS File: /CanadaFlag_3.jpg, size: 10.89KB
FS File: /Credentials.txt, size: 192B
FS File: /drd.dat, size: 4B
FS File: /edit.htm.gz, size: 4.02KB
FS File: /favicon.ico, size: 1.12KB
FS File: /graphs.js.gz, size: 1.92KB
FS File: /index.htm, size: 3.63KB
FS File: /wifi_cred.dat, size: 192B

[WM] RFC925 Hostname = ESP32-FSWebServerDRD
[WM] setAPStaticIPConfig
[WM] setSTAStaticIPConfig for USE_CONFIGURABLE_DNS
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
ESP Self-Stored: SSID = HueNet1, Pass = 12345678
[WM] * Add SSID =  HueNet1 , PW =  12345678
Got stored Credentials. Timeout 120s for Config Portal
LittleFS Flag read = 0xd0d01234
doubleResetDetected
Saving config file...
Saving config file OK
Open Config Portal without Timeout: Double Reset Detected
[WM] WiFi.waitForConnectResult Done
[WM] SET AP
[WM] Configuring AP SSID = ESP_9ABF498
[WM] AP PWD = your_password
[WM] AP Channel = 10
[WM] Custom AP IP/GW/Subnet =  192.168.232.1 192.168.232.1 255.255.255.0
[WM] AP IP address = 192.168.232.1
[WM] HTTP server started
[WM] ESP_WiFiManager::startConfigPortal : Enter loop
[WM] WiFi connected after time:  1
[WM] SSID: HueNet1 ,RSSI= -33
[WM] Channel: 2 ,IP address: 192.168.2.232
After waiting 2.92 secs more in setup(), connection result is connected. Local IP: 192.168.2.232
HTTP server started @ 192.168.2.232
Open http://esp32-fs-browser.local/edit to see the file browser
[WM] freeing allocated params!
HStop doubleResetDetecting
Saving config file...
Saving config file OK
HHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH
HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH
HHHHHHHHHH HHHHHHHHHH HHH
```

---

#### 4. [ConfigOnDRD_FS_MQTT_Ptr_Complex](examples/ConfigOnDRD_FS_MQTT_Ptr_Complex) on ESP32_DEV

This is terminal debug output when running [ConfigOnDRD_FS_MQTT_Ptr_Complex](examples/ConfigOnDRD_FS_MQTT_Ptr_Complex) on **ESP32 ESP32_DEV using LittleFS.**. Config Portal was then requested by DRD to input and save MQTT Credentials. The boards then connected to Adafruit MQTT Server successfully.

##### 4.1 With Config Data => Run normally

```
Starting ConfigOnDRD_FS_MQTT_Ptr_Complex using LittleFS on ESP32_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
{"AIO_KEY_Label":"aio_key","AIO_SERVER_Label":"io.adafruit.com","AIO_SERVERPORT_Label":"1883","AIO_USERNAME_Label":"user_name"}
Config File successfully parsed
LittleFS Flag read = 0xd0d04321
No doubleResetDetected
Saving config file...
Saving config file OK
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] stationIP = 192.168.2.235 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
ConnectMultiWiFi in setup
[WM] ConnectMultiWiFi with :
[WM] * Additional SSID =  HueNet1 , PW =  12345678
[WM] * Additional SSID =  HueNet2 , PW =  12345678
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  1
[WM] SSID: HueNet1 ,RSSI= -35
[WM] Channel: 2 ,IP address: 192.168.2.235
W
Creating new WiFi client object : OK
Creating new MQTT object : OK
AIO_SERVER = io.adafruit.com, AIO_SERVERPORT = 1883
AIO_USERNAME = user_name, AIO_KEY = aio_key
Creating new MQTT_Pub_Topic,  Temperature = user_name/feeds/Temperature
Creating new Temperature object : OK
Temperature MQTT_Pub_Topic = user_name/feeds/Temperature
Connecting to MQTT (3 attempts)...
MQTT connection successful!
TStop doubleResetDetecting
Saving config file...
Saving config file OK
WWWW WTWWWW WWTWWW WWWTWW WWWWTW WWWWW
```

#### 4.2 DRD => Config Portal

```
Starting ConfigOnDRD_FS_MQTT_Ptr_Complex using LittleFS on ESP32_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
{"AIO_KEY_Label":"aio_key","AIO_SERVER_Label":"io.adafruit.com","AIO_SERVERPORT_Label":"1883","AIO_USERNAME_Label":"user_name"}
Config File successfully parsed
LittleFS Flag read = 0xd0d01234
doubleResetDetected
Saving config file...
Saving config file OK
Open Config Portal without Timeout: Double Reset Detected

Config Portal requested.
[WM] RFC925 Hostname = ConfigOnSwichFS-MQTT
Opening Configuration Portal. No timeout : DRD or No stored Credentials..
[WM] Adding parameter AIO_KEY_Label
[WM] Adding parameter AIO_SERVER_Label
[WM] Adding parameter AIO_SERVERPORT_Label
[WM] Adding parameter AIO_USERNAME_Label
[WM] setAPStaticIPConfig
[WM] setSTAStaticIPConfig
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
[WM] WiFi.waitForConnectResult Done
[WM] SET AP
[WM] Configuring AP SSID = ESP_9ABF498
[WM] AP PWD = your_password
[WM] AP Channel = 4
[WM] Custom AP IP/GW/Subnet =  192.168.100.1 192.168.100.1 255.255.255.0
[WM] AP IP address = 192.168.100.1
[WM] HTTP server started
[WM] ESP_WiFiManager::startConfigPortal : Enter loop
```

#### 4.3 Config Portal Done

```
[WM] Connecting to new AP
[WM] Previous settings invalidated
[WM] Custom STA IP/GW/Subnet
[WM] DNS1 and DNS2 set
[WM] setWifiStaticIP IP = 192.168.2.235
[WM] Connect to new WiFi using new IP parameters
[WM] Connected after waiting (s) : 0.60
[WM] Local ip = 192.168.2.235
[WM] Connection result:  WL_CONNECTED
Connected...yeey :)
Local IP: 192.168.2.235
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
[WM] getSTAStaticIPConfig
[WM] stationIP = 192.168.2.235 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
[WM] SaveWiFiCfgFile 
[WM] OK
Saving Config File
{
  "AIO_KEY_Label": "aio_key",
  "AIO_SERVER_Label": "io.adafruit.com",
  "AIO_SERVERPORT_Label": "1883",
  "AIO_USERNAME_Label": "user_name"
}
Config File successfully saved

Creating new WiFi client object : OK
Creating new MQTT object : OK
AIO_SERVER = io.adafruit.com, AIO_SERVERPORT = 1883
AIO_USERNAME = user_name, AIO_KEY = aio_key
Creating new MQTT_Pub_Topic,  Temperature = user_name/feeds/Temperature
Creating new Temperature object : OK
Temperature MQTT_Pub_Topic = user_name/feeds/Temperature
[WM] freeing allocated params!
WConnecting to MQTT (3 attempts)...
MQTT connection successful!
TWWWW WTWWWW WWTWWW WWWTWW WWWWTW WWWWW
```

---

#### 5. [ConfigOnDRD_FS_MQTT_Ptr_Medium](examples/ConfigOnDRD_FS_MQTT_Ptr_Medium) on ESP8266_NODEMCU_ESP12E

This is terminal debug output when running [ConfigOnDRD_FS_MQTT_Ptr_Complex](examples/ConfigOnDRD_FS_MQTT_Ptr_Complex) on **ESP8266_NODEMCU_ESP12E using LittleFS.**. Config Portal was then requested by DRD to input and save MQTT Credentials. The boards then connected to Adafruit MQTT Server successfully.

##### 5.1 With Config Data => Run normally

```
Starting ConfigOnDRD_FS_MQTT_Ptr_Medium using LittleFS on ESP8266_NODEMCU_ESP12E
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
{"AIO_KEY_Label":"aio_key","AIO_SERVER_Label":"io.adafruit.com","AIO_SERVERPORT_Label":"1883","AIO_USERNAME_Label":"user_name"}
Config File successfully parsed
LittleFS Flag read = 0xd0d04321
No doubleResetDetected
Saving config file...
Saving config file OK
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] stationIP = 192.168.2.188 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
ConnectMultiWiFi in setup
[WM] ConnectMultiWiFi with :
[WM] * Additional SSID =  HueNet1 , PW =  12345678
[WM] * Additional SSID =  HueNet2 , PW =  12345678
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  1
[WM] SSID: HueNet1 ,RSSI= -32
[WM] Channel: 2 ,IP address: 192.168.2.188
W
Creating new WiFi client object : OK
Creating new MQTT object : OK
AIO_SERVER = io.adafruit.com, AIO_SERVERPORT = 1883
AIO_USERNAME = user_name, AIO_KEY = aio_key
Creating new MQTT_Pub_Topic,  Temperature = user_name/feeds/Temperature
Creating new Temperature object : OK
Temperature MQTT_Pub_Topic = user_name/feeds/Temperature
Connecting to MQTT (3 attempts)...
MQTT connection successful!
TWWWW WTWWWW WWTWWW WWWTWW WWWWTW WWWWW
```

#### 5.2 DRD => Config Portal

```
Starting ConfigOnDRD_FS_MQTT_Ptr_Medium using LittleFS on ESP8266_NODEMCU_ESP12E
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
{"AIO_KEY_Label":"aio_key","AIO_SERVER_Label":"io.adafruit.com","AIO_SERVERPORT_Label":"1883","AIO_USERNAME_Label":"user_name"}
Config File successfully parsed
LittleFS Flag read = 0xd0d01234
doubleResetDetected
Saving config file...
Saving config file OK
Open Config Portal without Timeout: Double Reset Detected

Config Portal requested.
[WM] RFC925 Hostname = ConfigOnSwichFS-MQTT
Opening Configuration Portal. No timeout : DRD or No stored Credentials..
[WM] Adding parameter AIO_SERVER_Label
[WM] Adding parameter AIO_SERVERPORT_Label
[WM] Adding parameter AIO_USERNAME_Label
[WM] Adding parameter AIO_KEY_Label
[WM] setAPStaticIPConfig
[WM] setSTAStaticIPConfig
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
[WM] WiFi.waitForConnectResult Done
[WM] SET AP_STA
[WM] Configuring AP SSID = ESP_702FF3
[WM] AP PWD = your_password
[WM] AP Channel = 3
[WM] Custom AP IP/GW/Subnet =  192.168.100.1 192.168.100.1 255.255.255.0
[WM] AP IP address = 192.168.100.1
[WM] HTTP server started
[WM] ESP_WiFiManager::startConfigPortal : Enter loop
```

#### 5.3 Config Portal Done

```
[WM] Connecting to new AP
[WM] Already connected. Bailing out.
Connected...yeey :)
Local IP: 192.168.2.166
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
[WM] getSTAStaticIPConfig
[WM] stationIP = 192.168.2.185 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
[WM] SaveWiFiCfgFile 
[WM] OK
Saving Config File
{
  "AIO_SERVER_Label": "io.adafruit.com",
  "AIO_SERVERPORT_Label": "1883",
  "AIO_USERNAME_Label": "user_name",
  "AIO_KEY_Label": "aio_key"
}
Config File successfully saved

Creating new WiFi client object : OK
Creating new MQTT object : OK
AIO_SERVER = io.adafruit.com, AIO_SERVERPORT = 1883
AIO_USERNAME = user_name, AIO_KEY = aio_key
Creating new MQTT_Pub_Topic,  Temperature = user_name/feeds/Temperature
Creating new Temperature object : OK
Temperature MQTT_Pub_Topic = user_name/feeds/Temperature
[WM] freeing allocated params!
WConnecting to MQTT (3 attempts)...
MQTT connection successful!
TWWWW WTWWWW WWTWWW WWWTWW WWWWTW WWWWW
```

---

#### 6. [ConfigOnDoubleReset](examples/ConfigOnDoubleReset) on ESP32S2_DEV

This is terminal debug output when running [ConfigOnDoubleReset](examples/ConfigOnDoubleReset) on **ESP32S2_DEV**.


```
Starting ConfigOnDoubleReset with DoubleResetDetect using LittleFS on ESP32S2_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
[WM] RFC925 Hostname = ConfigOnDoubleReset
[WM] setAPStaticIPConfig
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
ESP Self-Stored: SSID = HueNet1, Pass = 12345678
[WM] * Add SSID =  HueNet1 , PW =  12345678
Got ESP Self-Stored Credentials. Timeout 120s for Config Portal
LittleFS Flag read = 0xD0D04321
No doubleResetDetected
Saving config file...
Saving config file OK
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] stationIP = 0.0.0.0 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet2 , PW =  12345678
ConnectMultiWiFi in setup
[WM] ConnectMultiWiFi with :
[WM] * Flash-stored Router_SSID =  HueNet1 , Router_Pass =  12345678
[WM] * Additional SSID =  HueNet1 , PW =  12345678
[WM] * Additional SSID =  HueNet2 , PW =  12345678
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  1
[WM] SSID: HueNet1 ,RSSI= -35
[WM] Channel: 2 ,IP address: 192.168.2.157
After waiting 7.83 secs more in setup(), connection result is connected. Local IP: 192.168.2.157
[WM] freeing allocated params!
HStop doubleResetDetecting
Saving config file...
Saving config file OK
HHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH
HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH
HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH
HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH HHHHHHHHHH
```

---

#### 7. [ConfigOnDoubleReset](examples/ConfigOnDoubleReset) on ESP32_DEV

This is terminal debug output when running [ConfigOnDoubleReset](examples/ConfigOnDoubleReset) on **ESP32_DEV.**. Config Portal was requested by DRD to input and save Credentials. The boards then connected to WiFi using new Static IP successfully, with correct local time, TZ set and using NTP


#### 7.1 DRD => Config Portal

```
Starting ConfigOnDoubleReset with DoubleResetDetect using LittleFS on ESP32_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
[WM] RFC925 Hostname = ConfigOnDoubleReset
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
ESP Self-Stored: SSID = HueNet1, Pass = password
[WM] * Add SSID =  HueNet1 , PW =  password
Got ESP Self-Stored Credentials. Timeout 120s for Config Portal
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] stationIP = 192.168.2.232 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
Got stored Credentials. Timeout 120s for Config Portal
[WM] Current TZ_Name = America/New_York , TZ =  EST5EDT,M3.2.0,M11.1.0
LittleFS Flag read = 0xD0D01234
doubleResetDetected
Saving config file...
Saving config file OK
Open Config Portal without Timeout: Double Reset Detected
Starting configuration portal @ 192.168.4.1:80, SSID = ESP_9ABF498, PWD = MyESP_9ABF498
```

#### 7.2 Data Saved => Connect to WiFi with correct local time, TZ set and using NTP

```
[WM] WiFi.waitForConnectResult Done
[WM] SET AP
[WM] Configuring AP SSID = ESP_9ABF498
[WM] AP PWD = MyESP_9ABF498
[WM] AP Channel = 8
[WM] AP IP address = 192.168.4.1
[WM] HTTP server started
[WM] startConfigPortal : Enter loop
[WM] Handle root
[WM] captivePortal: hostHeader =  test.mosquitto.org
[WM] Request redirected to captive portal :  192.168.4.1
[WM] Handle root
[WM] captivePortal: hostHeader =  192.168.4.1
[WM] Info
[WM] Sent info page
[WM] Handle root
[WM] captivePortal: hostHeader =  192.168.4.1
[WM] Handle WiFi
[WM] Scanning Network
[WM] scanWifiNetworks: Done, Scanned Networks n = 8
[WM] Sorting
[WM] Removing Dup
[WM] Index = 0
[WM] SSID = HueNet
[WM] RSSI = -27
[WM] Index = 1
[WM] SSID = HueNet1
[WM] RSSI = -41
[WM] Index = 2
[WM] SSID = dragino-1ed63c
[WM] RSSI = -45
[WM] Index = 3
[WM] SSID = HueNetTek
[WM] RSSI = -49
[WM] Index = 4
[WM] SSID = HueNet2
[WM] RSSI = -52
[WM] Index = 5
[WM] SSID = bacau
[WM] RSSI = -67
[WM] Index = 6
[WM] SSID = guest_24
[WM] RSSI = -67
[WM] Index = 7
[WM] SSID = Jessie
[WM] RSSI = -85
[WM] Static IP = 0.0.0.0
[WM] Sent config page
[WM] WiFi save
[WM] TZ name = America/New_York
[WM] New Static IP = 0.0.0.0
[WM] New Static Gateway = 192.168.2.1
[WM] New Static Netmask = 255.255.255.0
[WM] New Static DNS1 = 192.168.2.1
[WM] New Static DNS2 = 8.8.8.8
[WM] Sent wifi save page
[WM] Connecting to new AP
[WM] Previous settings invalidated
[WM] Can't use Custom STA IP/GW/Subnet
[WM] Connect to new WiFi using new IP parameters
[WM] Connected after waiting (s) : 6.90
[WM] Local ip = 192.168.2.45
[WM] Connection result:  WL_CONNECTED
WiFi connected...yeey :)
[WM] * Add SSID =  HueNet1 , PW =  password
[WM] * Add SSID =  HueNet2 , PW =  password
[WM] Saving current TZ_Name = America/New_York , TZ =  EST5EDT,M3.2.0,M11.1.0
[WM] getSTAStaticIPConfig
[WM] SaveWiFiCfgFile 
[WM] stationIP = 0.0.0.0 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
[WM] OK
After waiting 0.00 secs more in setup(), connection result is connected. Local IP: 192.168.2.45
[WM] freeing allocated params!
Local Date/Time: Thu May  6 21:26:18 2021
Local Date/Time: Thu May  6 21:27:18 2021
Local Date/Time: Thu May  6 21:28:18 2021
Local Date/Time: Thu May  6 21:29:18 2021
```

#### 7.3 Normal running with correct local time, TZ set and using NTP

```
Starting ConfigOnDoubleReset with DoubleResetDetect using LittleFS on ESP32_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
[WM] RFC925 Hostname = ConfigOnDoubleReset
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
ESP Self-Stored: SSID = HueNet1, Pass = password
[WM] * Add SSID =  HueNet1 , PW =  password
Got ESP Self-Stored Credentials. Timeout 120s for Config Portal
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] stationIP = 192.168.2.232 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
Got stored Credentials. Timeout 120s for Config Portal
[WM] Current TZ_Name = America/New_York , TZ =  EST5EDT,M3.2.0,M11.1.0
LittleFS Flag read = 0xD0D04321
No doubleResetDetected
Saving config file...
Saving config file OK
[WM] * Add SSID =  HueNet1 , PW =  password
[WM] * Add SSID =  HueNet2 , PW =  password
ConnectMultiWiFi in setup
[WM] ConnectMultiWiFi with :
[WM] * Flash-stored Router_SSID =  HueNet1 , Router_Pass =  password
[WM] * Add SSID =  HueNet1 , PW =  password
[WM] * Additional SSID =  HueNet1 , PW =  password
[WM] * Additional SSID =  HueNet2 , PW =  password
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  1
[WM] SSID: HueNet1 ,RSSI= -46
[WM] Channel: 2 ,IP address: 192.168.2.45
After waiting 18.87 secs more in setup(), connection result is connected. Local IP: 192.168.2.45
[WM] freeing allocated params!
Stop doubleResetDetecting
Saving config file...
Saving config file OK
Local Date/Time: Thu May  6 21:31:18 2021
Local Date/Time: Thu May  6 21:32:18 2021
Local Date/Time: Thu May  6 21:33:18 2021
```

---

#### 8. [ConfigOnDoubleReset](examples/ConfigOnDoubleReset) on ESP32S2_DEV

This is terminal debug output when running [ConfigOnDoubleReset](examples/ConfigOnDoubleReset) on **ESP32S2_DEV.**. Config Portal was requested by DRD to input and save Credentials. The boards then connected to WiFi using new Static IP successfully, with correct local time, TZ set and using NTP


#### 8.1 No Data => Config Portal

```
Starting ConfigOnDoubleReset with DoubleResetDetect using LittleFS on ESP32S2_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
[WM] RFC925 Hostname = ConfigOnDoubleReset
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
ESP Self-Stored: SSID = HueNet1, Pass = password
[WM] * Add SSID =  HueNet1 , PW =  password
Got ESP Self-Stored Credentials. Timeout 120s for Config Portal
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] WM_config checksum wrong
Open Config Portal without Timeout: No stored Credentials.
LittleFS Flag read = 0xD0D04321
No doubleResetDetected
Saving config file...
Saving config file OK
Starting configuration portal @ 192.168.4.1:80, SSID = ESP_8A1DF7C, PWD = MyESP_8A1DF7C
```

#### 8.2 Data Saved => Connect to WiFi with correct local time, TZ set and using NTP

```
[WM] WiFi.waitForConnectResult Done
[WM] SET AP
[WM] Configuring AP SSID = ESP_8A1DF7C
[WM] AP PWD = MyESP_8A1DF7C
[WM] AP Channel = 4
[WM] AP IP address = 192.168.4.1
[WM] HTTP server started
[WM] startConfigPortal : Enter loop
[WM] Handle root
[WM] captivePortal: hostHeader =  192.168.4.1
[WM] Info
[WM] Sent info page
[WM] Handle root
[WM] captivePortal: hostHeader =  192.168.4.1
[WM] Handle WiFi
[WM] Scanning Network
[WM] scanWifiNetworks: Done, Scanned Networks n = 23
[WM] Sorting
[WM] Removing Dup
[WM] DUP AP: Waterhome
[WM] DUP AP: Waterhome
[WM] DUP AP: Access 2.0
[WM] Index = 0
[WM] SSID = HueNet
[WM] RSSI = -20
[WM] Index = 1
[WM] SSID = HueNet1
[WM] RSSI = -28
[WM] Index = 2
[WM] SSID = HueNetTek
[WM] RSSI = -31
[WM] Index = 3
[WM] SSID = dragino-1ed63c
[WM] RSSI = -43
[WM] Index = 4
[WM] SSID = HueNet2
[WM] RSSI = -47
[WM] Index = 5
[WM] SSID = bacau
[WM] RSSI = -67
[WM] Index = 6
[WM] SSID = guest_24
[WM] RSSI = -67
[WM] Index = 7
[WM] SSID = rogers786
[WM] RSSI = -72
[WM] Index = 8
[WM] SSID = dlink-4F96
[WM] RSSI = -81
[WM] Index = 9
[WM] SSID = Waterhome
[WM] RSSI = -81
[WM] Index = 10
[WM] SSID = VIRGIN874
[WM] RSSI = -83
[WM] Index = 11
[WM] SSID = elef
[WM] RSSI = -84
[WM] Index = 12
[WM] SSID = Access 2.0
[WM] RSSI = -85
[WM] Index = 14
[WM] SSID = Jasmine
[WM] RSSI = -86
[WM] Index = 16
[WM] SSID = FishBowl
[WM] RSSI = -87
[WM] Index = 17
[WM] SSID = FishTank
[WM] RSSI = -87
[WM] Index = 19
[WM] SSID = BELL246
[WM] RSSI = -89
[WM] Index = 20
[WM] SSID = BAHFAMILY
[WM] RSSI = -89
[WM] Index = 21
[WM] SSID = madda
[WM] RSSI = -91
[WM] Index = 22
[WM] SSID = BELL040
[WM] RSSI = -91
[WM] Static IP = 0.0.0.0
[WM] Sent config page
[WM] WiFi save
[WM] TZ name = America/New_York
[WM] New Static IP = 0.0.0.0
[WM] New Static Gateway = 192.168.2.1
[WM] New Static Netmask = 255.255.255.0
[WM] New Static DNS1 = 192.168.2.1
[WM] New Static DNS2 = 8.8.8.8
[WM] Sent wifi save page
[WM] Connecting to new AP
[WM] Previous settings invalidated
[WM] Can't use Custom STA IP/GW/Subnet
[WM] Connect to new WiFi using new IP parameters
[WM] Connected after waiting (s) : 6.80
[WM] Local ip = 192.168.2.157
[WM] Connection result:  WL_CONNECTED
WiFi connected...yeey :)
[WM] * Add SSID =  HueNet1 , PW =  password
[WM] * Add SSID =  HueNet2 , PW =  password
[WM] Saving current TZ_Name = America/New_York , TZ =  EST5EDT,M3.2.0,M11.1.0
[WM] getSTAStaticIPConfig
[WM] SaveWiFiCfgFile 
[WM] stationIP = 0.0.0.0 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
[WM] OK
After waiting 0.00 secs more in setup(), connection result is connected. Local IP: 192.168.2.157
[WM] freeing allocated params!
Stop doubleResetDetecting
Saving config file...
Saving config file OK

Local Date/Time: Thu May  6 21:26:18 2021
Local Date/Time: Thu May  6 21:27:18 2021
Local Date/Time: Thu May  6 21:28:18 2021
Local Date/Time: Thu May  6 21:29:18 2021
```

#### 8.3 Normal running with correct local time, TZ set and using NTP

```
Starting ConfigOnDoubleReset with DoubleResetDetect using LittleFS on ESP32S2_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
[WM] RFC925 Hostname = ConfigOnDoubleReset
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
ESP Self-Stored: SSID = HueNet1, Pass = password
[WM] * Add SSID =  HueNet1 , PW =  password
Got ESP Self-Stored Credentials. Timeout 120s for Config Portal
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] stationIP = 0.0.0.0 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
Got stored Credentials. Timeout 120s for Config Portal
[WM] Current TZ_Name = America/New_York , TZ =  EST5EDT,M3.2.0,M11.1.0
LittleFS Flag read = 0xD0D04321
No doubleResetDetected
Saving config file...
Saving config file OK
[WM] * Add SSID =  HueNet1 , PW =  password
[WM] * Add SSID =  HueNet2 , PW =  password
ConnectMultiWiFi in setup
[WM] ConnectMultiWiFi with :
[WM] * Flash-stored Router_SSID =  HueNet1 , Router_Pass =  password
[WM] * Add SSID =  HueNet1 , PW =  password
[WM] * Additional SSID =  HueNet1 , PW =  password
[WM] * Additional SSID =  HueNet2 , PW =  password
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  1
[WM] SSID: HueNet1 ,RSSI= -27
[WM] Channel: 2 ,IP address: 192.168.2.157
After waiting 7.72 secs more in setup(), connection result is connected. Local IP: 192.168.2.157
[WM] freeing allocated params!
Local Date/Time: Wed Dec 31 19:00:13 1969
Stop doubleResetDetecting
Saving config file...
Saving config file OK
Local Date/Time: Thu May  6 21:47:41 2021
Local Date/Time: Thu May  6 21:47:51 2021
Local Date/Time: Thu May  6 21:48:01 2021
Local Date/Time: Thu May  6 21:48:11 2021
Local Date/Time: Thu May  6 21:48:21 2021
Local Date/Time: Thu May  6 21:48:31 2021
Local Date/Time: Thu May  6 21:48:41 2021
```

---

#### 9. [ESP32_FSWebServer_DRD](examples/ESP32_FSWebServer_DRD) on ESP32C3_DEV using SPIFFS

This is terminal debug output when running [ESP32_FSWebServer_DRD](examples/ESP32_FSWebServer_DRD) on **ESP32C3_DEV using SPIFFS and ESP32 core v2.0.0-rc1.**. Config Portal was requested by DRD to input and save Credentials. The boards then connected to WiFi AP **HueNet** using new Static IP successfully.

```
Starting ESP32_FSWebServer_DRD with DoubleResetDetect using SPIFFS on ESP32C3_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
FS File: wm_cp.dat, size: 4B
FS File: wm_cp.bak, size: 4B
FS File: wmssl_conf.dat, size: 376B
FS File: wmssl_conf.bak, size: 376B
FS File: wmssl_cred.dat, size: 180B
FS File: wmssl_cred.bak, size: 180B
FS File: mrd.dat, size: 4B
FS File: wifi_cred.dat, size: 334B
FS File: CanadaFlag_1.png, size: 40.25KB
FS File: CanadaFlag_2.png, size: 8.12KB
FS File: CanadaFlag_3.jpg, size: 10.89KB
FS File: edit.htm.gz, size: 4.02KB
FS File: favicon.ico, size: 1.12KB
FS File: graphs.js.gz, size: 1.92KB
FS File: index.htm, size: 3.63KB
FS File: drd.dat, size: 4B

[WM] RFC925 Hostname = ESP32-FSWebServerDRD
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
ESP Self-Stored: SSID = HueNet, Pass = 12345678
[WM] * Add SSID =  HueNet , PW =  12345678
Got ESP Self-Stored Credentials. Timeout 120s for Config Portal
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] stationIP = 0.0.0.0 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
Got stored Credentials. Timeout 120s for Config Portal
[WM] Current TZ_Name = America/New_York , TZ =  EST5EDT,M3.2.0,M11.1.0
SPIFFS Flag read = 0xD0D04321
No doubleResetDetected
Saving config file...
Saving config file OK
[WM] * Add SSID =  HueNet1 , PW =  12345678
[WM] * Add SSID =  HueNet , PW =  12345678
ConnectMultiWiFi in setup
[WM] ConnectMultiWiFi with :
[WM] * Flash-stored Router_SSID =  HueNet , Router_Pass =  12345678
[WM] * Add SSID =  HueNet , PW =  12345678
[WM] * Additional SSID =  HueNet1 , PW =  12345678
[WM] * Additional SSID =  HueNet , PW =  12345678
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  4
[WM] SSID: HueNet ,RSSI= -27
[WM] Channel: 10 ,IP address: 192.168.2.99
After waiting 12.34 secs more in setup(), connection result is connected. Local IP: 192.168.2.99
HTTP server started @ 192.168.2.99
Open http://esp32-fs-browser.local/edit to see the file browser
[WM] freeing allocated params!
Stop doubleResetDetecting
Saving config file...
Saving config file OK
handleFileRead: /edit.htm
handleFileRead: /index.htm
handleFileList: /
handleFileList: [{"type":"file","name":"wm_cp.dat"},{"type":"file","name":"wm_cp.bak"},{"type":"file","name":"wmssl_conf.dat"},{"type":"file","name":"wmssl_conf.bak"},{"type":"file","name":"wmssl_cred.dat"},{"type":"file","name":"wmssl_cred.bak"},{"type":"file","name":"mrd.dat"},{"type":"file","name":"wifi_cred.dat"},{"type":"file","name":"CanadaFlag_1.png"},{"type":"file","name":"CanadaFlag_2.png"},{"type":"file","name":"CanadaFlag_3.jpg"},{"type":"file","name":"edit.htm.gz"},{"type":"file","name":"favicon.ico"},{"type":"file","name":"graphs.js.gz"},{"type":"file","name":"index.htm"},{"type":"file","name":"drd.dat"}]
handleFileRead: /CanadaFlag_1.png
Local Date/Time: Thu Jul 29 23:22:53 2021
Local Date/Time: Thu Jul 29 23:23:53 2021
```

---

#### 10. [ConfigOnDoubleReset](examples/ConfigOnDoubleReset) on ESP32S3_DEV

This is terminal debug output when running [ConfigOnDoubleReset](examples/ConfigOnDoubleReset) on **ESP32S3_DEV**.


```
Starting ConfigOnDoubleReset with DoubleResetDetect using LittleFS on ESP32S3_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
[WM] RFC925 Hostname = ConfigOnDoubleReset
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
ESP Self-Stored: SSID = HueNet1, Pass = password
[WM] * Add SSID =  HueNet1 , PW =  password
Got ESP Self-Stored Credentials. Timeout 120s for Config Portal
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] stationIP = 0.0.0.0 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
Got stored Credentials. Timeout 120s for Config Portal
[WM] Current TZ_Name = America/New_York , TZ =  EST5EDT,M3.2.0,M11.1.0
LittleFS Flag read = 0xD0D04321
No doubleResetDetected
Saving config file...
Saving config file OK
[WM] * Add SSID =  HueNet1 , PW =  password
[WM] * Add SSID =  HueNet , PW =  password
ConnectMultiWiFi in setup
[WM] ConnectMultiWiFi with :
[WM] * Flash-stored Router_SSID =  HueNet1 , Router_Pass =  password
[WM] * Add SSID =  HueNet1 , PW =  password
[WM] * Additional SSID =  HueNet1 , PW =  password
[WM] * Additional SSID =  HueNet , PW =  password
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  1
[WM] SSID: HueNet1 ,RSSI= -23
[WM] Channel: 2 ,IP address: 192.168.2.83
After waiting 7.92 secs more in setup(), connection result is connected. Local IP: 192.168.2.83
[WM] freeing allocated params!
Stop doubleResetDetecting
Saving config file...
Saving config file OK
Local Date/Time: Thu Feb 10 23:15:26 2022
Local Date/Time: Thu Feb 10 23:16:26 2022
Local Date/Time: Thu Feb 10 23:17:26 2022
Local Date/Time: Thu Feb 10 23:18:26 2022
Local Date/Time: Thu Feb 10 23:19:26 2022
Local Date/Time: Thu Feb 10 23:20:26 2022
Local Date/Time: Thu Feb 10 23:21:26 2022
Local Date/Time: Thu Feb 10 23:22:26 2022
Local Date/Time: Thu Feb 10 23:23:26 2022
Local Date/Time: Thu Feb 10 23:24:26 2022
Local Date/Time: Thu Feb 10 23:25:26 2022
Local Date/Time: Thu Feb 10 23:26:26 2022
```

---

#### 11. [ConfigOnDoubleReset](examples/ConfigOnDoubleReset) using LittleFS on ESP32C3_DEV

This is terminal debug output when running [ConfigOnDoubleReset](examples/ConfigOnDoubleReset) on **ESP32C3_DEV**.


```
Starting ConfigOnDoubleReset with DoubleResetDetect using LittleFS on ESP32C3_DEV
ESP_WiFiManager v1.10.2
ESP_DoubleResetDetector v1.3.1
[WM] RFC925 Hostname = ConfigOnDoubleReset
[WM] Set CORS Header to :  Your Access-Control-Allow-Origin
ESP Self-Stored: SSID = HueNet1, Pass = password
[WM] * Add SSID =  HueNet1 , PW =  password
Got ESP Self-Stored Credentials. Timeout 120s for Config Portal
[WM] LoadWiFiCfgFile 
[WM] OK
[WM] stationIP = 0.0.0.0 , gatewayIP = 192.168.2.1
[WM] netMask = 255.255.255.0
[WM] dns1IP = 192.168.2.1 , dns2IP = 8.8.8.8
Got stored Credentials. Timeout 120s for Config Portal
[WM] Current TZ_Name = America/New_York , TZ =  EST5EDT,M3.2.0,M11.1.0
LittleFS Flag read = 0xD0D04321
No doubleResetDetected
Saving config file...
Saving config file OK
[WM] * Add SSID =  HueNet1 , PW =  password
[WM] * Add SSID =  HueNet2 , PW =  password
ConnectMultiWiFi in setup
[WM] ConnectMultiWiFi with :
[WM] * Flash-stored Router_SSID =  HueNet1 , Router_Pass =  password
[WM] * Add SSID =  HueNet1 , PW =  password
[WM] * Additional SSID =  HueNet1 , PW =  password
[WM] * Additional SSID =  HueNet2 , PW =  password
[WM] Connecting MultiWifi...
[WM] WiFi connected after time:  1
[WM] SSID: HueNet1 ,RSSI= -20
[WM] Channel: 2 ,IP address: 192.168.2.85
After waiting 8.31 secs more in setup(), connection result is connected. Local IP: 192.168.2.85
[WM] freeing allocated params!
Stop doubleResetDetecting
Saving config file...
Saving config file OK
Local Date/Time: Fri Feb 11 18:40:46 2022
Local Date/Time: Fri Feb 11 18:41:46 2022
Local Date/Time: Fri Feb 11 18:42:46 2022
Local Date/Time: Fri Feb 11 18:43:46 2022
```

---
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
---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the `ESP32 / ESP8266` core for Arduino.

Sometimes, the library will only work if you update the `ESP32 / ESP8266` core to the latest version because I am using some newly added function.

If you connect to the created configuration Access Point but the ConfigPortal does not show up, just open a browser and type in the IP of the web portal, by default `192.168.4.1`.

---

### Issues ###

Submit issues to: [ESP_WiFiManager issues](https://github.com/khoih-prog/ESP_WiFiManager/issues)


---
---

### Contributions and Thanks

 1. Based on and modified from [Tzapu](https://github.com/tzapu/WiFiManager) and [KenTaylor's version]( https://github.com/kentaylor/WiFiManager)
 2. Thanks to [Amorphous](https://community.blynk.cc/t/esp-wifimanager-for-esp32-and-esp8266/42257/13) for the static DNS feature and code, included in v1.0.5
 3. Thanks to [CrispinP](https://github.com/CrispinP) for idea to add HostName (v1.0.4) and request to reduce the unnecessary waiting time in ESP_WiFiManager constructor (v1.0.6+). See [Starting WiFIManger is very slow (2000ms)](https://github.com/khoih-prog/ESP_WiFiManager/issues/6)
 4. Thanks to [OttoKlaasen](https://github.com/OttoKlaasen) for reporting [Having issue to read the SPIFF file](https://github.com/khoih-prog/ESP_WiFiManager/issues/14) bug in examples.
 5. Thanks to [Giuseppe](https://github.com/Gius-8) for reporting [Static Station IP doesn't work](https://github.com/khoih-prog/ESP_WiFiManager/issues/17) bug.
 6. Thanks to [AlesSt](https://github.com/AlesSt) for reporting then help provide the fixes:
  - [On Android phone ConfigPortal is unresponsive](https://github.com/khoih-prog/ESP_WiFiManager/issues/23) and request an enhancement (**HOWTO disable the static IP inputs on the config page**) leading to [ESP_WiFiManager v1.0.10](https://github.com/khoih-prog/ESP_WiFiManager/releases/tag/v1.0.10).
  - [Issue #25: API call /r doesnt clear credentials](https://github.com/khoih-prog/ESP_WiFiManager/issues/25), [Issue #26: softAP with custom IP not working](https://github.com/khoih-prog/ESP_WiFiManager/issues/26) and [Issue #27: CORS protection fires up with AJAX](https://github.com/khoih-prog/ESP_WiFiManager/issues/27) leading to [ESP_WiFiManager v1.0.11](https://github.com/khoih-prog/ESP_WiFiManager/releases/tag/v1.0.11).
 7. Thanks to [Marko](https://github.com/wackoo-arduino) for agreeing to contribute the sample code dealing with MQTT which the  [ConfigOnSwitchFS_MQTT_Ptr](examples/ConfigOnSwitchFS_MQTT_Ptr) is based on. See [Custom MQTT parameters using Wifi Manager](https://forum.arduino.cc/index.php?topic=692108.75).
 8. Thanks to [05prateek](https://github.com/05prateek) for reporting [Stationmode Static IP changes to dhcp when esp8266 is restarted](https://github.com/khoih-prog/ESP_WiFiManager/issues/28) bug which is fixed in v1.0.11 by enhance autoConnect() function.
 9. Thanks to [Egor](https://github.com/eg321) and [HenrikW](https://github.com/Invento3D) to make [`Support building in PlatformIO PR`](https://github.com/khoih-prog/ESP_WiFiManager/pull/20) and post issue [`Change Implementation to seperate *.h and *.cpp file instead of *.h and *-Impl.h`](https://github.com/khoih-prog/ESP_WiFiManager/issues/38) to address the `multiple definition` linker error in certain cases, leading to v1.2.0
10. Thanks to [Maurice Poos](https://github.com/MaPoLom) to report issue [`ESP_WiFiManager Issue 39: Not able to read analog port when using the autoconnect example`](https://github.com/khoih-prog/ESP_WiFiManager/issues/39).
11. Thanks to [Vague Rabbit](https://github.com/thewhiterabbit) for requesting, collarborating in creating the [**HOWTO Add Dynamic Parameters**](#howto-add-dynamic-parameters).
12. Thanks to [Roshan](https://github.com/solroshan) to report the issue in [Error esp_littlefs.c 'utime_p'](https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/28) to fix PIO error in using ESP32 LittleFS with old [`LittleFS_esp32 v1.0`](https://github.com/lorol/LITTLEFS)
13. Thanks to [yiancar](https://github.com/yiancar) to report the issue and propose a fix in [In AP, DNS server always redirects to 192.168.4.1 no mater what APStaticIP is set to. #58](https://github.com/khoih-prog/ESP_WiFiManager/issues/58) leading to v1.6.1
14. Thanks to [Stephen Lavelle](https://github.com/increpare) and [Ben Peart](https://github.com/benpeart) for requesting enhancement in [_timezoneName never getting set? #51](https://github.com/khoih-prog/ESP_WiFiManager/issues/51) and [How to retrieve timezone? #51](https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/51) leading to new v1.7.0
15. Thanks to [energise](https://github.com/energise) to report the issue in [Invalid Json generated #60](https://github.com/khoih-prog/ESP_WiFiManager/issues/60) leading to new v1.7.1
16. Thanks to [eth0up](https://github.com/eth0up) to make the PR [Add support for Wifi hidden SSID scanning. #66](https://github.com/khoih-prog/ESP_WiFiManager/pull/66) leading to v1.7.4
17. Thanks to [Dean Ott](https://github.com/deanjott) for reporting [WiFiManager works only on port 80 #75](https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/75) and providing the solution leading to v1.7.8
18. Thanks to [Twaste](https://github.com/Twaste) for initiating the discussion in [Different behaviour using the src_cpp or src_h lib #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/discussions/80) and providing the idea to the solution, to fix `multiple-definitions` linker error, leading to v1.8.0
19. Thanks to [Richard Hawthorn](https://github.com/richardhawthorn) for reporting [Cors header not sent when saving wifi details, even when cors is enabled #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/80) and providing the solution leading to v1.10.2

---

<table>
  <tr>
    <td align="center"><a href="https://github.com/Tzapu"><img src="https://github.com/Tzapu.png" width="100px;" alt="Tzapu"/><br /><sub><b>⭐️ Tzapu</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/kentaylor"><img src="https://github.com/kentaylor.png" width="100px;" alt="kentaylor"/><br /><sub><b>⭐️ Ken Taylor</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/Amorphous"><img src="https://github.com/Amorphous.png" width="100px;" alt="Amorphous"/><br /><sub><b>⭐️ Amorphous</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/CrispinP"><img src="https://github.com/CrispinP.png" width="100px;" alt="CrispinP"/><br /><sub><b>CrispinP</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/OttoKlaasen"><img src="https://github.com/OttoKlaasen.png" width="100px;" alt="OttoKlaasen"/><br /><sub><b>Otto Klaasen</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/Gius-8"><img src="https://github.com/Gius-8.png" width="100px;" alt="Giuseppe"/><br /><sub><b>Giuseppe</b></sub></a><br /></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/AlesSt"><img src="https://github.com/AlesSt.png" width="100px;" alt="AlesSt"/><br /><sub><b>⭐️ AlesSt</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/wackoo-arduino"><img src="https://github.com/wackoo-arduino.png" width="100px;" alt="wackoo-arduino"/><br /><sub><b>Marko</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/05prateek"><img src="https://github.com/05prateek.png" width="100px;" alt="05prateek"/><br /><sub><b>05prateek</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/eg321"><img src="https://github.com/eg321.png" width="100px;" alt="eg321"/><br /><sub><b>Egor</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/Invento3D"><img src="https://github.com/Invento3D.png" width="100px;" alt="Invento3D"/><br /><sub><b>HenrikW</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/MaPoLom"><img src="https://github.com/MaPoLom.png" width="100px;" alt="MaPoLom"/><br /><sub><b>Maurice Poos</b></sub></a><br /></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/thewhiterabbit"><img src="https://github.com/thewhiterabbit.png" width="100px;" alt="thewhiterabbit"/><br /><sub><b>Vague Rabbit</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/solroshan"><img src="https://github.com/solroshan.png" width="100px;" alt="solroshan"/><br /><sub><b>Roshan</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/yiancar"><img src="https://github.com/yiancar.png" width="100px;" alt="yiancar"/><br /><sub><b>yiancar</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/increpare"><img src="https://github.com/increpare.png" width="100px;" alt="increpare"/><br /><sub><b>Stephen Lavelle</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/benpeart"><img src="https://github.com/benpeart.png" width="100px;" alt="benpeart"/><br /><sub><b>Ben Peart</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/energise"><img src="https://github.com/energise.png" width="100px;" alt="energise"/><br /><sub><b>energise</b></sub></a><br /></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/eth0up"><img src="https://github.com/eth0up.png" width="100px;" alt="eth0up"/><br /><sub><b>eth0up</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/deanjott"><img src="https://github.com/deanjott.png" width="100px;" alt="deanjott"/><br /><sub><b>Dean Ott</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/Twaste"><img src="https://github.com/Twaste.png" width="100px;" alt="Twaste"/><br /><sub><b>Twaste</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/richardhawthorn"><img src="https://github.com/richardhawthorn.png" width="100px;" alt="richardhawthorn"/><br /><sub><b>Richard Hawthorn</b></sub></a><br /></td>
  </tr>
</table>

---

### Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License and credits ###

- The library is licensed under [MIT](https://github.com/khoih-prog/ESP_WiFiManager/blob/master/LICENSE)

---

## Copyright

Copyright 2019- Khoi Hoang


