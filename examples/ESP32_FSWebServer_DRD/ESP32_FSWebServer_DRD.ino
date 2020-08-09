/****************************************************************************************************************************
  ESP32_FSWebServer_DRD - Example WebServer with SPIFFS backend for esp32 with DoubleResetDetect feature
  For ESP32 boards
  
  ESP_WiFiManager is a library for the ESP8266/ESP32 platform (https://github.com/esp8266/Arduino) to enable easy
  configuration and reconfiguration of WiFi credentials using a Captive Portal.
  
  Modified from Tzapu https://github.com/tzapu/WiFiManager
  and from Ken Taylor https://github.com/kentaylor
  
  Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager
  Licensed under MIT license
  
  Example modified from https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/FSBrowser/FSBrowser.ino
  
  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WebServer library for Arduino environment.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
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
/*****************************************************************************************************************************
   How To Use:
   1) Upload the contents of the data folder with MkSPIFFS Tool ("ESP32 Sketch Data Upload" in Tools menu in Arduino IDE)
   2) or you can upload the contents of a folder if you CD in that folder and run the following command:
      for file in `\ls -A1`; do curl -F "file=@$PWD/$file" esp32-fs-browser.local/edit; done
   3) access the sample web page at http://esp32-fs-browser.local
   4) edit the page by going to http://esp32-fs-browser.local/edit
*****************************************************************************************************************************/
#if !defined(ESP32)
  #error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _WIFIMGR_LOGLEVEL_    3

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// These defines must be put before #include <ESP_DoubleResetDetector.h>
// to select where to store DoubleResetDetector's variable.
// For ESP32, You must select one to be true (EEPROM or SPIFFS)
// Otherwise, library will use default EEPROM storage
#define ESP_DRD_USE_EEPROM      false
#define ESP_DRD_USE_SPIFFS      true

#define DOUBLERESETDETECTOR_DEBUG       true  //false

#include <ESP_DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector

// Number of seconds after reset during which a
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

//DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);
DoubleResetDetector* drd;
//////

// You only need to format the filesystem once
//#define FORMAT_FILESYSTEM true
#define FORMAT_FILESYSTEM false

#define USE_SPIFFS      true

#if USE_SPIFFS
  #include <SPIFFS.h>
  FS* filesystem = &SPIFFS;
  #define FILESYSTEM    SPIFFS
#else
  // Use FFat
  #include <FFat.h>
  FS* filesystem = &FFat;
  #define FILESYSTEM    FFat
#endif

#define DBG_OUTPUT_PORT Serial

// SSID and PW for Config Portal
String ssid = "ESP_" + String((uint32_t)ESP.getEfuseMac(), HEX);
const char* password = "your_password";

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

// Indicates whether ESP has WiFi credentials saved from previous session, or double reset detected
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

const char* host = "esp32-fs-browser";

#define HTTP_PORT     80

WebServer server(HTTP_PORT);

//holds the current upload
File fsUploadFile;

//format bytes
String formatBytes(size_t bytes) 
{
  if (bytes < 1024) 
  {
    return String(bytes) + "B";
  } 
  else if (bytes < (1024 * 1024)) 
  {
    return String(bytes / 1024.0) + "KB";
  } 
  else if (bytes < (1024 * 1024 * 1024)) 
  {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } 
  else 
  {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) 
{
  if (server.hasArg("download")) 
  {
    return "application/octet-stream";
  } 
  else if (filename.endsWith(".htm")) 
  {
    return "text/html";
  } 
  else if (filename.endsWith(".html")) 
  {
    return "text/html";
  } 
  else if (filename.endsWith(".css")) 
  {
    return "text/css";
  } 
  else if (filename.endsWith(".js")) 
  {
    return "application/javascript";
  } 
  else if (filename.endsWith(".png")) 
  {
    return "image/png";
  } 
  else if (filename.endsWith(".gif")) 
  {
    return "image/gif";
  } 
  else if (filename.endsWith(".jpg")) 
  {
    return "image/jpeg";
  } 
  else if (filename.endsWith(".ico")) 
  {
    return "image/x-icon";
  } 
  else if (filename.endsWith(".xml")) 
  {
    return "text/xml";
  } 
  else if (filename.endsWith(".pdf")) 
  {
    return "application/x-pdf";
  } 
  else if (filename.endsWith(".zip")) 
  {
    return "application/x-zip";
  } 
  else if (filename.endsWith(".gz")) 
  {
    return "application/x-gzip";
  }
  return "text/plain";
}

bool handleFileRead(String path) 
{
  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  
  if (path.endsWith("/")) 
  {
    path += "index.htm";
  }
  
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  
  if (filesystem->exists(pathWithGz) || filesystem->exists(path)) 
  {
    if (filesystem->exists(pathWithGz)) 
    {
      path += ".gz";
    }
    
    File file = filesystem->open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  
  return false;
}

void handleFileUpload() 
{
  if (server.uri() != "/edit") 
  {
    return;
  }
  
  HTTPUpload& upload = server.upload();
  
  if (upload.status == UPLOAD_FILE_START) 
  {
    String filename = upload.filename;
    if (!filename.startsWith("/")) 
    {
      filename = "/" + filename;
    }
    
    DBG_OUTPUT_PORT.print("handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = filesystem->open(filename, "w");
    filename.clear();
  } 
  else if (upload.status == UPLOAD_FILE_WRITE) 
  {
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile) 
    {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } 
  else if (upload.status == UPLOAD_FILE_END) 
  {
    if (fsUploadFile) 
    {
      fsUploadFile.close();
    }
    DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void handleFileDelete() 
{
  if (server.args() == 0) 
  {
    return server.send(500, "text/plain", "BAD ARGS");
  }
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
  
  if (path == "/") 
  {
    return server.send(500, "text/plain", "BAD PATH");
  }
  
  if (!filesystem->exists(path)) 
  {
    return server.send(404, "text/plain", "FileNotFound");
  }
  
  filesystem->remove(path);
  server.send(200, "text/plain", "");
  path.clear();
}

void handleFileCreate() 
{
  if (server.args() == 0) 
  {
    return server.send(500, "text/plain", "BAD ARGS");
  }
  
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
  
  if (path == "/") 
  {
    return server.send(500, "text/plain", "BAD PATH");
  }
  
  if (filesystem->exists(path)) 
  {
    return server.send(500, "text/plain", "FILE EXISTS");
  }
  
  File file = filesystem->open(path, "w");
  
  if (file) 
  {
    file.close();
  } 
  else 
  {
    return server.send(500, "text/plain", "CREATE FAILED");
  }
  server.send(200, "text/plain", "");
  path.clear();
}

void handleFileList() 
{
  if (!server.hasArg("dir")) 
  {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  DBG_OUTPUT_PORT.println("handleFileList: " + path);
  
  File root = FILESYSTEM.open(path);
  path = String();

  String output = "[";
  
  if (root.isDirectory()) 
  {
    File file = root.openNextFile();
    
    while (file) 
    {
      if (output != "[") 
      {
        output += ',';
      }
      
      output += "{\"type\":\"";
      output += (file.isDirectory()) ? "dir" : "file";
      output += "\",\"name\":\"";
      output += String(file.name()).substring(1);
      output += "\"}";
      file = root.openNextFile();
    }
  }
  
  output += "]";

  DBG_OUTPUT_PORT.println("handleFileList: " + output);

  server.send(200, "text/json", output);
}

void setup(void)
{
  DBG_OUTPUT_PORT.begin(115200);
  while (!DBG_OUTPUT_PORT);
  
  DBG_OUTPUT_PORT.println("\nStarting ESP32_FSWebServer_DRD with DoubleResetDetect on " + String(ARDUINO_BOARD));

  DBG_OUTPUT_PORT.setDebugOutput(false);

  drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);

  if (FORMAT_FILESYSTEM) 
    FILESYSTEM.format();

  FILESYSTEM.begin();
  {
    File root = FILESYSTEM.open("/");
    File file = root.openNextFile();
    
    while (file) 
    {
      String fileName = file.name();
      size_t fileSize = file.size();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
      file = root.openNextFile();
    }
    
    DBG_OUTPUT_PORT.printf("\n");
  }

  unsigned long startedAt = millis();

  //Local intialization. Once its business is done, there is no need to keep it around
  // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
  //ESP_WiFiManager ESP_wifiManager;
  // Use this to personalize DHCP hostname (RFC952 conformed)
  ESP_WiFiManager ESP_wifiManager("ESP32-FSWebServer_DRD");

  //set custom ip for portal
  ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));

  ESP_wifiManager.setMinimumSignalQuality(-1);

  // From v1.0.10 only
  // Set config portal channel, default = 1. Use 0 => random channel from 1-13
  ESP_wifiManager.setConfigPortalChannel(0);
  //////
  
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
  DBG_OUTPUT_PORT.println("Stored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);

  // SSID to uppercase
  ssid.toUpperCase();

  if (Router_SSID != "")
  {
    ESP_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
    Serial.println("Got stored Credentials. Timeout 120s for Config Portal");
  }
  else
  {
    Serial.println("Open Config Portal without Timeout: No stored Credentials.");
    initialConfig = true;
  }

  if (drd->detectDoubleReset())
  {
    // DRD, disable timeout.
    ESP_wifiManager.setConfigPortalTimeout(0);
    
    DBG_OUTPUT_PORT.println("Open Config Portal without Timeout: Double Reset Detected");
    initialConfig = true;
  }

  if (initialConfig)
  {
    // Starts an access point
    if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
      DBG_OUTPUT_PORT.println("Not connected to WiFi but continuing anyway.");
    else
    {
      DBG_OUTPUT_PORT.println("WiFi connected...yeey :)");
    }
  }

#define WIFI_CONNECT_TIMEOUT        30000L
#define WHILE_LOOP_DELAY            200L
#define WHILE_LOOP_STEPS            (WIFI_CONNECT_TIMEOUT / ( 3 * WHILE_LOOP_DELAY ))

  startedAt = millis();

  if (!initialConfig)
  {
    while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) )
    {
      WiFi.mode(WIFI_STA);
      WiFi.persistent (true);
  
      // We start by connecting to a WiFi network
      DBG_OUTPUT_PORT.print("Connecting to ");
      DBG_OUTPUT_PORT.println(Router_SSID);
  
      //WiFi.config(stationIP, gatewayIP, netMask);
      WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);
      
      WiFi.begin(Router_SSID.c_str(), Router_Pass.c_str());
  
      int i = 0;
      
      while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS)
      {
        delay(WHILE_LOOP_DELAY);
      }
    }
  }

  DBG_OUTPUT_PORT.println("");
  DBG_OUTPUT_PORT.print("Connected! IP address: ");
  DBG_OUTPUT_PORT.println(WiFi.localIP());

  //SERVER INIT
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  
  //load editor
  server.on("/edit", HTTP_GET, []() 
  {
    if (!handleFileRead("/edit.htm")) 
    {
      server.send(404, "text/plain", "FileNotFound");
    }
  });
  
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() 
  {
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() 
  {
    if (!handleFileRead(server.uri())) 
    {
      server.send(404, "text/plain", "FileNotFound");
    }
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, []() 
  {
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ", \"analog\":" + String(analogRead(A0));
    json += ", \"gpio\":" + String((uint32_t)(0));
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });

  server.begin();
  
  DBG_OUTPUT_PORT.print("HTTP server started @ ");
  DBG_OUTPUT_PORT.println(WiFi.localIP());

  MDNS.begin(host);
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", HTTP_PORT);

  DBG_OUTPUT_PORT.print("Open http://");
  DBG_OUTPUT_PORT.print(host);
  DBG_OUTPUT_PORT.println(".local/edit to see the file browser");
}

void loop(void) 
{
  // Call the double reset detector loop method every so often,
  // so that it can recognise when the timeout expires.
  // You can also call drd.stop() when you wish to no longer
  // consider the next reset as a double reset.
  drd->loop();
  
  server.handleClient();
}
