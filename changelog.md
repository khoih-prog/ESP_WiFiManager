# ESP_WiFiManager

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_WiFiManager.svg?)](https://www.ardu-badge.com/ESP_WiFiManager)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP_WiFiManager.svg)](https://github.com/khoih-prog/ESP_WiFiManager/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP_WiFiManager/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP_WiFiManager.svg)](http://github.com/khoih-prog/ESP_WiFiManager/issues)
[![star this repo](https://githubbadges.com/star.svg?user=khoih-prog&repo=ESP_WiFiManager&style=default)](https://github.com/khoih-prog/ESP_WiFiManager)

---
---

## Table of Contents

* [Changelog](#changelog)
  * [Releases v1.12.1](#releases-v1121)
  * [Releases v1.12.0](#releases-v1120)
  * [Releases v1.11.0](#releases-v1110)
  * [Releases v1.10.2](#releases-v1102)
  * [Releases v1.10.1](#releases-v1101)
  * [Releases v1.10.0](#releases-v1100)
  * [Releases v1.9.0](#releases-v190)
  * [Releases v1.8.0](#releases-v180)
  * [Releases v1.7.8](#releases-v178)
  * [Releases v1.7.7](#releases-v177)
  * [Releases v1.7.6](#releases-v176)
  * [Releases v1.7.5](#releases-v175)
  * [Releases v1.7.4](#releases-v174)
  * [Releases v1.7.3](#releases-v173)
  * [Releases v1.7.2](#releases-v172)
  * [Releases v1.7.1](#releases-v171)
  * [Major Releases v1.7.0](#major-releases-v170)
  * [Releases v1.6.1](#releases-v161)
  * [Releases v1.6.0](#releases-v160)
  * [Releases v1.5.3](#releases-v153)
  * [Releases v1.5.2](#releases-v152)
  * [Releases v1.5.1](#releases-v151)
  * [Releases v1.5.0](#releases-v150)
  * [Releases v1.4.3](#releases-v143)
  * [Releases v1.4.2](#releases-v142)
  * [Major Releases v1.4.1](#major-releases-v141)
  * [Releases v1.3.0](#releases-v130)
  * [Releases v1.2.0](#releases-v120)
  * [Releases v1.1.2](#releases-v112)
  * [Releases v1.1.1](#releases-v111)
  * [Major Releases v1.1.0](#major-releases-v110)
  * [Releases v1.0.11](#releases-v1011)
  * [Releases v1.0.10](#releases-v1010)
  * [Releases v1.0.9](#releases-v109)
  * [Releases v1.0.8](#releases-v108)
  * [Releases v1.0.7](#releases-v107)
  * [Releases v1.0.6](#releases-v106)

---
---


## Changelog

### Releases v1.12.1

1. Using random channel for softAP without password. Check [fix: using random CH for non-password use too #118](https://github.com/khoih-prog/ESPAsync_WiFiManager/pull/118)
2. Add astyle using `allman` style. Restyle the library

### Releases v1.12.0

1. Optionally display Credentials (SSIDs, PWDs) in Config Portal. Check [Populate portal wifi with saved credentials #91](https://github.com/khoih-prog/ESP_WiFiManager/discussions/91)
2. Display `Credentials` Hint on Config Portal
3. Periodic code clean-up

### Releases v1.11.0

1. Fix ESP32 chipID. Check [Help for storing variables in memory (non-volatile) #87](https://github.com/khoih-prog/ESP_WiFiManager/discussions/87#discussioncomment-3593028)
2. Add ESP32 ESP_getChipOUI() function
3. Display new info on Config Portal for ESP32
4. Remove dependency on `LittleFS_esp32` library to prevent PIO error when using new ESP32 core v1.0.6+

### Releases v1.10.2

1. Send CORS header in handleWifiSave() function. Check [Cors header not sent when saving wifi details, even when cors is enabled #80](https://github.com/khoih-prog/ESP_WiFiManager/issues/80)
2. Optimize code by using passing by `reference` instead of by `value`
3. Delete all confusing, function-lacking `minimal` examples
4. Display informational warnings only when `_WIFIMGR_LOGLEVEL_ > 3`

### Releases v1.10.1

1. Add LittleFS support to `ESP32-C3`.
2. Use ESP32-core's LittleFS library instead of Lorol's LITTLEFS library for v2.0.0+

### Releases v1.10.0

1. Add support to `ESP32-S3` (`ESP32S3_DEV, ESP32_S3_BOX, UM TINYS3, UM PROS3, UM FEATHERS3`, etc.) using [ESP32 core, esp32-s3-support branch, v2.0.2+](https://github.com/espressif/arduino-esp32/tree/esp32-s3-support)

### Releases v1.9.0

1. Reduce the breaking effect of v1.8.0 by enabling compatibility with old code to include only `ESP_WiFiManager.h`. Check [Important Breaking Change from v1.8.0](https://github.com/khoih-prog/ESP_WiFiManager#Important-Breaking-Change-from-v180)


### Releases v1.8.0

1. Fix `multiple-definitions` linker error and weird bug related to `src_cpp`. Check [Different behaviour using the src_cpp or src_h lib #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/discussions/80)
2. Optimize library code by using `reference-passing` instead of `value-passing`

### Releases v1.7.8

1. To permit using HTTP port different from 80. Check [WiFiManager works only on port 80 #75](https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/75)
2. Fix bug returning IP `255.255.255.255` in core v2.0.0+ when using `hostname`


### Releases v1.7.7

1. Fix compile error for ESP32 core v1.0.5-

### Releases v1.7.6

1. Auto detect ESP32 core and use either built-in LittleFS or [LITTLEFS](https://github.com/lorol/LITTLEFS) library
2. Update `library.json` to use new `headers` for PIO

### Releases v1.7.5

1. Update `platform.ini` and `library.json` to use original `khoih-prog` instead of `khoih.prog` after PIO fix

### Releases v1.7.4

1. Add WiFi scanning of hidden SSIDs. Check [Add support for Wifi hidden SSID scanning. #66](https://github.com/khoih-prog/ESP_WiFiManager/pull/66)

### Releases v1.7.3

1. Fix MultiWiFi connection issue with ESP32 core v2.0.0-rc1+
2. Verify compatibility with new ESP32 core v2.0.0-rc1+
3. Verify compatibility with new ESP8266 core v3.0.2

### Releases v1.7.2

1. Fix warnings and Verify compatibility with new ESP8266 core v3.0.0

### Releases v1.7.1

1. Fix Json bug in /scan. Check [Invalid Json generated #60](https://github.com/khoih-prog/ESP_WiFiManager/issues/60)
2. Fix timezoneName not displayed in Info page.

### Major Releases v1.7.0

1. Add auto-Timezone feature with variable `_timezoneName` (e.g. `America/New_York`) and function to retrieve TZ (e.g. `EST5EDT,M3.2.0,M11.1.0`) to use directly to configure ESP32/ESP8266 timezone. Check [How to retrieve timezone? #51](https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/51) for more info.
2. Store those `_timezoneName` and `TZ` in LittleFS or SPIFFS config file.
3. Using these new timezone feature is optional.
4. Add checksum in config file to validate data read from LittleFS or SPIFFS config file.
5. Update examples to show how to use the new TZ feature.

### Releases v1.6.1

1. Fix captive-portal bug if Config Portal AP address is not default 192.168.4.1. Check [In AP, DNS server always redirects to 192.168.4.1 no mater what APStaticIP is set to. #58](https://github.com/khoih-prog/ESP_WiFiManager/issues/58)
2. Fix MultiWiFi bug.

### Releases v1.6.0

1. Add **EEPROM and SPIFFS** support to new **ESP32-C3** boards (**Arduino ESP32C3_DEV**). Check [HOWTO Install esp32 core for ESP32-S2 (Saola, AI-Thinker ESP-12K) and ESP32-C3 boards into Arduino IDE](#howto-install-esp32-core-for-esp32-s2-saola-ai-thinker-esp-12k-and-esp32-c3-boards-into-arduino-ide).


### Releases v1.5.3

1. Add `dnsServer can't allocate` and `dnsServer can't start` error messages

### Releases v1.5.2

1. Fix example misleading messages. Check [**Minor: examples/Async_ESP32_FSWebServer/ wrongly uses FileFS.begin(true)** #47](https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/47)
2. Tested with [**ESP32 Core 1.0.6**](https://github.com/espressif/arduino-esp32) and [**LittleFS_esp32 v1.0.6**](https://github.com/lorol/LITTLEFS)

### Releases v1.5.1

1. Fix compiler error if setting `Compiler Warnings` to `All`. 
2. Modify multiWiFi-related timings to work with latest esp32 core v1.0.6

### Releases v1.5.0

1. Add support to **ESP32-S2 (ESP32-S2 Saola, AI-Thinker ESP-12K, ESP32S2 Dev Module, UM FeatherS2, UM ProS2, microS2, etc.)**
2. Add [**Instructions to install ESP32-S2 core**](#howto-install-esp32-s2-core-for-esp32-s2-saola-ai-thinker-esp-12k-boards-into-arduino-ide)

### Releases v1.4.3

1. Fix examples' bug not saving Static IP in certain cases.
2. Add feature to warn if using examples with old library versions

### Releases v1.4.2

1. Fix examples' bug not using saved WiFi Credentials after losing all WiFi connections.
2. Fix compiler warnings.

### Major Releases v1.4.1

1. Fix staticIP not saved in examples. See [ESP32 static IP not saved after restarting the device](https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/19)
2. Add structures and functions to handle AP and STA IPs.
3. Add complex examples 
  * [ConfigOnDRD_FS_MQTT_Ptr_Complex](examples/ConfigOnDRD_FS_MQTT_Ptr_Complex) to demo usage of std::map
  * [ConfigOnDRD_FS_MQTT_Ptr_Medium](examples/ConfigOnDRD_FS_MQTT_Ptr_Medium).
4. Add simple minimal examples 
  * [ConfigOnDRD_ESP32_minimal](examples/ConfigOnDRD_ESP32_minimal)
  * [ConfigOnDRD_ESP8266_minimal](examples/ConfigOnDRD_ESP8266_minimal)
  * [AutoConnect_ESP32_minimal](examples/AutoConnect_ESP32_minimal)
  * [AutoConnect_ESP8266_minimal](examples/AutoConnect_ESP8266_minimal)
5. Fix bug.
6. Fix compiler warnings.
7. Modify Version String
8. Add Table of Contents


### Releases v1.3.0

1. Add LittleFS support to ESP32-related examples to use [LittleFS_esp32 Library](https://github.com/lorol/LITTLEFS)
2. Add Version String

### Releases v1.2.0

1. Restore cpp code besides Impl.h code to use in case of `multiple definition` linker error. See [`Change Implementation to seperate *.h and *.cpp file instead of *.h and *-Impl.h`](https://github.com/khoih-prog/ESP_WiFiManager/issues/38) and [`Support building in PlatformIO PR`](https://github.com/khoih-prog/ESP_WiFiManager/pull/20). Also have a look at [**HOWTO Fix Multiple Definitions Linker Error**](https://github.com/khoih-prog/ESP_WiFiManager#HOWTO-Fix-Multiple-Definitions-Linker-Error)
2. Fix bug [/close does not close the config portal](https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/16).

### Releases v1.1.2

1. Fix bug in examples.
2. Add example.

### Releases v1.1.1

1. Add setCORSHeader function to allow **configurable CORS Header**. See [Using CORS feature](https://github.com/khoih-prog/ESP_WiFiManager#15-using-cors-cross-origin-resource-sharing-feature)
2. Fix typo and minor improvement.
3. Shorten MultiWiFi connection time.

### Major Releases v1.1.0

1. Add **MultiWiFi feature to auto(Re)connect to the best WiFi at runtime**
2. Fix bug.
3. Completely enhanced examples to use new MultiWiFi feature.

#### Releases v1.0.11

1. Add optional **CORS (Cross-Origin Resource Sharing)** feature. Thanks to [AlesSt](https://github.com/AlesSt). See more in [Issue #27: CORS protection fires up with AJAX](https://github.com/khoih-prog/ESP_WiFiManager/issues/27) and [Cross Origin Resource Sharing](https://en.wikipedia.org/wiki/Cross-origin_resource_sharing). To use, you must explicitly use `#define USING_CORS_FEATURE true`
2. Solve issue softAP with custom IP sometimes not working. Thanks to [AlesSt](https://github.com/AlesSt). See [Issue #26: softAP with custom IP not working](https://github.com/khoih-prog/ESP_WiFiManager/issues/26) and [Wifi.softAPConfig() sometimes set the wrong IP address](https://github.com/espressif/arduino-esp32/issues/985).
3. Temporary fix for issue of not clearing WiFi SSID/PW from flash of ESP32. Thanks to [AlesSt](https://github.com/AlesSt). See more in [Issue #25: API call /r doesnt clear credentials](https://github.com/khoih-prog/ESP_WiFiManager/issues/25) and [WiFi.disconnect(true) problem](https://github.com/espressif/arduino-esp32/issues/400).
4. Fix autoConnect() feature to permit autoConnect() to use STA static IP or DHCP IP. Remove from deprecated function list.
5. Enhance README.md with more instructions and illustrations.


#### Releases v1.0.10

1. Don't need to reinput already working SSID in Config Port to update other parameters, such as StaticIP.
2. Disable/Enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
3. Change HTTP_XYZ constants to WM_HTTP_XYZ to avoid conflicts with future releases of ESP32 and ESP8266 cores.
4. Add feature to change WiFi AP channel (fixed or random) to avoid conflict in AP-overcrowded environments.
5. Enhance Config Portal GUI and get the item out of the bucket list.
6. Enhance README.md with more instructions and illustrations.

#### Releases v1.0.9

1. Fix ESP32 STAstaticIP bug. 
2. Enable changing from DHCP <-> static IP using Config Portal.
3. Enable NTP configuration from sketch (USE_ESP_WIFIMANAGER_NTP, USE_CLOUDFLARE_NTP). See Issue #21: [CloudFlare link in the default portal](https://github.com/khoih-prog/ESP_WiFiManager/issues/21).
4. Add, enhance examples (fix MDNS for ESP32 examples, add DRD feature).

#### Releases v1.0.8

1. Fix setSTAStaticIPConfig issue. See [Static Station IP doesn't work](https://github.com/khoih-prog/ESP_WiFiManager/issues/17)
2. Add LittleFS support for ESP8266 core 2.7.1+ in examples to replace deprecated SPIFFS.
3. Restructure code.

#### Releases v1.0.7

1. Use `just-in-time` scanWiFiNetworks() to reduce connection time necessary for battery-operated DeepSleep application. Thanks to [CrispinP](https://github.com/CrispinP) for identifying, requesting and testing. See [Starting WiFIManger is very slow (2000ms)](https://github.com/khoih-prog/ESP_WiFiManager/issues/6)
2. Fix bug relating SPIFFS in examples :
 - [ConfigOnSwitchFS](examples/ConfigOnSwitchFS)
 - [ConfigPortalParamsOnSwitch](examples/ConfigPortalParamsOnSwitch)  (now support ArduinoJson 6.0.0+ as well as 5.13.5-)
 - [AutoConnectWithFSParameters](examples/AutoConnectWithFSParameters)
 See [Having issue to read the SPIFF file](https://github.com/khoih-prog/ESP_WiFiManager/issues/14), Thanks to [OttoKlaasen](https://github.com/OttoKlaasen) to report.
3. Fix [README](README.md). See [Accessing manager after connection](https://github.com/khoih-prog/ESP_WiFiManager/issues/15) 

#### Releases v1.0.6

1. Add function getConfigPortalPW()
2. Add 4 new complicated examples compatible with ArduinoJson 6.0.0+ :[AutoConnect](examples/AutoConnect), [AutoConnectWithFeedback](examples/AutoConnectWithFeedback), [AutoConnectWithFeedbackLED](examples/AutoConnectWithFeedbackLED) and [AutoConnectWithFSParameters](examples/AutoConnectWithFSParameters)




