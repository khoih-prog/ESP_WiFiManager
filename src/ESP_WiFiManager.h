/****************************************************************************************************************************
  ESP_WiFiManager.h
  For ESP8266 / ESP32 boards

  ESP_WiFiManager is a library for the ESP8266/Arduino platform
  (https://github.com/esp8266/Arduino) to enable easy
  configuration and reconfiguration of WiFi credentials using a Captive Portal
  inspired by:
  http://www.esp8266.com/viewtopic.php?f=29&t=2520
  https://github.com/chriscook8/esp-arduino-apboot
  https://github.com/esp8266/Arduino/blob/master/libraries/DNSServer/examples/CaptivePortalAdvanced/

  Modified from Tzapu https://github.com/tzapu/WiFiManager
  and from Ken Taylor https://github.com/kentaylor

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager
  Licensed under MIT license
  
  Version: 1.10.1

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
  1.4.1   K Hoang      22/12/2020 Fix staticIP not saved. Add functions. Add complex examples. Sync with ESPAsync_WiFiManager
  1.4.2   K Hoang      14/01/2021 Fix examples' bug not using saved WiFi Credentials after losing all WiFi connections.
  1.4.3   K Hoang      23/01/2021 Fix examples' bug not saving Static IP in certain cases.
  1.5.0   K Hoang      12/02/2021 Add support to new ESP32-S2
  1.5.1   K Hoang      26/03/2021 Fix compiler error if setting Compiler Warnings to All. Retest with esp32 core v1.0.6
  1.5.2   K Hoang      08/04/2021 Fix example misleading messages.
  1.5.3   K Hoang      13/04/2021 Add dnsServer error message.
  1.6.0   K Hoang      20/04/2021 Add support to new ESP32-C3 using SPIFFS or EEPROM
  1.6.1   K Hoang      25/04/2021 Fix MultiWiFi bug. Fix captive-portal bug if CP AP address is not default 192.168.4.1
  1.7.0   K Hoang      06/05/2021 Set _timezoneName. Add support to new ESP32-S2 (METRO_ESP32S2, FUNHOUSE_ESP32S2, etc.)
  1.7.1   K Hoang      08/05/2021 Fix Json bug. Fix timezoneName not displayed in Info page.
  1.7.2   K Hoang      08/05/2021 Fix warnings with ESP8266 core v3.0.0
  1.7.3   K Hoang      29/07/2021 Fix MultiWiFi connection issue with ESP32 core v2.0.0-rc1+
  1.7.4   K Hoang      13/08/2021 Add WiFi scanning of hidden SSIDs
  1.7.5   K Hoang      10/10/2021 Update `platform.ini` and `library.json`
  1.7.6   K Hoang      26/11/2021 Auto detect ESP32 core and use either built-in LittleFS or LITTLEFS library
  1.7.7   K Hoang      26/11/2021 Fix compile error for ESP32 core v1.0.5-
  1.7.8   K Hoang      30/11/2021 Fix bug to permit using HTTP port different from 80. Fix bug
  1.8.0   K Hoang      29/12/2021 Fix `multiple-definitions` linker error and weird bug related to src_cpp
  1.9.0   K Hoang      17/01/2022 Enable compatibility with old code to include only ESP_WiFiManager.h
  1.10.0  K Hoang      10/02/2022 Add support to new ESP32-S3
  1.10.1  K Hoang      11/02/2022 Add LittleFS support to ESP32-C3. Use core LittleFS instead of Lorol's LITTLEFS for v2.0.0+
 *****************************************************************************************************************************/

#pragma once

#ifndef ESP_WiFiManager_h
#define ESP_WiFiManager_h

#include <ESP_WiFiManager.hpp>              //https://github.com/khoih-prog/ESP_WiFiManager
#include <ESP_WiFiManager-Impl.h>           //https://github.com/khoih-prog/ESP_WiFiManager

#endif    // ESP_WiFiManager_h

