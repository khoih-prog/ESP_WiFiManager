/****************************************************************************************************************************
  ESP_WiFiManager-Impl.h
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

#ifndef ESP_WiFiManager_Impl_h
#define ESP_WiFiManager_Impl_h

//////////////////////////////////////////

ESP_WMParameter::ESP_WMParameter(const char *custom)
{
  _WMParam_data._id = NULL;
  _WMParam_data._placeholder = NULL;
  _WMParam_data._length = 0;
  _WMParam_data._value = NULL;
  _WMParam_data._labelPlacement = WFM_LABEL_BEFORE;

  _customHTML = custom;
}

//////////////////////////////////////////

ESP_WMParameter::ESP_WMParameter(const char *id, const char *placeholder, const char *defaultValue, const int& length,
                                 const char *custom, const int& labelPlacement)
{
  init(id, placeholder, defaultValue, length, custom, labelPlacement);
}

//////////////////////////////////////////

ESP_WMParameter::ESP_WMParameter(const WMParam_Data& WMParam_data)
{
  init(WMParam_data._id, WMParam_data._placeholder, WMParam_data._value, WMParam_data._length, "",
       WMParam_data._labelPlacement);
}

//////////////////////////////////////////

void ESP_WMParameter::init(const char *id, const char *placeholder, const char *defaultValue, const int& length,
                           const char *custom, const int& labelPlacement)
{
  _WMParam_data._id = id;
  _WMParam_data._placeholder = placeholder;
  _WMParam_data._length = length;
  _WMParam_data._labelPlacement = labelPlacement;

  _WMParam_data._value = new char[_WMParam_data._length + 1];

  if (_WMParam_data._value != NULL)
  {
    memset(_WMParam_data._value, 0, _WMParam_data._length + 1);

    if (defaultValue != NULL)
    {
      strncpy(_WMParam_data._value, defaultValue, _WMParam_data._length);
    }
  }

  _customHTML = custom;
}

//////////////////////////////////////////

ESP_WMParameter::~ESP_WMParameter()
{
  if (_WMParam_data._value != NULL)
  {
    delete[] _WMParam_data._value;
  }
}

//////////////////////////////////////////

// Using Struct to get/set whole data at once
void ESP_WMParameter::setWMParam_Data(const WMParam_Data& WMParam_data)
{
  LOGINFO(F("setWMParam_Data"));

  memcpy(&_WMParam_data, &WMParam_data, sizeof(_WMParam_data));
}

//////////////////////////////////////////

void ESP_WMParameter::getWMParam_Data(WMParam_Data &WMParam_data)
{
  LOGINFO(F("getWMParam_Data"));

  memcpy(&WMParam_data, &_WMParam_data, sizeof(WMParam_data));
}

//////////////////////////////////////////

const char* ESP_WMParameter::getValue()
{
  return _WMParam_data._value;
}

//////////////////////////////////////////

const char* ESP_WMParameter::getID()
{
  return _WMParam_data._id;
}

//////////////////////////////////////////

const char* ESP_WMParameter::getPlaceholder()
{
  return _WMParam_data._placeholder;
}

//////////////////////////////////////////

int ESP_WMParameter::getValueLength()
{
  return _WMParam_data._length;
}

//////////////////////////////////////////

int ESP_WMParameter::getLabelPlacement()
{
  return _WMParam_data._labelPlacement;
}

//////////////////////////////////////////

const char* ESP_WMParameter::getCustomHTML()
{
  return _customHTML;
}

//////////////////////////////////////////
//////////////////////////////////////////

/**
   [getParameters description]
   @access public
*/
ESP_WMParameter** ESP_WiFiManager::getParameters()
{
  return _params;
}

//////////////////////////////////////////

/**
   [getParametersCount description]
   @access public
*/
int ESP_WiFiManager::getParametersCount()
{
  return _paramsCount;
}

//////////////////////////////////////////

char* ESP_WiFiManager::getRFC952_hostname(const char* iHostname)
{
  memset(RFC952_hostname, 0, sizeof(RFC952_hostname));

  size_t len = (RFC952_HOSTNAME_MAXLEN < strlen(iHostname)) ? RFC952_HOSTNAME_MAXLEN : strlen(iHostname);

  size_t j = 0;

  for (size_t i = 0; i < len - 1; i++)
  {
    if (isalnum(iHostname[i]) || iHostname[i] == '-')
    {
      RFC952_hostname[j] = iHostname[i];
      j++;
    }
  }

  // no '-' as last char
  if (isalnum(iHostname[len - 1]) || (iHostname[len - 1] != '-'))
    RFC952_hostname[j] = iHostname[len - 1];

  return RFC952_hostname;
}

//////////////////////////////////////////

ESP_WiFiManager::ESP_WiFiManager(const char *iHostname)
{
#if USE_DYNAMIC_PARAMS
  _max_params = WIFI_MANAGER_MAX_PARAMS;
  _params = (ESP_WMParameter**)malloc(_max_params * sizeof(ESP_WMParameter*));
#endif

  //WiFi not yet started here, must call WiFi.mode(WIFI_STA) and modify function WiFiGenericClass::mode(wifi_mode_t m) !!!
  WiFi.mode(WIFI_STA);

  if (iHostname[0] == 0)
  {
#ifdef ESP8266
    String _hostname = "ESP8266-" + String(ESP.getChipId(), HEX);
#else   //ESP32
    String _hostname = "ESP32-" + String(ESP_getChipId(), HEX);

#endif
    _hostname.toUpperCase();

    getRFC952_hostname(_hostname.c_str());

  }
  else
  {
    // Prepare and store the hostname only not NULL
    getRFC952_hostname(iHostname);
  }

  LOGWARN1(F("RFC925 Hostname ="), RFC952_hostname);

  setHostname();

  networkIndices = NULL;
}

//////////////////////////////////////////

ESP_WiFiManager::~ESP_WiFiManager()
{
#if USE_DYNAMIC_PARAMS

  if (_params != NULL)
  {
    LOGINFO(F("freeing allocated params!"));

    free(_params);
  }

#endif

  if (networkIndices)
  {
    free(networkIndices); //indices array no longer required so free memory
  }
}

//////////////////////////////////////////

#if USE_DYNAMIC_PARAMS
  bool ESP_WiFiManager::addParameter(ESP_WMParameter *p)
#else
  void ESP_WiFiManager::addParameter(ESP_WMParameter *p)
#endif
{
#if USE_DYNAMIC_PARAMS

  if (_paramsCount == _max_params)
  {
    // rezise the params array
    _max_params += WIFI_MANAGER_MAX_PARAMS;

    LOGINFO1(F("Increasing _max_params to:"), _max_params);

    ESP_WMParameter** new_params = (ESP_WMParameter**)realloc(_params, _max_params * sizeof(ESP_WMParameter*));

    if (new_params != NULL)
    {
      _params = new_params;
    }
    else
    {
      LOGINFO(F("ERROR: failed to realloc params, size not increased!"));

      return false;
    }
  }

  _params[_paramsCount] = p;
  _paramsCount++;

  LOGINFO1(F("Adding parameter"), p->getID());

  return true;

#else

  // Danger here. Better to use Tzapu way here
  if (_paramsCount < (WIFI_MANAGER_MAX_PARAMS))
  {
    _params[_paramsCount] = p;
    _paramsCount++;

    LOGINFO1(F("Adding parameter"), p->getID());
  }
  else
  {
    LOGINFO("Can't add parameter. Full");
  }

#endif
}

//////////////////////////////////////////

void ESP_WiFiManager::setupConfigPortal()
{
  stopConfigPortal = false; //Signal not to close config portal

  /*This library assumes autoconnect is set to 1. It usually is
    but just in case check the setting and turn on autoconnect if it is off.
    Some useful discussion at https://github.com/esp8266/Arduino/issues/1615*/
  if (WiFi.getAutoConnect() == 0)
    WiFi.setAutoConnect(1);

  dnsServer.reset(new DNSServer());

#ifdef ESP8266
  server.reset(new ESP8266WebServer(HTTP_PORT_TO_USE));
#else   //ESP32
  server.reset(new WebServer(HTTP_PORT_TO_USE));
#endif

  // optional soft ip config
  // Must be put here before dns server start to take care of the non-default ConfigPortal AP IP.
  // Check (https://github.com/khoih-prog/ESP_WiFiManager/issues/58)
  if (_WiFi_AP_IPconfig._ap_static_ip)
  {
    LOGWARN3(F("Custom AP IP/GW/Subnet = "), _WiFi_AP_IPconfig._ap_static_ip, _WiFi_AP_IPconfig._ap_static_gw,
             _WiFi_AP_IPconfig._ap_static_sn);

    WiFi.softAPConfig(_WiFi_AP_IPconfig._ap_static_ip, _WiFi_AP_IPconfig._ap_static_gw, _WiFi_AP_IPconfig._ap_static_sn);
  }

  /* Setup the DNS server redirecting all the domains to the apIP */
  if (dnsServer)
  {
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);

    // DNSServer started with "*" domain name, all DNS requests will be passsed to WiFi.softAPIP()
    if (! dnsServer->start(DNS_PORT, "*", WiFi.softAPIP()))
    {
      // No socket available
      LOGERROR(F("Can't start DNS Server. No available socket"));
    }
  }
  else
  {
    // No space available
    LOGERROR(F("Can't initiate DNS Server. No enough space"));
  }

  _configPortalStart = millis();

  LOGWARN1(F("Configuring AP SSID ="), _apName);

  if (_apPassword != NULL)
  {
    if (strlen(_apPassword) < 8 || strlen(_apPassword) > 63)
    {
      // fail passphrase to short or long!
      LOGERROR(F("Invalid AccessPoint password. Ignoring"));

      _apPassword = NULL;
    }

    LOGWARN1(F("AP PWD ="), _apPassword);
  }

  // KH, To enable dynamic/random channel
  static int channel;

  // Use random channel if  _WiFiAPChannel == 0
  if (_WiFiAPChannel == 0)
    channel = (_configPortalStart % MAX_WIFI_CHANNEL) + 1;
  else
    channel = _WiFiAPChannel;

  LOGWARN1(F("AP Channel ="), channel);

  WiFi.softAP(_apName, _apPassword, channel);

  delay(500); // Without delay I've seen the IP address blank

  LOGWARN1(F("AP IP address ="), WiFi.softAPIP());

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server->on("/", std::bind(&ESP_WiFiManager::handleRoot, this));
  server->on("/wifi", std::bind(&ESP_WiFiManager::handleWifi, this));
  server->on("/wifisave", std::bind(&ESP_WiFiManager::handleWifiSave, this));
  server->on("/close", std::bind(&ESP_WiFiManager::handleServerClose, this));
  server->on("/i", std::bind(&ESP_WiFiManager::handleInfo, this));
  server->on("/r", std::bind(&ESP_WiFiManager::handleReset, this));
  server->on("/state", std::bind(&ESP_WiFiManager::handleState, this));
  server->on("/scan", std::bind(&ESP_WiFiManager::handleScan, this));
  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server->on("/fwlink", std::bind(&ESP_WiFiManager::handleRoot, this));
  server->onNotFound(std::bind(&ESP_WiFiManager::handleNotFound, this));
  server->begin(); // Web server start

  LOGWARN(F("HTTP server started"));
}

//////////////////////////////////////////

bool ESP_WiFiManager::autoConnect()
{
#ifdef ESP8266
  String ssid = "ESP_" + String(ESP.getChipId());
#else   //ESP32
  String ssid = "ESP_" + String(ESP_getChipId());

#endif

  return autoConnect(ssid.c_str(), NULL);
}

//////////////////////////////////////////

/* This is not very useful as there has been an assumption that device has to be
  told to connect but Wifi already does it's best to connect in background. Calling this
  method will block until WiFi connects. Sketch can avoid
  blocking call then use (WiFi.status()==WL_CONNECTED) test to see if connected yet.
  See some discussion at https://github.com/tzapu/WiFiManager/issues/68
*/

// To permit autoConnect() to use STA static IP or DHCP IP.
#ifndef AUTOCONNECT_NO_INVALIDATE
  #define AUTOCONNECT_NO_INVALIDATE true
#endif

//////////////////////////////////////////

bool ESP_WiFiManager::autoConnect(char const *apName, char const *apPassword)
{
#if AUTOCONNECT_NO_INVALIDATE
  LOGINFO(F("\nAutoConnect using previously saved SSID/PW, but keep previous settings"));
  // Connect to previously saved SSID/PW, but keep previous settings
  connectWifi();
#else
  LOGINFO(F("\nAutoConnect using previously saved SSID/PW, but invalidate previous settings"));
  // Connect to previously saved SSID/PW, but invalidate previous settings
  connectWifi(WiFi_SSID(), WiFi_Pass());
#endif

  unsigned long startedAt = millis();

  while (millis() - startedAt < 10000)
  {
    delay(200);

    if (WiFi.status() == WL_CONNECTED)
    {
      float waited = (millis() - startedAt);

      LOGWARN1(F("Connected after waiting (s) :"), waited / 1000);
      LOGWARN1(F("Local ip ="), WiFi.localIP());

      return true;
    }
  }

  return startConfigPortal(apName, apPassword);
}

//////////////////////////////////////////

bool  ESP_WiFiManager::startConfigPortal()
{
#ifdef ESP8266
  String ssid = "ESP_" + String(ESP.getChipId());
#else   //ESP32
  String ssid = "ESP_" + String(ESP_getChipId());
#endif
  ssid.toUpperCase();

  return startConfigPortal(ssid.c_str(), NULL);
}

//////////////////////////////////////////

bool  ESP_WiFiManager::startConfigPortal(char const *apName, char const *apPassword)
{
  //setup AP
  int connRes = WiFi.waitForConnectResult();

  LOGINFO("WiFi.waitForConnectResult Done");

  if (connRes == WL_CONNECTED)
  {
    LOGINFO("SET AP_STA");

    WiFi.mode(WIFI_AP_STA); //Dual mode works fine if it is connected to WiFi
  }
  else
  {
    LOGINFO("SET AP");

    // Dual mode becomes flaky if not connected to a WiFi network.
    // When ESP8266 station is trying to find a target AP, it will scan on every channel,
    // that means ESP8266 station is changing its channel to scan. This makes the channel of ESP8266 softAP keep changing too..
    // So the connection may break. From http://bbs.espressif.com/viewtopic.php?t=671#p2531

    WiFi.mode(WIFI_AP);
  }

  _apName = apName;
  _apPassword = apPassword;

  //notify we entered AP mode
  if (_apcallback != NULL)
  {
    LOGINFO("_apcallback");

    _apcallback(this);
  }

  connect = false;

  setupConfigPortal();

  bool TimedOut = true;

  LOGINFO("startConfigPortal : Enter loop");

  while (_configPortalTimeout == 0 || millis() < _configPortalStart + _configPortalTimeout)
  {
    //DNS
    dnsServer->processNextRequest();
    //HTTP
    server->handleClient();

#if ( USING_ESP32_S2 || USING_ESP32_C3 )
    // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
    delay(1);
#endif

    if (connect)
    {
      TimedOut = false;
      delay(2000);

      LOGERROR(F("Connecting to new AP"));

      // using user-provided  _ssid, _pass in place of system-stored ssid and pass
      if (connectWifi(_ssid, _pass) != WL_CONNECTED)
      {
        LOGERROR(F("Failed to connect"));

        WiFi.mode(WIFI_AP); // Dual mode becomes flaky if not connected to a WiFi network.
      }
      else
      {
        //notify that configuration has changed and any optional parameters should be saved
        if (_savecallback != NULL)
        {
          //todo: check if any custom parameters actually exist, and check if they really changed maybe
          _savecallback();
        }

        break;
      }

      if (_shouldBreakAfterConfig)
      {
        //flag set to exit after config after trying to connect
        //notify that configuration has changed and any optional parameters should be saved
        if (_savecallback != NULL)
        {
          //todo: check if any custom parameters actually exist, and check if they really changed maybe
          _savecallback();
        }

        break;
      }
    }

    if (stopConfigPortal)
    {
      //TimedOut = false;

      LOGERROR("Stop ConfigPortal");    //KH

      stopConfigPortal = false;
      break;
    }

    yield();
  }

  WiFi.mode(WIFI_STA);

  if (TimedOut)
  {
    setHostname();

    // To fix static IP when CP not entered or timed-out
    setWifiStaticIP();

    WiFi.begin();
    int connRes = waitForConnectResult();

    LOGERROR1("Timed out connection result:", getStatus(connRes));
  }

  server->stop();
  server.reset();
  dnsServer->stop();
  dnsServer.reset();

  return  WiFi.status() == WL_CONNECTED;
}

//////////////////////////////////////////

void ESP_WiFiManager::setWifiStaticIP()
{
#if USE_CONFIGURABLE_DNS

  if (_WiFi_STA_IPconfig._sta_static_ip)
  {
    LOGWARN(F("Custom STA IP/GW/Subnet"));

    //***** Added section for DNS config option *****
    if (_WiFi_STA_IPconfig._sta_static_dns1 && _WiFi_STA_IPconfig._sta_static_dns2)
    {
      LOGWARN(F("DNS1 and DNS2 set"));

      WiFi.config(_WiFi_STA_IPconfig._sta_static_ip, _WiFi_STA_IPconfig._sta_static_gw, _WiFi_STA_IPconfig._sta_static_sn,
                  _WiFi_STA_IPconfig._sta_static_dns1, _WiFi_STA_IPconfig._sta_static_dns2);
    }
    else if (_WiFi_STA_IPconfig._sta_static_dns1)
    {
      LOGWARN(F("Only DNS1 set"));

      WiFi.config(_WiFi_STA_IPconfig._sta_static_ip, _WiFi_STA_IPconfig._sta_static_gw, _WiFi_STA_IPconfig._sta_static_sn,
                  _WiFi_STA_IPconfig._sta_static_dns1);
    }
    else
    {
      LOGWARN(F("No DNS server set"));

      WiFi.config(_WiFi_STA_IPconfig._sta_static_ip, _WiFi_STA_IPconfig._sta_static_gw, _WiFi_STA_IPconfig._sta_static_sn);
    }

    //***** End added section for DNS config option *****

    LOGINFO1(F("setWifiStaticIP IP ="), WiFi.localIP());
  }
  else
  {
    LOGWARN(F("Can't use Custom STA IP/GW/Subnet"));
  }

#else

  // check if we've got static_ip settings, if we do, use those.
  if (_WiFi_STA_IPconfig._sta_static_ip)
  {
    WiFi.config(_WiFi_STA_IPconfig._sta_static_ip, _WiFi_STA_IPconfig._sta_static_gw, _WiFi_STA_IPconfig._sta_static_sn);

    LOGWARN1(F("Custom STA IP/GW/Subnet : "), WiFi.localIP());
  }

#endif
}

//////////////////////////////////////////

int ESP_WiFiManager::reconnectWifi()
{
  int connectResult;

  // using user-provided _ssid, _pass instead of system-stored
  if ( ( connectResult = connectWifi(_ssid, _pass) ) != WL_CONNECTED)
  {
    LOGERROR1(F("Failed to connect to"), _ssid);

    if ( ( connectResult = connectWifi(_ssid1, _pass1) ) != WL_CONNECTED)
    {
      LOGERROR1(F("Failed to connect to"), _ssid1);

    }
    else
      LOGERROR1(F("Connected to"), _ssid1);
  }
  else
    LOGERROR1(F("Connected to"), _ssid);

  return connectResult;
}

//////////////////////////////////////////

int ESP_WiFiManager::connectWifi(const String& ssid, const String& pass)
{
  // Add option if didn't input/update SSID/PW => Use the previous saved Credentials.
  // But update the Static/DHCP options if changed.
  if ( (ssid != "") || ( (ssid == "") && (WiFi_SSID() != "") ) )
  {
    //fix for auto connect racing issue, to avoid resetSettings()
    if (WiFi.status() == WL_CONNECTED)
    {
      LOGWARN(F("Already connected. Bailing out."));
      return WL_CONNECTED;
    }

    if (ssid != "")
      resetSettings();

#ifdef ESP8266
    setWifiStaticIP();
#endif

    WiFi.mode(WIFI_AP_STA); //It will start in station mode if it was previously in AP mode.

    setHostname();

    // KH, Fix ESP32 staticIP after exiting CP
#ifdef ESP32
    setWifiStaticIP();
#endif

    if (ssid != "")
    {
      // Start Wifi with new values.
      LOGWARN(F("Connect to new WiFi using new IP parameters"));

      WiFi.begin(ssid.c_str(), pass.c_str());
    }
    else
    {
      // Start Wifi with old values.
      LOGWARN(F("Connect to previous WiFi using new IP parameters"));

      WiFi.begin();
    }
  }
  else if (WiFi_SSID() == "")
  {
    LOGWARN(F("No saved credentials"));
  }

  int connRes = waitForConnectResult();

  LOGWARN1("Connection result: ", getStatus(connRes));

  //not connected, WPS enabled, no pass - first attempt
  if (_tryWPS && connRes != WL_CONNECTED && pass == "")
  {
    startWPS();
    //should be connected at the end of WPS
    connRes = waitForConnectResult();
  }

  return connRes;
}

//////////////////////////////////////////

uint8_t ESP_WiFiManager::waitForConnectResult()
{
  if (_connectTimeout == 0)
  {
    unsigned long startedAt = millis();

    // In ESP8266, WiFi.waitForConnectResult() @return wl_status_t (0-255) or -1 on timeout !!!
    // In ESP32, WiFi.waitForConnectResult() @return wl_status_t (0-255)
    // So, using int for connRes to be safe
    //int connRes = WiFi.waitForConnectResult();
    WiFi.waitForConnectResult();

    float waited = (millis() - startedAt);

    LOGWARN1(F("Connected after waiting (s) :"), waited / 1000);
    LOGWARN1(F("Local ip ="), WiFi.localIP());

    // Fix bug, connRes is sometimes not correct.
    //return connRes;
    return WiFi.status();
  }
  else
  {
    LOGERROR(F("Waiting WiFi connection with time out"));

    unsigned long start = millis();
    bool keepConnecting = true;
    uint8_t status;

    while (keepConnecting)
    {
      status = WiFi.status();

      if (millis() > start + _connectTimeout)
      {
        keepConnecting = false;
        LOGERROR(F("Connection timed out"));
      }

      if (status == WL_CONNECTED || status == WL_CONNECT_FAILED)
      {
        keepConnecting = false;
      }

      delay(100);
    }

    return status;
  }
}

//////////////////////////////////////////

void ESP_WiFiManager::startWPS()
{
#ifdef ESP8266
  LOGINFO("START WPS");
  WiFi.beginWPSConfig();
  LOGINFO("END WPS");
#else   //ESP32
  // TODO
  LOGINFO("ESP32 WPS TODO");
#endif
}

//////////////////////////////////////////

//Convenient for debugging but wasteful of program space.
//Remove if short of space
const char* ESP_WiFiManager::getStatus(const int& status)
{
  switch (status)
  {
    case WL_IDLE_STATUS:
      return "WL_IDLE_STATUS";

    case WL_NO_SSID_AVAIL:
      return "WL_NO_SSID_AVAIL";

    case WL_CONNECTED:
      return "WL_CONNECTED";

    case WL_CONNECT_FAILED:
      return "WL_CONNECT_FAILED";

    case WL_DISCONNECTED:
      return "WL_DISCONNECTED";

    default:
      return "UNKNOWN";
  }
}

//////////////////////////////////////////

String ESP_WiFiManager::getConfigPortalSSID()
{
  return _apName;
}

//////////////////////////////////////////

String ESP_WiFiManager::getConfigPortalPW()
{
  return _apPassword;
}

//////////////////////////////////////////

void ESP_WiFiManager::resetSettings()
{
  LOGINFO(F("Previous settings invalidated"));

#ifdef ESP8266
  WiFi.disconnect(true);
#else
  WiFi.disconnect(true, true);

  // Temporary fix for issue of not clearing WiFi SSID/PW from flash of ESP32
  // See https://github.com/khoih-prog/ESP_WiFiManager/issues/25 and https://github.com/espressif/arduino-esp32/issues/400
  WiFi.begin("0", "0");
  //////
#endif

  delay(200);

  return;
}

//////////////////////////////////////////

void ESP_WiFiManager::setTimeout(const unsigned long& seconds)
{
  setConfigPortalTimeout(seconds);
}

//////////////////////////////////////////

void ESP_WiFiManager::setConfigPortalTimeout(const unsigned long& seconds)
{
  _configPortalTimeout = seconds * 1000;
}

//////////////////////////////////////////

void ESP_WiFiManager::setConnectTimeout(const unsigned long& seconds)
{
  _connectTimeout = seconds * 1000;
}

//////////////////////////////////////////

void ESP_WiFiManager::setDebugOutput(bool debug)
{
  _debug = debug;
}

//////////////////////////////////////////

// KH, To enable dynamic/random channel
int ESP_WiFiManager::setConfigPortalChannel(const int& channel)
{
  // If channel < MIN_WIFI_CHANNEL - 1 or channel > MAX_WIFI_CHANNEL => channel = 1
  // If channel == 0 => will use random channel from MIN_WIFI_CHANNEL to MAX_WIFI_CHANNEL
  // If (MIN_WIFI_CHANNEL <= channel <= MAX_WIFI_CHANNEL) => use it
  if ( (channel < MIN_WIFI_CHANNEL - 1) || (channel > MAX_WIFI_CHANNEL) )
    _WiFiAPChannel = 1;
  else if ( (channel >= MIN_WIFI_CHANNEL - 1) && (channel <= MAX_WIFI_CHANNEL) )
    _WiFiAPChannel = channel;

  return _WiFiAPChannel;
}

//////////////////////////////////////////

void ESP_WiFiManager::setAPStaticIPConfig(const IPAddress& ip, const IPAddress& gw, const IPAddress& sn)
{
  LOGINFO(F("setAPStaticIPConfig"));
  _WiFi_AP_IPconfig._ap_static_ip = ip;
  _WiFi_AP_IPconfig._ap_static_gw = gw;
  _WiFi_AP_IPconfig._ap_static_sn = sn;
}

//////////////////////////////////////////

void ESP_WiFiManager::setAPStaticIPConfig(const WiFi_AP_IPConfig&  WM_AP_IPconfig)
{
  LOGINFO(F("setAPStaticIPConfig"));

  memcpy((void*) &_WiFi_AP_IPconfig, &WM_AP_IPconfig, sizeof(_WiFi_AP_IPconfig));
}

//////////////////////////////////////////

void ESP_WiFiManager::getAPStaticIPConfig(WiFi_AP_IPConfig  &WM_AP_IPconfig)
{
  LOGINFO(F("getAPStaticIPConfig"));

  memcpy((void*) &WM_AP_IPconfig, &_WiFi_AP_IPconfig, sizeof(WM_AP_IPconfig));
}

//////////////////////////////////////////

void ESP_WiFiManager::setSTAStaticIPConfig(const IPAddress& ip, const IPAddress& gw, const IPAddress& sn)
{
  LOGINFO(F("setSTAStaticIPConfig"));
  _WiFi_STA_IPconfig._sta_static_ip = ip;
  _WiFi_STA_IPconfig._sta_static_gw = gw;
  _WiFi_STA_IPconfig._sta_static_sn = sn;
}

//////////////////////////////////////////

void ESP_WiFiManager::setSTAStaticIPConfig(const WiFi_STA_IPConfig& WM_STA_IPconfig)
{
  LOGINFO(F("setSTAStaticIPConfig"));

  memcpy((void*) &_WiFi_STA_IPconfig, &WM_STA_IPconfig, sizeof(_WiFi_STA_IPconfig));
}

//////////////////////////////////////////

void ESP_WiFiManager::getSTAStaticIPConfig(WiFi_STA_IPConfig &WM_STA_IPconfig)
{
  LOGINFO(F("getSTAStaticIPConfig"));

  memcpy((void*) &WM_STA_IPconfig, &_WiFi_STA_IPconfig, sizeof(WM_STA_IPconfig));
}

//////////////////////////////////////////

#if USE_CONFIGURABLE_DNS
void ESP_WiFiManager::setSTAStaticIPConfig(const IPAddress& ip, const IPAddress& gw, const IPAddress& sn,
                                           const IPAddress& dns_address_1, const IPAddress& dns_address_2)
{
  LOGINFO(F("setSTAStaticIPConfig for USE_CONFIGURABLE_DNS"));
  _WiFi_STA_IPconfig._sta_static_ip = ip;
  _WiFi_STA_IPconfig._sta_static_gw = gw;
  _WiFi_STA_IPconfig._sta_static_sn = sn;
  _WiFi_STA_IPconfig._sta_static_dns1 = dns_address_1; //***** Added argument *****
  _WiFi_STA_IPconfig._sta_static_dns2 = dns_address_2; //***** Added argument *****
}
#endif

//////////////////////////////////////////

void ESP_WiFiManager::setMinimumSignalQuality(const int& quality)
{
  _minimumQuality = quality;
}

//////////////////////////////////////////

void ESP_WiFiManager::setBreakAfterConfig(bool shouldBreak)
{
  _shouldBreakAfterConfig = shouldBreak;
}

//////////////////////////////////////////

void ESP_WiFiManager::reportStatus(String &page)
{
  page += FPSTR(WM_HTTP_SCRIPT_NTP_MSG);

  if (WiFi_SSID() != "")
  {
    page += F("Configured to connect to access point <b>");
    page += WiFi_SSID();

    if (WiFi.status() == WL_CONNECTED)
    {
      page += F(" and currently connected</b> on IP <a href=\"http://");
      page += WiFi.localIP().toString();
      page += F("/\">");
      page += WiFi.localIP().toString();
      page += F("</a>");
    }
    else
    {
      page += F(" but not currently connected</b> to network.");
    }
  }
  else
  {
    page += F("No network currently configured.");
  }
}

//////////////////////////////////////////

/** Handle root or redirect to captive portal */
void ESP_WiFiManager::handleRoot()
{
  LOGDEBUG(F("Handle root"));

  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;   //KH

  if (captivePortal())
  {
    // If caprive portal redirect instead of displaying the error page.
    return;
  }

  server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  // For configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  server->sendHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif

  server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");

  String page = FPSTR(WM_HTTP_HEAD_START);

  page.replace("{v}", "Options");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_SCRIPT_NTP);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += "<h2>";
  page += _apName;

  if (WiFi_SSID() != "")
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      page += " on ";
      page += WiFi_SSID();
    }
    else
    {
      page += " <s>on ";
      page += WiFi_SSID();
      page += "</s>";
    }
  }

  page += "</h2>";
  page += FPSTR(WM_HTTP_PORTAL_OPTIONS);
  page += F("<div class=\"msg\">");
  reportStatus(page);
  page += F("</div>");
  page += FPSTR(WM_HTTP_END);

  server->send(200, "text/html", page);

}

//////////////////////////////////////////

/** Wifi config page handler */
void ESP_WiFiManager::handleWifi()
{
  LOGDEBUG(F("Handle WiFi"));

  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;   //KH

  server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  // For configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  server->sendHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif

  server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");

  String page = FPSTR(WM_HTTP_HEAD_START);

  page.replace("{v}", "Config ESP");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_SCRIPT_NTP);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += F("<h2>Configuration</h2>");

  //  KH, New
  numberOfNetworks = scanWifiNetworks(&networkIndices);

  //Print list of WiFi networks that were found in earlier scan
  if (numberOfNetworks == 0)
  {
    page += F("No network found. Refresh to scan again.");
  }
  else
  {
    page += FPSTR(WM_FLDSET_START);

    //display networks in page
    for (int i = 0; i < numberOfNetworks; i++)
    {
      if (networkIndices[i] == -1)
        continue; // skip dups and those that are below the required quality

      LOGDEBUG1(F("Index ="), i);
      LOGDEBUG1(F("SSID ="), WiFi.SSID(networkIndices[i]));
      LOGDEBUG1(F("RSSI ="), WiFi.RSSI(networkIndices[i]));

      int quality = getRSSIasQuality(WiFi.RSSI(networkIndices[i]));

      String item = FPSTR(WM_HTTP_ITEM);
      String rssiQ;
      rssiQ += quality;
      item.replace("{v}", WiFi.SSID(networkIndices[i]));
      item.replace("{r}", rssiQ);

#ifdef ESP8266

      if (WiFi.encryptionType(networkIndices[i]) != ENC_TYPE_NONE)
#else   //ESP32
      if (WiFi.encryptionType(networkIndices[i]) != WIFI_AUTH_OPEN)
#endif
      {
        item.replace("{i}", "l");
      }
      else
      {
        item.replace("{i}", "");
      }

      //LOGDEBUG(item);

      page += item;
      delay(0);
    }

    page += FPSTR(WM_FLDSET_END);

    page += "<br/>";
  }

  page += "<small>*Hint: To reuse the saved WiFi credentials, leave SSID and PWD fields empty</small>";

  page += FPSTR(WM_HTTP_FORM_START);

#if DISPLAY_STORED_CREDENTIALS_IN_CP
  // Populate SSIDs and PWDs if valid
  page.replace("[[ssid]]",  _ssid );
  page.replace("[[pwd]]",   _pass );
  page.replace("[[ssid1]]", _ssid1 );
  page.replace("[[pwd1]]",  _pass1 );
#endif

  char parLength[2];

  page += FPSTR(WM_FLDSET_START);

  // add the extra parameters to the form
  for (int i = 0; i < _paramsCount; i++)
  {
    if (_params[i] == NULL)
    {
      break;
    }

    String pitem;

    switch (_params[i]->getLabelPlacement())
    {
      case WFM_LABEL_BEFORE:
        pitem = FPSTR(WM_HTTP_FORM_LABEL_BEFORE);
        break;

      case WFM_LABEL_AFTER:
        pitem = FPSTR(WM_HTTP_FORM_LABEL_AFTER);
        break;

      default:
        // WFM_NO_LABEL
        pitem = FPSTR(WM_HTTP_FORM_PARAM);
        break;
    }

    if (_params[i]->getID() != NULL)
    {
      pitem.replace("{i}", _params[i]->getID());
      pitem.replace("{n}", _params[i]->getID());
      pitem.replace("{p}", _params[i]->getPlaceholder());
      snprintf(parLength, 2, "%d", _params[i]->getValueLength());
      pitem.replace("{l}", parLength);
      pitem.replace("{v}", _params[i]->getValue());
      pitem.replace("{c}", _params[i]->getCustomHTML());
    }
    else
    {
      pitem = _params[i]->getCustomHTML();
    }

    page += pitem;
  }

  if (_paramsCount > 0)
  {
    page += FPSTR(WM_FLDSET_END);
  }

  if (_params[0] != NULL)
  {
    page += "<br/>";
  }

  LOGDEBUG1(F("Static IP ="), _WiFi_STA_IPconfig._sta_static_ip.toString());

  // KH, Comment out to permit changing from DHCP to static IP, or vice versa
  // and add staticIP label in CP

  // To permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
  // You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
  // You have to explicitly specify false to disable the feature.

#if !USE_STATIC_IP_CONFIG_IN_CP

  if (_WiFi_STA_IPconfig._sta_static_ip)
#endif
  {
    page += FPSTR(WM_FLDSET_START);

    String item = FPSTR(WM_HTTP_FORM_LABEL);
    item += FPSTR(WM_HTTP_FORM_PARAM);
    item.replace("{i}", "ip");
    item.replace("{n}", "ip");
    item.replace("{p}", "Static IP");
    item.replace("{l}", "15");
    item.replace("{v}", _WiFi_STA_IPconfig._sta_static_ip.toString());

    page += item;

    item = FPSTR(WM_HTTP_FORM_LABEL);
    item += FPSTR(WM_HTTP_FORM_PARAM);
    item.replace("{i}", "gw");
    item.replace("{n}", "gw");
    item.replace("{p}", "Gateway IP");
    item.replace("{l}", "15");
    item.replace("{v}", _WiFi_STA_IPconfig._sta_static_gw.toString());

    page += item;

    item = FPSTR(WM_HTTP_FORM_LABEL);
    item += FPSTR(WM_HTTP_FORM_PARAM);
    item.replace("{i}", "sn");
    item.replace("{n}", "sn");
    item.replace("{p}", "Subnet");
    item.replace("{l}", "15");
    item.replace("{v}", _WiFi_STA_IPconfig._sta_static_sn.toString());

#if USE_CONFIGURABLE_DNS
    //***** Added for DNS address options *****
    page += item;

    item = FPSTR(WM_HTTP_FORM_LABEL);
    item += FPSTR(WM_HTTP_FORM_PARAM);
    item.replace("{i}", "dns1");
    item.replace("{n}", "dns1");
    item.replace("{p}", "DNS1 IP");
    item.replace("{l}", "15");
    item.replace("{v}", _WiFi_STA_IPconfig._sta_static_dns1.toString());

    page += item;

    item = FPSTR(WM_HTTP_FORM_LABEL);
    item += FPSTR(WM_HTTP_FORM_PARAM);
    item.replace("{i}", "dns2");
    item.replace("{n}", "dns2");
    item.replace("{p}", "DNS2 IP");
    item.replace("{l}", "15");
    item.replace("{v}", _WiFi_STA_IPconfig._sta_static_dns2.toString());
    //***** End added for DNS address options *****
#endif

    page += item;

    page += FPSTR(WM_FLDSET_END);

    page += "<br/>";
  }

  page += FPSTR(WM_HTTP_SCRIPT_NTP_HIDDEN);

  page += FPSTR(WM_HTTP_FORM_END);

  page += FPSTR(WM_HTTP_END);

  server->send(200, "text/html", page);

  LOGDEBUG(F("Sent config page"));
}

//////////////////////////////////////////

/** Handle the WLAN save form and redirect to WLAN config page again */
void ESP_WiFiManager::handleWifiSave()
{
  LOGDEBUG(F("WiFi save"));

  //SAVE/connect here
  _ssid = server->arg("s").c_str();
  _pass = server->arg("p").c_str();

  _ssid1 = server->arg("s1").c_str();
  _pass1 = server->arg("p1").c_str();

  ///////////////////////

#if USING_CORS_FEATURE
  // For configuring CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  server->sendHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif

#if USE_ESP_WIFIMANAGER_NTP

  if (server->arg("timezone") != "")
  {
    _timezoneName = server->arg("timezone");
    LOGDEBUG1(F("TZ name ="), _timezoneName);
  }
  else
  {
    LOGDEBUG(F("No TZ arg"));
  }

#endif
  ///////////////////////

  //parameters
  for (int i = 0; i < _paramsCount; i++)
  {
    if (_params[i] == NULL)
    {
      break;
    }

    //read parameter
    String value = server->arg(_params[i]->getID()).c_str();

    //store it in array
    value.toCharArray(_params[i]->_WMParam_data._value, _params[i]->_WMParam_data._length);

    LOGDEBUG2(F("Parameter and value :"), _params[i]->getID(), value);
  }

  if (server->arg("ip") != "")
  {
    String ip = server->arg("ip");
    optionalIPFromString(&_WiFi_STA_IPconfig._sta_static_ip, ip.c_str());

    LOGDEBUG1(F("New Static IP ="), _WiFi_STA_IPconfig._sta_static_ip.toString());
  }

  if (server->arg("gw") != "")
  {
    String gw = server->arg("gw");
    optionalIPFromString(&_WiFi_STA_IPconfig._sta_static_gw, gw.c_str());

    LOGDEBUG1(F("New Static Gateway ="), _WiFi_STA_IPconfig._sta_static_gw.toString());
  }

  if (server->arg("sn") != "")
  {
    String sn = server->arg("sn");
    optionalIPFromString(&_WiFi_STA_IPconfig._sta_static_sn, sn.c_str());

    LOGDEBUG1(F("New Static Netmask ="), _WiFi_STA_IPconfig._sta_static_sn.toString());
  }

#if USE_CONFIGURABLE_DNS

  //*****  Added for DNS Options *****
  if (server->arg("dns1") != "")
  {
    String dns1 = server->arg("dns1");
    optionalIPFromString(&_WiFi_STA_IPconfig._sta_static_dns1, dns1.c_str());

    LOGDEBUG1(F("New Static DNS1 ="), _WiFi_STA_IPconfig._sta_static_dns1.toString());
  }

  if (server->arg("dns2") != "")
  {
    String dns2 = server->arg("dns2");
    optionalIPFromString(&_WiFi_STA_IPconfig._sta_static_dns2, dns2.c_str());

    LOGDEBUG1(F("New Static DNS2 ="), _WiFi_STA_IPconfig._sta_static_dns2.toString());
  }

  //*****  End added for DNS Options *****
#endif

  String page = FPSTR(WM_HTTP_HEAD_START);

  page.replace("{v}", "Credentials Saved");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += FPSTR(WM_HTTP_SAVED);
  page.replace("{v}", _apName);
  page.replace("{x}", _ssid);
  page.replace("{x1}", _ssid1);

  page += FPSTR(WM_HTTP_END);

  server->send(200, "text/html", page);

  LOGDEBUG(F("Sent wifi save page"));

  connect = true; //signal ready to connect/reset

  // Restore when Press Save WiFi
  _configPortalTimeout = DEFAULT_PORTAL_TIMEOUT;
}

//////////////////////////////////////////

/** Handle shut down the server page */
void ESP_WiFiManager::handleServerClose()
{
  LOGDEBUG(F("Server Close"));

  server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  // For configuring CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  server->sendHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif

  server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");

  String page = FPSTR(WM_HTTP_HEAD_START);

  page.replace("{v}", "Close Server");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += F("<div class=\"msg\">");
  page += F("My network is <b>");
  page += WiFi_SSID();
  page += F("</b><br>");
  page += F("IP address is <b>");
  page += WiFi.localIP().toString();
  page += F("</b><br><br>");
  page += F("Portal closed...<br><br>");

  //page += F("Push button on device to restart configuration server!");

  page += FPSTR(WM_HTTP_END);

  server->send(200, "text/html", page);

  stopConfigPortal = true; //signal ready to shutdown config portal

  LOGDEBUG(F("Sent server close page"));

  // Restore when Press Save WiFi
  _configPortalTimeout = DEFAULT_PORTAL_TIMEOUT;
}

//////////////////////////////////////////

/** Handle the info page */
void ESP_WiFiManager::handleInfo()
{
  LOGDEBUG(F("Info"));

  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;   //KH

  server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  // For configuring CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  server->sendHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif

  server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");

  String page = FPSTR(WM_HTTP_HEAD_START);

  page.replace("{v}", "Info");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_SCRIPT_NTP);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);

  page += F("<h2>WiFi Information</h2>");

  reportStatus(page);

  page += FPSTR(WM_FLDSET_START);

  page += F("<h3>Device Data</h3>");
  page += F("<table class=\"table\">");
  page += F("<thead><tr><th>Name</th><th>Value</th></tr></thead><tbody><tr><td>Chip ID</td><td>");

  page += F("0x");
#ifdef ESP8266
  page += String(ESP.getChipId(), HEX);   //ESP.getChipId();
#else   //ESP32

  page += String(ESP_getChipId(), HEX);   //ESP.getChipId();

  page += F("</td></tr>");
  page += F("<tr><td>Chip OUI</td><td>");
  page += F("0x");
  page += String(getChipOUI(), HEX);    //ESP.getChipId();

  page += F("</td></tr>");
  page += F("<tr><td>Chip Model</td><td>");
  page += ESP.getChipModel();
  page += F(" Rev");
  page += ESP.getChipRevision();
#endif

  page += F("</td></tr>");
  page += F("<tr><td>Flash Chip ID</td><td>");

#ifdef ESP8266
  page += String(ESP.getFlashChipId(), HEX);    //ESP.getFlashChipId();
#else   //ESP32
  // TODO
  page += F("TODO");
#endif

  page += F("</td></tr>");
  page += F("<tr><td>IDE Flash Size</td><td>");
  page += ESP.getFlashChipSize();
  page += F(" bytes</td></tr>");
  page += F("<tr><td>Real Flash Size</td><td>");

#ifdef ESP8266
  page += ESP.getFlashChipRealSize();
#else   //ESP32
  // TODO
  page += F("TODO");
#endif

  page += F(" bytes</td></tr>");
  page += F("<tr><td>Access Point IP</td><td>");
  page += WiFi.softAPIP().toString();
  page += F("</td></tr>");
  page += F("<tr><td>Access Point MAC</td><td>");
  page += WiFi.softAPmacAddress();
  page += F("</td></tr>");

  page += F("<tr><td>SSID</td><td>");
  page += WiFi_SSID();
  page += F("</td></tr>");

  page += F("<tr><td>Station IP</td><td>");
  page += WiFi.localIP().toString();
  page += F("</td></tr>");

  page += F("<tr><td>Station MAC</td><td>");
  page += WiFi.macAddress();
  page += F("</td></tr>");
  page += F("</tbody></table>");

  page += FPSTR(WM_FLDSET_END);

#if USE_AVAILABLE_PAGES
  page += FPSTR(WM_FLDSET_START);

  page += FPSTR(WM_HTTP_AVAILABLE_PAGES);

  page += FPSTR(WM_FLDSET_END);
#endif

  page += F("<p/>More information about ESP_WiFiManager at");
  page += F("<p/><a href=\"https://github.com/khoih-prog/ESP_WiFiManager\">https://github.com/khoih-prog/ESP_WiFiManager</a>");
  page += FPSTR(WM_HTTP_END);

  server->send(200, "text/html", page);

  LOGDEBUG(F("Sent info page"));
}

//////////////////////////////////////////

/** Handle the state page */
void ESP_WiFiManager::handleState()
{
  LOGDEBUG(F("State - json"));

  server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  // For configuring CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  server->sendHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif

  server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");

  String page = F("{\"Soft_AP_IP\":\"");

  page += WiFi.softAPIP().toString();
  page += F("\",\"Soft_AP_MAC\":\"");
  page += WiFi.softAPmacAddress();
  page += F("\",\"Station_IP\":\"");
  page += WiFi.localIP().toString();
  page += F("\",\"Station_MAC\":\"");
  page += WiFi.macAddress();
  page += F("\",");

  if (WiFi.psk() != "")
  {
    page += F("\"Password\":true,");
  }
  else
  {
    page += F("\"Password\":false,");
  }

  page += F("\"SSID\":\"");
  page += WiFi_SSID();
  page += F("\"}");

  server->send(200, "application/json", page);

  LOGDEBUG(F("Sent state page in json format"));
}

//////////////////////////////////////////

/** Handle the scan page */
void ESP_WiFiManager::handleScan()
{
  LOGDEBUG(F("Scan"));

  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;   //KH

  LOGDEBUG(F("State-Json"));

  server->sendHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));

#if USING_CORS_FEATURE
  // For configuring CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  server->sendHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif

  server->sendHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  server->sendHeader(FPSTR(WM_HTTP_EXPIRES), "-1");

  int n;
  int *indices;

  //Space for indices array allocated on heap in scanWifiNetworks
  //and should be freed when indices no longer required.

  n = scanWifiNetworks(&indices);

  LOGDEBUG(F("In handleScan, scanWifiNetworks done"));

  String page = F("{\"Access_Points\":[");

  //display networks in page
  for (int i = 0; i < n; i++)
  {
    if (indices[i] == -1)
      continue; // skip duplicates and those that are below the required quality

    if (i != 0)
      page += F(", ");

    LOGDEBUG1(F("Index ="), i);
    LOGDEBUG1(F("SSID ="), WiFi.SSID(indices[i]));
    LOGDEBUG1(F("RSSI ="), WiFi.RSSI(indices[i]));

    int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));
    String item = FPSTR(JSON_ITEM);
    String rssiQ;

    rssiQ += quality;
    item.replace("{v}", WiFi.SSID(indices[i]));
    item.replace("{r}", rssiQ);

#ifdef ESP8266

    if (WiFi.encryptionType(indices[i]) != ENC_TYPE_NONE)
#else   //ESP32
    if (WiFi.encryptionType(indices[i]) != WIFI_AUTH_OPEN)
#endif
    {
      item.replace("{i}", "true");
    }
    else
    {
      item.replace("{i}", "false");
    }

    //LOGDEBUG(item);

    page += item;
    delay(0);
  }

  if (indices)
  {
    free(indices); //indices array no longer required so free memory
  }

  page += F("]}");

  server->send(200, "application/json", page);

  LOGDEBUG(F("Sent WiFiScan Data in Json format"));
}

//////////////////////////////////////////

/** Handle the reset page */
void ESP_WiFiManager::handleReset()
{
  LOGDEBUG(F("Reset"));

  server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server->sendHeader("Pragma", "no-cache");
  server->sendHeader("Expires", "-1");

  String page = FPSTR(WM_HTTP_HEAD_START);

  page.replace("{v}", "WiFi Information");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += F("Resetting");
  page += FPSTR(WM_HTTP_END);

  server->send(200, "text/html", page);

  LOGDEBUG(F("Sent reset page"));
  delay(5000);

  // Temporary fix for issue of not clearing WiFi SSID/PW from flash of ESP32
  // See https://github.com/khoih-prog/ESP_WiFiManager/issues/25 and https://github.com/espressif/arduino-esp32/issues/400
  resetSettings();

  //WiFi.disconnect(true); // Wipe out WiFi credentials.
  //////

#ifdef ESP8266
  ESP.reset();
#else   //ESP32
  ESP.restart();
#endif

  delay(2000);
}

//////////////////////////////////////////

void ESP_WiFiManager::handleNotFound()
{
  if (captivePortal())
  {
    // If caprive portal redirect instead of displaying the error page.
    return;
  }

  String message = "File Not Found\n\n";

  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += (server->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";

  for (uint8_t i = 0; i < server->args(); i++)
  {
    message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
  }

  server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server->sendHeader("Pragma", "no-cache");
  server->sendHeader("Expires", "-1");

  server->send(404, "text/plain", message);
}

//////////////////////////////////////////

/**
   HTTPD redirector
   Redirect to captive portal if we got a request for another domain.
   Return true in that case so the page handler do not try to handle the request again.
*/
bool ESP_WiFiManager::captivePortal()
{
  LOGDEBUG1(F("captivePortal: hostHeader = "), server->hostHeader());

  if (!isIp(server->hostHeader()))
  {
    LOGINFO1(F("Request redirected to captive portal : "), server->client().localIP());

    server->sendHeader(F("Location"), (String)F("http://") + toStringIp(server->client().localIP()), true);

    // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server->send(302, FPSTR(WM_HTTP_HEAD_CT2), "");

    server->client().stop(); // Stop is needed because we sent no content length

    return true;
  }

  return false;
}

//////////////////////////////////////////

//start up config portal callback
void ESP_WiFiManager::setAPCallback(void(*func)(ESP_WiFiManager* myWiFiManager))
{
  _apcallback = func;
}

//////////////////////////////////////////

//start up save config callback
void ESP_WiFiManager::setSaveConfigCallback(void(*func)())
{
  _savecallback = func;
}

//////////////////////////////////////////

//sets a custom element to add to head, like a new style tag
void ESP_WiFiManager::setCustomHeadElement(const char* element)
{
  _customHeadElement = element;
}

//if this is true, remove duplicated Access Points - defaut true
void ESP_WiFiManager::setRemoveDuplicateAPs(bool removeDuplicates)
{
  _removeDuplicateAPs = removeDuplicates;
}

//////////////////////////////////////////

//Scan for WiFiNetworks in range and sort by signal strength
//space for indices array allocated on the heap and should be freed when no longer required
int ESP_WiFiManager::scanWifiNetworks(int **indicesptr)
{
  LOGDEBUG(F("Scanning Network"));

  int n = WiFi.scanNetworks(false, true);

  LOGDEBUG1(F("scanWifiNetworks: Done, Scanned Networks n ="), n);

  //KH, Terrible bug here. WiFi.scanNetworks() returns n < 0 => malloc( negative == very big ) => crash!!!
  //In .../esp32/libraries/WiFi/src/WiFiType.h
  //#define WIFI_SCAN_RUNNING   (-1)
  //#define WIFI_SCAN_FAILED    (-2)
  //if (n == 0)
  if (n <= 0)
  {
    LOGDEBUG(F("No network found"));

    return (0);
  }
  else
  {
    // Allocate space off the heap for indices array.
    // This space should be freed when no longer required.
    int* indices = (int *)malloc(n * sizeof(int));

    if (indices == NULL)
    {
      LOGDEBUG(F("ERROR: Out of memory"));
      *indicesptr = NULL;

      return (0);
    }

    *indicesptr = indices;

    //sort networks
    for (int i = 0; i < n; i++)
    {
      indices[i] = i;
    }

    LOGDEBUG(F("Sorting"));

    // RSSI SORT
    // old sort
    for (int i = 0; i < n; i++)
    {
      for (int j = i + 1; j < n; j++)
      {
        if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i]))
        {
          std::swap(indices[i], indices[j]);
        }
      }
    }

    LOGDEBUG(F("Removing Dup"));

    // remove duplicates ( must be RSSI sorted )
    if (_removeDuplicateAPs)
    {
      String cssid;

      for (int i = 0; i < n; i++)
      {
        if (indices[i] == -1)
          continue;

        cssid = WiFi.SSID(indices[i]);

        for (int j = i + 1; j < n; j++)
        {
          if (cssid == WiFi.SSID(indices[j]))
          {
            LOGDEBUG1("DUP AP:", WiFi.SSID(indices[j]));
            indices[j] = -1; // set dup aps to index -1
          }
        }
      }
    }

    for (int i = 0; i < n; i++)
    {
      if (indices[i] == -1)
        continue; // skip dups

      int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));

      if (!(_minimumQuality == -1 || _minimumQuality < quality))
      {
        indices[i] = -1;
        LOGDEBUG(F("Skipping low quality"));
      }
    }

#if (DEBUG_WIFIMGR > 2)

    for (int i = 0; i < n; i++)
    {
      if (indices[i] == -1)
        continue; // skip dups
      else
        Serial.println(WiFi.SSID(indices[i]));
    }

#endif

    return (n);
  }
}

//////////////////////////////////////////

int ESP_WiFiManager::getRSSIasQuality(const int& RSSI)
{
  int quality = 0;

  if (RSSI <= -100)
  {
    quality = 0;
  }
  else if (RSSI >= -50)
  {
    quality = 100;
  }
  else
  {
    quality = 2 * (RSSI + 100);
  }

  return quality;
}

//////////////////////////////////////////

/** Is this an IP? */
bool ESP_WiFiManager::isIp(const String& str)
{
  for (unsigned int i = 0; i < str.length(); i++)
  {
    int c = str.charAt(i);

    if (c != '.' && c != ':' && (c < '0' || c > '9'))
    {
      return false;
    }
  }

  return true;
}

//////////////////////////////////////////

/** IP to String? */
String ESP_WiFiManager::toStringIp(const IPAddress& ip)
{
  String res = "";

  for (int i = 0; i < 3; i++)
  {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }

  res += String(((ip >> 8 * 3)) & 0xFF);

  return res;
}

//////////////////////////////////////////

#ifdef ESP32
// We can't use WiFi.SSID() in ESP32 as it's only valid after connected.
// SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
// Have to create a new function to store in EEPROM/SPIFFS for this purpose

String ESP_WiFiManager::getStoredWiFiSSID()
{
  if (WiFi.getMode() == WIFI_MODE_NULL)
  {
    return String();
  }

  wifi_ap_record_t info;

  if (!esp_wifi_sta_get_ap_info(&info))
  {
    return String(reinterpret_cast<char*>(info.ssid));
  }
  else
  {
    wifi_config_t conf;

    esp_wifi_get_config(WIFI_IF_STA, &conf);

    return String(reinterpret_cast<char*>(conf.sta.ssid));
  }

  return String();
}

//////////////////////////////////////////

String ESP_WiFiManager::getStoredWiFiPass()
{
  if (WiFi.getMode() == WIFI_MODE_NULL)
  {
    return String();
  }

  wifi_config_t conf;

  esp_wifi_get_config(WIFI_IF_STA, &conf);

  return String(reinterpret_cast<char*>(conf.sta.password));
}

//////////////////////////////////////////

uint32_t getChipID()
{
  uint64_t chipId64 = 0;

  for (int i = 0; i < 6; i++)
  {
    chipId64 |= ( ( (uint64_t) ESP.getEfuseMac() >> (40 - (i * 8)) ) & 0xff ) << (i * 8);
  }

  return (uint32_t) (chipId64 & 0xFFFFFF);
}

//////////////////////////////////////////

uint32_t getChipOUI()
{
  uint64_t chipId64 = 0;

  for (int i = 0; i < 6; i++)
  {
    chipId64 |= ( ( (uint64_t) ESP.getEfuseMac() >> (40 - (i * 8)) ) & 0xff ) << (i * 8);
  }

  return (uint32_t) (chipId64 >> 24);
}

//////////////////////////////////////////

#endif    // #ifdef ESP32

//////////////////////////////////////////

#endif    //ESP_WiFiManager_Impl_h
