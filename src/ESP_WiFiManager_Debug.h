/****************************************************************************************************************************
  ESP_WiFiManager_Debug.h
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

  Version: 1.12.1

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      07/10/2019 Initial coding
  ...
  1.8.0   K Hoang      29/12/2021 Fix `multiple-definitions` linker error and weird bug related to src_cpp
  1.9.0   K Hoang      17/01/2022 Enable compatibility with old code to include only ESP_WiFiManager.h
  1.10.0  K Hoang      10/02/2022 Add support to new ESP32-S3
  1.10.1  K Hoang      11/02/2022 Add LittleFS support to ESP32-C3. Use core LittleFS instead of Lorol's LITTLEFS for v2.0.0+
  1.10.2  K Hoang      13/03/2022 Send CORS header in handleWifiSave() function
  1.11.0  K Hoang      09/09/2022 Fix ESP32 chipID and add ESP_getChipOUI()
  1.12.0  K Hoang      07/10/2022 Optional display Credentials (SSIDs, PWDs) in Config Portal
  1.12.1  K Hoang      25/10/2022 Using random channel for softAP without password. Add astyle using allman style
 *****************************************************************************************************************************/

#pragma once

#ifndef ESP_WiFiManager_Debug_H
#define ESP_WiFiManager_Debug_H

//////////////////////////////////////////

#ifdef WIFIMGR_DEBUG_PORT
  #define WM_DBG_PORT      WIFIMGR_DEBUG_PORT
#else
  #define WM_DBG_PORT      Serial
#endif

//////////////////////////////////////////

// Change _WIFIMGR_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _WIFIMGR_LOGLEVEL_
  #define _WIFIMGR_LOGLEVEL_       0
#endif

//////////////////////////////////////////

const char WM_MARK[] = "[WM] ";
const char WM_SP[]   = " ";

#define WM_PRINT        WM_DBG_PORT.print
#define WM_PRINTLN      WM_DBG_PORT.println

#define WM_PRINT_MARK   WM_PRINT(WM_MARK)
#define WM_PRINT_SP     WM_PRINT(WM_SP)

////////////////////////////////////////////////////

#define LOGERROR(x)         if(_WIFIMGR_LOGLEVEL_>0) { WM_PRINT_MARK; WM_PRINTLN(x); }
#define LOGERROR0(x)        if(_WIFIMGR_LOGLEVEL_>0) { WM_PRINT(x); }
#define LOGERROR1(x,y)      if(_WIFIMGR_LOGLEVEL_>0) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINTLN(y); }
#define LOGERROR2(x,y,z)    if(_WIFIMGR_LOGLEVEL_>0) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINT(y); WM_PRINT_SP; WM_PRINTLN(z); }
#define LOGERROR3(x,y,z,w)  if(_WIFIMGR_LOGLEVEL_>0) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINT(y); WM_PRINT_SP; WM_PRINT(z); WM_PRINT_SP; WM_PRINTLN(w); }

////////////////////////////////////////////////////

#define LOGWARN(x)          if(_WIFIMGR_LOGLEVEL_>1) { WM_PRINT_MARK; WM_PRINTLN(x); }
#define LOGWARN0(x)         if(_WIFIMGR_LOGLEVEL_>1) { WM_PRINT(x); }
#define LOGWARN1(x,y)       if(_WIFIMGR_LOGLEVEL_>1) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINTLN(y); }
#define LOGWARN2(x,y,z)     if(_WIFIMGR_LOGLEVEL_>1) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINT(y); WM_PRINT_SP; WM_PRINTLN(z); }
#define LOGWARN3(x,y,z,w)   if(_WIFIMGR_LOGLEVEL_>1) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINT(y); WM_PRINT_SP; WM_PRINT(z); WM_PRINT_SP; WM_PRINTLN(w); }

////////////////////////////////////////////////////

#define LOGINFO(x)          if(_WIFIMGR_LOGLEVEL_>2) { WM_PRINT_MARK; WM_PRINTLN(x); }
#define LOGINFO0(x)         if(_WIFIMGR_LOGLEVEL_>2) { WM_PRINT(x); }
#define LOGINFO1(x,y)       if(_WIFIMGR_LOGLEVEL_>2) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINTLN(y); }
#define LOGINFO2(x,y,z)     if(_WIFIMGR_LOGLEVEL_>2) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINT(y); WM_PRINT_SP; WM_PRINTLN(z); }
#define LOGINFO3(x,y,z,w)   if(_WIFIMGR_LOGLEVEL_>2) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINT(y); WM_PRINT_SP; WM_PRINT(z); WM_PRINT_SP; WM_PRINTLN(w); }

////////////////////////////////////////////////////

#define LOGDEBUG(x)         if(_WIFIMGR_LOGLEVEL_>3) { WM_PRINT_MARK; WM_PRINTLN(x); }
#define LOGDEBUG0(x)        if(_WIFIMGR_LOGLEVEL_>3) { WM_PRINT(x); }
#define LOGDEBUG1(x,y)      if(_WIFIMGR_LOGLEVEL_>3) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINTLN(y); }
#define LOGDEBUG2(x,y,z)    if(_WIFIMGR_LOGLEVEL_>3) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINT(y); WM_PRINT_SP; WM_PRINTLN(z); }
#define LOGDEBUG3(x,y,z,w)  if(_WIFIMGR_LOGLEVEL_>3) { WM_PRINT_MARK; WM_PRINT(x); WM_PRINT_SP; WM_PRINT(y); WM_PRINT_SP; WM_PRINT(z); WM_PRINT_SP; WM_PRINTLN(w); }

////////////////////////////////////////////////////

#endif    //ESP_WiFiManager_Debug_H
