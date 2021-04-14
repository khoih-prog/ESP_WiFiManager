/****************************************************************************************************************************
  ESPAsync_WiFiManager-Impl.h
  For ESP8266 / ESP32 boards

  ESPAsync_WiFiManager is a library for the ESP8266/Arduino platform, using (ESP)AsyncWebServer to enable easy
  configuration and reconfiguration of WiFi credentials using a Captive Portal.

  Modified from 
  1. Tzapu               (https://github.com/tzapu/WiFiManager)
  2. Ken Taylor          (https://github.com/kentaylor)
  3. Alan Steremberg     (https://github.com/alanswx/ESPAsyncWiFiManager)
  4. Khoi Hoang          (https://github.com/khoih-prog/ESP_WiFiManager)

  Built by Khoi Hoang https://github.com/khoih-prog/ESPAsync_WiFiManager
  Licensed under MIT license
  Version: 1.6.3

  Version Modified By  Date      Comments
  ------- -----------  ---------- -----------
  1.0.11  K Hoang      21/08/2020 Initial coding to use (ESP)AsyncWebServer instead of (ESP8266)WebServer. Bump up to v1.0.11
                                  to sync with ESP_WiFiManager v1.0.11
  1.1.1   K Hoang      29/08/2020 Add MultiWiFi feature to autoconnect to best WiFi at runtime to sync with
                                  ESP_WiFiManager v1.1.1. Add setCORSHeader function to allow flexible CORS
  1.1.2   K Hoang      17/09/2020 Fix bug in examples.
  1.2.0   K Hoang      15/10/2020 Restore cpp code besides Impl.h code to use if linker error. Fix bug.
  1.3.0   K Hoang      04/12/2020 Add LittleFS support to ESP32 using LITTLEFS Library
  1.4.0   K Hoang      18/12/2020 Fix staticIP not saved. Add functions. Add complex examples.
  1.4.1   K Hoang      21/12/2020 Fix bug and compiler warnings.
  1.4.2   K Hoang      21/12/2020 Fix examples' bug not using saved WiFi Credentials after losing all WiFi connections.
  1.4.3   K Hoang      23/12/2020 Fix examples' bug not saving Static IP in certain cases.
  1.5.0   K Hoang      13/02/2021 Add support to new ESP32-S2. Optimize code.
  1.6.0   K Hoang      25/02/2021 Fix WiFi Scanning bug.
  1.6.1   K Hoang      26/03/2021 Modify multiWiFi-related timings to work better with latest esp32 core v1.0.6
  1.6.2   K Hoang      08/04/2021 Fix example misleading messages.
  1.6.3   K Hoang      13/04/2021 Allow captive portal to run more than once by closing dnsServer.
 *****************************************************************************************************************************/

#include "ESPAsync_WiFiManager.h"

/////////////////////////////////////////////////////////////////////////////

ESPAsync_WMParameter::ESPAsync_WMParameter(const char *custom)
{
  _WMParam_data._id = NULL;
  _WMParam_data._placeholder = NULL;
  _WMParam_data._length = 0;
  _WMParam_data._value = NULL;
  _WMParam_data._labelPlacement = WFM_LABEL_BEFORE;

  _customHTML = custom;
}

ESPAsync_WMParameter::ESPAsync_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom, int labelPlacement)
{
  init(id, placeholder, defaultValue, length, custom, labelPlacement);
}

// KH, using struct                      
ESPAsync_WMParameter::ESPAsync_WMParameter(WMParam_Data WMParam_data)
{
  init(WMParam_data._id, WMParam_data._placeholder, WMParam_data._value, WMParam_data._length, "", WMParam_data._labelPlacement);
}                  
//////

void ESPAsync_WMParameter::init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom, int labelPlacement)
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

ESPAsync_WMParameter::~ESPAsync_WMParameter()
{
  if (_WMParam_data._value != NULL)
  {
    delete[] _WMParam_data._value;
  }
}

// Using Struct to get/set whole data at once
void ESPAsync_WMParameter::setWMParam_Data(WMParam_Data WMParam_data)
{
  LOGINFO(F("setWMParam_Data"));
  
  memcpy(&_WMParam_data, &WMParam_data, sizeof(_WMParam_data));
}

void ESPAsync_WMParameter::getWMParam_Data(WMParam_Data &WMParam_data)
{
  LOGINFO(F("getWMParam_Data"));
  
  memcpy(&WMParam_data, &_WMParam_data, sizeof(WMParam_data));
}
//////

const char* ESPAsync_WMParameter::getValue()
{
  return _WMParam_data._value;
}

const char* ESPAsync_WMParameter::getID()
{
  return _WMParam_data._id;
}
const char* ESPAsync_WMParameter::getPlaceholder()
{
  return _WMParam_data._placeholder;
}

int ESPAsync_WMParameter::getValueLength()
{
  return _WMParam_data._length;
}

int ESPAsync_WMParameter::getLabelPlacement()
{
  return _WMParam_data._labelPlacement;
}
const char* ESPAsync_WMParameter::getCustomHTML()
{
  return _customHTML;
}

/**
   [getParameters description]
   @access public
*/
ESPAsync_WMParameter** ESPAsync_WiFiManager::getParameters() 
{
  return _params;
}

/////////////////////////////////////////////////////////////////////////////

/**
   [getParametersCount description]
   @access public
*/
int ESPAsync_WiFiManager::getParametersCount() 
{
  return _paramsCount;
}

//////////////////////////////////////////

char* ESPAsync_WiFiManager::getRFC952_hostname(const char* iHostname)
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

ESPAsync_WiFiManager::ESPAsync_WiFiManager(AsyncWebServer * webserver, DNSServer *dnsserver, const char *iHostname)
{

  server    = webserver;
  dnsServer = dnsserver;
  
  wifiSSIDs     = NULL;
  
  // KH
  wifiSSIDscan  = true;
  //wifiSSIDscan  = false;
  //////
  
  _modeless     = false;
  shouldscan    = true;
  
#if USE_DYNAMIC_PARAMS
  _max_params = WIFI_MANAGER_MAX_PARAMS;
  _params = (ESPAsync_WMParameter**)malloc(_max_params * sizeof(ESPAsync_WMParameter*));
#endif

  //WiFi not yet started here, must call WiFi.mode(WIFI_STA) and modify function WiFiGenericClass::mode(wifi_mode_t m) !!!

  WiFi.mode(WIFI_STA);

  if (iHostname[0] == 0)
  {
#ifdef ESP8266
    String _hostname = "ESP8266-" + String(ESP.getChipId(), HEX);
#else		//ESP32
    String _hostname = "ESP32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
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

ESPAsync_WiFiManager::~ESPAsync_WiFiManager()
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
bool ESPAsync_WiFiManager::addParameter(ESPAsync_WMParameter *p)
#else
void ESPAsync_WiFiManager::addParameter(ESPAsync_WMParameter *p)
#endif
{
#if USE_DYNAMIC_PARAMS

  if (_paramsCount == _max_params)
  {
    // rezise the params array
    _max_params += WIFI_MANAGER_MAX_PARAMS;
    
    LOGINFO1(F("Increasing _max_params to:"), _max_params);
    
    ESPAsync_WMParameter** new_params = (ESPAsync_WMParameter**)realloc(_params, _max_params * sizeof(ESPAsync_WMParameter*));

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

void ESPAsync_WiFiManager::setupConfigPortal()
{
  stopConfigPortal = false; //Signal not to close config portal

  /*This library assumes autoconnect is set to 1. It usually is
    but just in case check the setting and turn on autoconnect if it is off.
    Some useful discussion at https://github.com/esp8266/Arduino/issues/1615*/
  if (WiFi.getAutoConnect() == 0)
    WiFi.setAutoConnect(1);

#if !( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 ) 
  #ifdef ESP8266
    // KH, mod for Async
    server->reset();
  #else		//ESP32
    server->reset();
  #endif

  if (!dnsServer)
    dnsServer = new DNSServer;
#endif    // ARDUINO_ESP32S2_DEV

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

  _configPortalStart = millis();

  LOGWARN1(F("\nConfiguring AP SSID ="), _apName);

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
  
  if (_apPassword != NULL)
  {
    LOGWARN1(F("AP Channel ="), channel);
    
    //WiFi.softAP(_apName, _apPassword);//password option
    WiFi.softAP(_apName, _apPassword, channel);
  }
  else
  {
    // Can't use channel here
    WiFi.softAP(_apName);
  }
  //////
  
  // Contributed by AlesSt (https://github.com/AlesSt) to solve issue softAP with custom IP sometimes not working
  // See https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/26 and https://github.com/espressif/arduino-esp32/issues/985
  // delay 100ms to wait for SYSTEM_EVENT_AP_START
  delay(100);
  //////
  
  //optional soft ip config
  if (_WiFi_AP_IPconfig._ap_static_ip)
  {
    LOGWARN3(F("Custom AP IP/GW/Subnet = "), _WiFi_AP_IPconfig._ap_static_ip, _WiFi_AP_IPconfig._ap_static_gw, _WiFi_AP_IPconfig._ap_static_sn);
    
    WiFi.softAPConfig(_WiFi_AP_IPconfig._ap_static_ip, _WiFi_AP_IPconfig._ap_static_gw, _WiFi_AP_IPconfig._ap_static_sn);
  }

  delay(500); // Without delay I've seen the IP address blank
  
  LOGWARN1(F("AP IP address ="), WiFi.softAPIP());

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  
  server->on("/",         std::bind(&ESPAsync_WiFiManager::handleRoot,        this, std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/wifi",     std::bind(&ESPAsync_WiFiManager::handleWifi,        this, std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/wifisave", std::bind(&ESPAsync_WiFiManager::handleWifiSave,    this, std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/close",    std::bind(&ESPAsync_WiFiManager::handleServerClose, this, std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/i",        std::bind(&ESPAsync_WiFiManager::handleInfo,        this, std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/r",        std::bind(&ESPAsync_WiFiManager::handleReset,       this, std::placeholders::_1)).setFilter(ON_AP_FILTER);
  server->on("/state",    std::bind(&ESPAsync_WiFiManager::handleState,       this, std::placeholders::_1)).setFilter(ON_AP_FILTER);
  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server->on("/fwlink",   std::bind(&ESPAsync_WiFiManager::handleRoot,        this, std::placeholders::_1)).setFilter(ON_AP_FILTER);  
  server->onNotFound (std::bind(&ESPAsync_WiFiManager::handleNotFound,        this, std::placeholders::_1));
  
  server->begin(); // Web server start
  
  LOGWARN(F("HTTP server started"));
}

//////////////////////////////////////////

bool ESPAsync_WiFiManager::autoConnect()
{
#ifdef ESP8266
  String ssid = "ESP_" + String(ESP.getChipId());
#else		//ESP32
  String ssid = "ESP_" + String((uint32_t)ESP.getEfuseMac());
#endif

  return autoConnect(ssid.c_str(), NULL);
}

/* This is not very useful as there has been an assumption that device has to be
  told to connect but Wifi already does it's best to connect in background. Calling this
  method will block until WiFi connects. Sketch can avoid
  blocking call then use (WiFi.status()==WL_CONNECTED) test to see if connected yet.
  See some discussion at https://github.com/tzapu/WiFiManager/issues/68
*/


//////////////////////////////////////////

bool ESPAsync_WiFiManager::autoConnect(char const *apName, char const *apPassword)
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
    //delay(100);
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


///////////////////////////////////////////////////////////////////
// NEW

String ESPAsync_WiFiManager::networkListAsString()
{
  String pager ;
  
  //display networks in page
  for (int i = 0; i < wifiSSIDCount; i++) 
  {
    if (wifiSSIDs[i].duplicate == true) 
      continue; // skip dups
      
    int quality = getRSSIasQuality(wifiSSIDs[i].RSSI);

    if (_minimumQuality == -1 || _minimumQuality < quality) 
    {
      String item = FPSTR(WM_HTTP_ITEM);
      String rssiQ;
      
      rssiQ += quality;
      item.replace("{v}", wifiSSIDs[i].SSID);
      item.replace("{r}", rssiQ);
      
#if defined(ESP8266)
      if (wifiSSIDs[i].encryptionType != ENC_TYPE_NONE)
#else
      if (wifiSSIDs[i].encryptionType != WIFI_AUTH_OPEN)
#endif
      {
        item.replace("{i}", "l");
      } 
      else 
      {
        item.replace("{i}", "");
      }
      
      pager += item;

    } 
    else 
    {
      LOGDEBUG(F("Skipping due to quality"));
    }

  }
  
  return pager;
}

//////////////////////////////////////////

String ESPAsync_WiFiManager::scanModal()
{
  shouldscan = true;
  scan();
  
  String pager = networkListAsString();
  
  return pager;
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::scan()
{
  if (!shouldscan) 
    return;
  
  LOGDEBUG(F("scan: About to scan()"));
  
  if (wifiSSIDscan)
  {
    delay(100);
  }

  if (wifiSSIDscan)
  {
    LOGDEBUG(F("scan: Start scan()"));
    wifi_ssid_count_t n = WiFi.scanNetworks();
    LOGDEBUG(F("scan: Scan done"));
    
    if (n == WIFI_SCAN_FAILED) 
    {
      LOGDEBUG(F("scan: WIFI_SCAN_FAILED!"));
    }
    else if (n == WIFI_SCAN_RUNNING) 
    {
      LOGDEBUG(F("scan: WIFI_SCAN_RUNNING!"));
    } 
    else if (n < 0) 
    {
      LOGDEBUG(F("scan: Failed with unknown error code!"));
    } 
    else if (n == 0) 
    {
      LOGDEBUG(F("scan: No networks found"));
      // page += F("No networks found. Refresh to scan again.");
    } 
    else 
    {
      if (wifiSSIDscan)
      {
        /* WE SHOULD MOVE THIS IN PLACE ATOMICALLY */
        if (wifiSSIDs) 
          delete [] wifiSSIDs;
          
        wifiSSIDs     = new WiFiResult[n];
        wifiSSIDCount = n;

        if (n > 0)
          shouldscan = false;

        for (wifi_ssid_count_t i = 0; i < n; i++)
        {
          wifiSSIDs[i].duplicate=false;

#if defined(ESP8266)
          WiFi.getNetworkInfo(i, wifiSSIDs[i].SSID, wifiSSIDs[i].encryptionType, wifiSSIDs[i].RSSI, wifiSSIDs[i].BSSID, wifiSSIDs[i].channel, wifiSSIDs[i].isHidden);
#else
          WiFi.getNetworkInfo(i, wifiSSIDs[i].SSID, wifiSSIDs[i].encryptionType, wifiSSIDs[i].RSSI, wifiSSIDs[i].BSSID, wifiSSIDs[i].channel);
 #endif
        }

        // RSSI SORT
        // old sort
        for (int i = 0; i < n; i++) 
        {
          for (int j = i + 1; j < n; j++) 
          {
            if (wifiSSIDs[j].RSSI > wifiSSIDs[i].RSSI) 
            {
              std::swap(wifiSSIDs[i], wifiSSIDs[j]);
            }
          }
        }

        // remove duplicates ( must be RSSI sorted )
        if (_removeDuplicateAPs) 
        {
        String cssid;
        
        for (int i = 0; i < n; i++) 
        {
          if (wifiSSIDs[i].duplicate == true) 
            continue;
            
          cssid = wifiSSIDs[i].SSID;
          
          for (int j = i + 1; j < n; j++) 
          {
            if (cssid == wifiSSIDs[j].SSID) 
            {
              LOGDEBUG("scan: DUP AP: " +wifiSSIDs[j].SSID);
              // set dup aps to NULL
              wifiSSIDs[j].duplicate = true; 
            }
          }
        }
        }
      }
    }
  }
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::startConfigPortalModeless(char const *apName, char const *apPassword, bool shouldConnectWiFi) 
{
  _modeless     = true;
  _apName       = apName;
  _apPassword   = apPassword;

  WiFi.mode(WIFI_AP_STA);
  
  LOGDEBUG("SET AP STA");

  // try to connect
  if (shouldConnectWiFi && connectWifi("", "") == WL_CONNECTED)   
  {
    LOGDEBUG1(F("IP Address:"), WiFi.localIP());
       
 	  if ( _savecallback != NULL) 
	  {
	    //todo: check if any custom parameters actually exist, and check if they really changed maybe
	    _savecallback();
	  }
  }

  if ( _apcallback != NULL) 
  {
    _apcallback(this);
  }

  connect = false;
  setupConfigPortal();
  scannow = -1 ;
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::loop()
{
	safeLoop();
	criticalLoop();
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::setInfo() 
{
  if (needInfo) 
  {
    pager       = infoAsString();
    wifiStatus  = WiFi.status();
    needInfo    = false;
  }
}

//////////////////////////////////////////

// Anything that accesses WiFi, ESP or EEPROM goes here

void ESPAsync_WiFiManager::criticalLoop()
{
  LOGDEBUG(F("criticalLoop: Enter"));
  
  if (_modeless)
  {
    if (scannow == -1 || millis() > scannow + TIME_BETWEEN_MODELESS_SCANS)
    {
      LOGDEBUG(F("criticalLoop: modeless scan"));
      
      scan();
      scannow = millis();
    }
    
    if (connect) 
    {
      connect = false;

      LOGDEBUG(F("criticalLoop: Connecting to new AP"));

      // using user-provided  _ssid, _pass in place of system-stored ssid and pass
      if (connectWifi(_ssid, _pass) != WL_CONNECTED) 
      {
        LOGDEBUG(F("criticalLoop: Failed to connect."));
      } 
      else 
      {
        //connected
        // alanswx - should we have a config to decide if we should shut down AP?
        // WiFi.mode(WIFI_STA);
        //notify that configuration has changed and any optional parameters should be saved
        if ( _savecallback != NULL) 
        {
          //todo: check if any custom parameters actually exist, and check if they really changed maybe
          _savecallback();
        }

        return;
      }

      if (_shouldBreakAfterConfig) 
      {
        //flag set to exit after config after trying to connect
        //notify that configuration has changed and any optional parameters should be saved
        if ( _savecallback != NULL) 
        {
          //todo: check if any custom parameters actually exist, and check if they really changed maybe
          _savecallback();
        }
      }
    }
  }
}

//////////////////////////////////////////

// Anything that doesn't access WiFi, ESP or EEPROM can go here

void ESPAsync_WiFiManager::safeLoop()
{
  #ifndef USE_EADNS	
  dnsServer->processNextRequest();
  #endif
}

///////////////////////////////////////////////////////////

bool  ESPAsync_WiFiManager::startConfigPortal()
{
#ifdef ESP8266
  String ssid = "ESP_" + String(ESP.getChipId());
#else		//ESP32
  String ssid = "ESP_" + String((uint32_t)ESP.getEfuseMac());
#endif
  ssid.toUpperCase();

  return startConfigPortal(ssid.c_str(), NULL);
}

//////////////////////////////////////////

bool  ESPAsync_WiFiManager::startConfigPortal(char const *apName, char const *apPassword)
{
  WiFi.mode(WIFI_AP_STA);

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

  LOGINFO("ESPAsync_WiFiManager::startConfigPortal : Enter loop");
  
  scannow = -1 ;

  while (_configPortalTimeout == 0 || millis() < _configPortalStart + _configPortalTimeout)
  {
#if ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 )    
    // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
    delay(1);
#else
    //DNS
    if (dnsServer)
      dnsServer->processNextRequest();    
    
    //
    //  we should do a scan every so often here and
    //  try to reconnect to AP while we are at it
    //
    if ( scannow == -1 || millis() > scannow + TIME_BETWEEN_MODAL_SCANS)
    {
      LOGDEBUG(F("startConfigPortal: About to modal scan()"));
      
      // since we are modal, we can scan every time
      shouldscan = true;
      
#if defined(ESP8266)
      // we might still be connecting, so that has to stop for scanning
      ETS_UART_INTR_DISABLE ();
      wifi_station_disconnect ();
      ETS_UART_INTR_ENABLE ();
#else
      WiFi.disconnect (false);
#endif

      scan();
      
      //if (_tryConnectDuringConfigPortal) 
      //  WiFi.begin(); // try to reconnect to AP
        
      scannow = millis() ;
    }
#endif    // ARDUINO_ESP32S2_DEV

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
      LOGERROR("Stop ConfigPortal");
     
      stopConfigPortal = false;
      break;
    }
    
    yield();
    
#if ( defined(TIME_BETWEEN_CONFIG_PORTAL_LOOP) && (TIME_BETWEEN_CONFIG_PORTAL_LOOP > 0) )
    #warning Using delay in startConfigPortal loop
    delay(TIME_BETWEEN_CONFIG_PORTAL_LOOP);
#endif    
  }

  WiFi.mode(WIFI_STA);
  if (TimedOut)
  {
    setHostname();

    // New v1.0.8 to fix static IP when CP not entered or timed-out
    setWifiStaticIP();
    
    WiFi.begin();
    int connRes = waitForConnectResult();

    LOGERROR1("Timed out connection result:", getStatus(connRes));
  }

#if !( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 ) 
  server->reset();
  dnsServer->stop();
#endif

  return  WiFi.status() == WL_CONNECTED;
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::setWifiStaticIP()
{ 
#if USE_CONFIGURABLE_DNS
  if (_WiFi_STA_IPconfig._sta_static_ip)
  {
    LOGWARN(F("Custom STA IP/GW/Subnet"));
   
    //***** Added section for DNS config option *****
    if (_WiFi_STA_IPconfig._sta_static_dns1 && _WiFi_STA_IPconfig._sta_static_dns2) 
    { 
      LOGWARN(F("DNS1 and DNS2 set"));
 
      WiFi.config(_WiFi_STA_IPconfig._sta_static_ip, _WiFi_STA_IPconfig._sta_static_gw, _WiFi_STA_IPconfig._sta_static_sn, _WiFi_STA_IPconfig._sta_static_dns1, _WiFi_STA_IPconfig._sta_static_dns2);
    }
    else if (_WiFi_STA_IPconfig._sta_static_dns1) 
    {
      LOGWARN(F("Only DNS1 set"));
     
      WiFi.config(_WiFi_STA_IPconfig._sta_static_ip, _WiFi_STA_IPconfig._sta_static_gw, _WiFi_STA_IPconfig._sta_static_sn, _WiFi_STA_IPconfig._sta_static_dns1);
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

// New from v1.1.1
int ESPAsync_WiFiManager::reconnectWifi()
{
  int connectResult;
  
  // using user-provided  _ssid, _pass in place of system-stored ssid and pass
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

int ESPAsync_WiFiManager::connectWifi(String ssid, String pass)
{
  // Add option if didn't input/update SSID/PW => Use the previous saved Credentials.
  // But update the Static/DHCP options if changed.
  if ( (ssid != "") || ( (ssid == "") && (WiFi_SSID() != "") ) )
  {  
    //fix for auto connect racing issue. Move up from v1.1.0 to avoid resetSettings()
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

wl_status_t ESPAsync_WiFiManager::waitForConnectResult()
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

    // Fix bug from v1.1.0+, connRes is sometimes not correct.
    //return connRes;
    return WiFi.status();
  }
  else
  {
    LOGERROR(F("Waiting WiFi connection with time out"));
    unsigned long start = millis();
    bool keepConnecting = true;
    
    wl_status_t status;

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

void ESPAsync_WiFiManager::startWPS()
{
#ifdef ESP8266
  LOGINFO("START WPS");
  WiFi.beginWPSConfig();
  LOGINFO("END WPS");
#else		//ESP32
  // TODO
  LOGINFO("ESP32 WPS TODO");
#endif
}

//////////////////////////////////////////

//Convenient for debugging but wasteful of program space.
//Remove if short of space
const char* ESPAsync_WiFiManager::getStatus(int status)
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

String ESPAsync_WiFiManager::getConfigPortalSSID()
{
  return _apName;
}

//////////////////////////////////////////

String ESPAsync_WiFiManager::getConfigPortalPW()
{
  return _apPassword;
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::resetSettings()
{
  LOGINFO(F("Previous settings invalidated"));
  
#ifdef ESP8266  
  WiFi.disconnect(true);
#else
  WiFi.disconnect(true, true);
  
  // Temporary fix for issue of not clearing WiFi SSID/PW from flash of ESP32
  // See https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/25 and https://github.com/espressif/arduino-esp32/issues/400
  WiFi.begin("0","0");
  //////
#endif

  delay(200);
  return;
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::setTimeout(unsigned long seconds)
{
  setConfigPortalTimeout(seconds);
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::setConfigPortalTimeout(unsigned long seconds)
{
  _configPortalTimeout = seconds * 1000;
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::setConnectTimeout(unsigned long seconds)
{
  _connectTimeout = seconds * 1000;
}

void ESPAsync_WiFiManager::setDebugOutput(bool debug)
{
  _debug = debug;
}

//////////////////////////////////////////

// KH, To enable dynamic/random channel
int ESPAsync_WiFiManager::setConfigPortalChannel(int channel)
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

void ESPAsync_WiFiManager::setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn)
{
  LOGINFO(F("setAPStaticIPConfig"));
  _WiFi_AP_IPconfig._ap_static_ip = ip;
  _WiFi_AP_IPconfig._ap_static_gw = gw;
  _WiFi_AP_IPconfig._ap_static_sn = sn;
}

//////////////////////////////////////////

// KH, new using struct
void ESPAsync_WiFiManager::setAPStaticIPConfig(WiFi_AP_IPConfig  WM_AP_IPconfig)
{
  LOGINFO(F("setAPStaticIPConfig"));
  
  memcpy(&_WiFi_AP_IPconfig, &WM_AP_IPconfig, sizeof(_WiFi_AP_IPconfig));
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::getAPStaticIPConfig(WiFi_AP_IPConfig  &WM_AP_IPconfig)
{
  LOGINFO(F("getAPStaticIPConfig"));
  
  memcpy(&WM_AP_IPconfig, &_WiFi_AP_IPconfig, sizeof(WM_AP_IPconfig));
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn)
{
  LOGINFO(F("setSTAStaticIPConfig"));
  _WiFi_STA_IPconfig._sta_static_ip = ip;
  _WiFi_STA_IPconfig._sta_static_gw = gw;
  _WiFi_STA_IPconfig._sta_static_sn = sn;
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::setSTAStaticIPConfig(WiFi_STA_IPConfig WM_STA_IPconfig)
{
  LOGINFO(F("setSTAStaticIPConfig"));
  
  memcpy(&_WiFi_STA_IPconfig, &WM_STA_IPconfig, sizeof(_WiFi_STA_IPconfig));
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::getSTAStaticIPConfig(WiFi_STA_IPConfig &WM_STA_IPconfig)
{
  LOGINFO(F("getSTAStaticIPConfig"));
  
  memcpy(&WM_STA_IPconfig, &_WiFi_STA_IPconfig, sizeof(WM_STA_IPconfig));
}


//////////////////////////////////////////

#if USE_CONFIGURABLE_DNS
void ESPAsync_WiFiManager::setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn, IPAddress dns_address_1, IPAddress dns_address_2)
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

void ESPAsync_WiFiManager::setMinimumSignalQuality(int quality)
{
  _minimumQuality = quality;
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::setBreakAfterConfig(bool shouldBreak)
{
  _shouldBreakAfterConfig = shouldBreak;
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::reportStatus(String &page)
{
  page += FPSTR(WM_HTTP_SCRIPT_NTP_MSG);

  if (WiFi_SSID() != "")
  {
    page += F("Configured to connect to AP <b>");
    page += WiFi_SSID();

    if (WiFi.status() == WL_CONNECTED)
    {
      page += F(" and connected</b> on IP <a href=\"http://");
      page += WiFi.localIP().toString();
      page += F("/\">");
      page += WiFi.localIP().toString();
      page += F("</a>");
    }
    else
    {
      page += F(" but not connected.</b>");
    }
  }
  else
  {
    page += F("No network configured.");
  }
}

//////////////////////////////////////////

// Handle root or redirect to captive portal
void ESPAsync_WiFiManager::handleRoot(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("Handle root"));

  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;
  
  //wifiSSIDscan  = true;
  //scan();

  if (captivePortal(request))
  {
    // If captive portal redirect instead of displaying the error page.
    return;
  }

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
  
  page += FPSTR(WM_FLDSET_START);
  
  page += FPSTR(WM_HTTP_PORTAL_OPTIONS);
  page += F("<div class=\"msg\">");
  reportStatus(page);
  page += F("</div>");
  
  page += FPSTR(WM_FLDSET_END);
    
  page += FPSTR(WM_HTTP_END);
 
#if ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 ) 
  request->send(200, WM_HTTP_HEAD_CT, page);
  
  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else  
 
  AsyncWebServerResponse *response = request->beginResponse(200, WM_HTTP_HEAD_CT, page);
  response->addHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));
  
#if USING_CORS_FEATURE
  // New from v1.1.0, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  response->addHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif
  
  response->addHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(WM_HTTP_EXPIRES), "-1");
  
  request->send(response);
  
#endif    // ARDUINO_ESP32S2_DEV
}


//////////////////////////////////////////

// Wifi config page handler
void ESPAsync_WiFiManager::handleWifi(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("Handle WiFi"));

  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;
   
  String page = FPSTR(WM_HTTP_HEAD_START);
  page.replace("{v}", "Config ESP");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_SCRIPT_NTP);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += F("<h2>Configuration</h2>");

#if !( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 )

  wifiSSIDscan = false;
  LOGDEBUG(F("handleWifi: Scan done"));

  if (wifiSSIDCount == 0) 
  {
    LOGDEBUG(F("handleWifi: No networks found"));
    page += F("No networks found. Refresh to scan again.");
  } 
  else 
  {
    page += FPSTR(WM_FLDSET_START);
    
    //display networks in page
    String pager = networkListAsString();
    
    page += pager;
    
    page += FPSTR(WM_FLDSET_END);
   
    page += "<br/>";
  }
  
  wifiSSIDscan = true;
  
  page += "<small>To reuse already connected AP, leave SSID & password fields empty</small>";

#endif    // ARDUINO_ESP32S2_DEV
  
  page += FPSTR(WM_HTTP_FORM_START);
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

  page += FPSTR(WM_HTTP_FORM_END);

  page += FPSTR(WM_HTTP_END);
  
#if ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 ) 
  request->send(200, WM_HTTP_HEAD_CT, page);
  
  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else  
 
  AsyncWebServerResponse *response = request->beginResponse(200, WM_HTTP_HEAD_CT, page);
  response->addHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));
  
#if USING_CORS_FEATURE
  // New from v1.1.0, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  response->addHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif
  
  response->addHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(WM_HTTP_EXPIRES), "-1");
  
  request->send(response);
  
#endif    // ARDUINO_ESP32S2_DEV  

  LOGDEBUG(F("Sent config page"));
}

//////////////////////////////////////////

// Handle the WLAN save form and redirect to WLAN config page again
void ESPAsync_WiFiManager::handleWifiSave(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("WiFi save"));

  //SAVE/connect here
  _ssid = request->arg("s").c_str();
  _pass = request->arg("p").c_str();
  
  // New from v1.1.0
  _ssid1 = request->arg("s1").c_str();
  _pass1 = request->arg("p1").c_str();
  //////

  //parameters
  for (int i = 0; i < _paramsCount; i++)
  {
    if (_params[i] == NULL)
    {
      break;
    }

    //read parameter
    String value = request->arg(_params[i]->getID()).c_str();
    
    //store it in array
    value.toCharArray(_params[i]->_WMParam_data._value, _params[i]->_WMParam_data._length);
    
    LOGDEBUG2(F("Parameter and value :"), _params[i]->getID(), value);
  }

  if (request->hasArg("ip"))
  {
    String ip = request->arg("ip");
    optionalIPFromString(&_WiFi_STA_IPconfig._sta_static_ip, ip.c_str());
    
    LOGDEBUG1(F("New Static IP ="), _WiFi_STA_IPconfig._sta_static_ip.toString());
  }

  if (request->hasArg("gw"))
  {
    String gw = request->arg("gw");
    optionalIPFromString(&_WiFi_STA_IPconfig._sta_static_gw, gw.c_str());
    
    LOGDEBUG1(F("New Static Gateway ="), _WiFi_STA_IPconfig._sta_static_gw.toString());
  }

  if (request->hasArg("sn"))
  {
    String sn = request->arg("sn");
    optionalIPFromString(&_WiFi_STA_IPconfig._sta_static_sn, sn.c_str());
    
    LOGDEBUG1(F("New Static Netmask ="), _WiFi_STA_IPconfig._sta_static_sn.toString());
  }

#if USE_CONFIGURABLE_DNS
  //*****  Added for DNS Options *****
  if (request->hasArg("dns1"))
  {
    String dns1 = request->arg("dns1");
    optionalIPFromString(&_WiFi_STA_IPconfig._sta_static_dns1, dns1.c_str());
    
    LOGDEBUG1(F("New Static DNS1 ="), _WiFi_STA_IPconfig._sta_static_dns1.toString());
  }

  if (request->hasArg("dns2"))
  {
    String dns2 = request->arg("dns2");
    optionalIPFromString(&_WiFi_STA_IPconfig._sta_static_dns2, dns2.c_str());
    
    LOGDEBUG1(F("New Static DNS2 ="), _WiFi_STA_IPconfig._sta_static_dns2.toString());
  }
  //*****  End added for DNS Options *****
#endif

  String page = FPSTR(WM_HTTP_HEAD_START);
  page.replace("{v}", "Credentials Saved");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_SCRIPT_NTP);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += FPSTR(WM_HTTP_SAVED);
  page.replace("{v}", _apName);
  page.replace("{x}", _ssid);
  
  // KH, update from v1.1.0
  page.replace("{x1}", _ssid1);
  //////
  
  page += FPSTR(WM_HTTP_END);
 
#if ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 ) 
  request->send(200, WM_HTTP_HEAD_CT, page);
  
  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else    
 
  AsyncWebServerResponse *response = request->beginResponse(200, WM_HTTP_HEAD_CT, page);
  response->addHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));
  
#if USING_CORS_FEATURE
  // New from v1.1.0, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  response->addHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif
  
  response->addHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(WM_HTTP_EXPIRES), "-1");
  request->send(response);
  
#endif    // ARDUINO_ESP32S2_DEV

  LOGDEBUG(F("Sent wifi save page"));

  connect = true; //signal ready to connect/reset

  // Restore when Press Save WiFi
  _configPortalTimeout = DEFAULT_PORTAL_TIMEOUT;
}

//////////////////////////////////////////

// Handle shut down the server page
void ESPAsync_WiFiManager::handleServerClose(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("Server Close"));
   
  String page = FPSTR(WM_HTTP_HEAD_START);
  page.replace("{v}", "Close Server");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_SCRIPT_NTP);
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
   
#if ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 ) 
  request->send(200, WM_HTTP_HEAD_CT, page);
  
  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else    
 
  AsyncWebServerResponse *response = request->beginResponse(200, WM_HTTP_HEAD_CT, page);
  response->addHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));
  
#if USING_CORS_FEATURE
  // New from v1.1.0, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  response->addHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif
  
  response->addHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(WM_HTTP_EXPIRES), "-1");
  request->send(response);
  
#endif    // ARDUINO_ESP32S2_DEV
  
  stopConfigPortal = true; //signal ready to shutdown config portal
  
  LOGDEBUG(F("Sent server close page"));

  // Restore when Press Save WiFi
  _configPortalTimeout = DEFAULT_PORTAL_TIMEOUT;
}

//////////////////////////////////////////

// Handle the info page
void ESPAsync_WiFiManager::handleInfo(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("Info"));

  // Disable _configPortalTimeout when someone accessing Portal to give some time to config
  _configPortalTimeout = 0;
 
  String page = FPSTR(WM_HTTP_HEAD_START);
  page.replace("{v}", "Info");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_SCRIPT_NTP);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  
  if (connect)
    page += F("<meta http-equiv=\"refresh\" content=\"5; url=/i\">");
  
  page += FPSTR(WM_HTTP_HEAD_END);
  
  page += F("<dl>");
  
  if (connect)
  {
    page += F("<dt>Trying to connect</dt><dd>");
    page += wifiStatus;
    page += F("</dd>");
  }

  page +=pager;
  
  page += F("<h2>WiFi Information</h2>");
  reportStatus(page);
  
  page += FPSTR(WM_FLDSET_START);
  
  page += F("<h3>Device Data</h3>");
  
  page += F("<table class=\"table\">");
  page += F("<thead><tr><th>Name</th><th>Value</th></tr></thead><tbody><tr><td>Chip ID</td><td>");

#ifdef ESP8266
  page += String(ESP.getChipId(), HEX);		//ESP.getChipId();
#else		//ESP32
  page += String((uint32_t)ESP.getEfuseMac(), HEX);		//ESP.getChipId();
#endif

  page += F("</td></tr>");
  page += F("<tr><td>Flash Chip ID</td><td>");

#ifdef ESP8266
  page += String(ESP.getFlashChipId(), HEX);		//ESP.getFlashChipId();
#else		//ESP32
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
#else		//ESP32
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

  page += F("<p/>More information about ESPAsync_WiFiManager at");
  page += F("<p/><a href=\"https://github.com/khoih-prog/ESPAsync_WiFiManager\">https://github.com/khoih-prog/ESPAsync_WiFiManager</a>");
  page += FPSTR(WM_HTTP_END);
 
#if ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 ) 
  request->send(200, WM_HTTP_HEAD_CT, page);
  
  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else    
 
  AsyncWebServerResponse *response = request->beginResponse(200, WM_HTTP_HEAD_CT, page);
  response->addHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));
  
#if USING_CORS_FEATURE
  // New from v1.1.0, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  response->addHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif
  
  response->addHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(WM_HTTP_EXPIRES), "-1");
  request->send(response);
  
#endif    // ARDUINO_ESP32S2_DEV

  LOGDEBUG(F("Info page sent"));
}

//////////////////////////////////////////

// Handle the state page
void ESPAsync_WiFiManager::handleState(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("State - json"));
   
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
   
#if ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 ) 
  request->send(200, WM_HTTP_HEAD_CT, page);
  
  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else  
   
  AsyncWebServerResponse *response = request->beginResponse(200, "application/json", page);
  response->addHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE));
  
#if USING_CORS_FEATURE
  // New from v1.1.0, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
  response->addHeader(FPSTR(WM_HTTP_CORS), _CORS_Header);
#endif
  
  response->addHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(WM_HTTP_EXPIRES), "-1");
  
  request->send(response);
#endif    // ARDUINO_ESP32S2_DEV
  
  LOGDEBUG(F("Sent state page in json format"));
}

//////////////////////////////////////////

// Handle the reset page
void ESPAsync_WiFiManager::handleReset(AsyncWebServerRequest *request)
{
  LOGDEBUG(F("Reset"));
    
  String page = FPSTR(WM_HTTP_HEAD_START);
  page.replace("{v}", "WiFi Information");
  page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_SCRIPT_NTP);
  page += FPSTR(WM_HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(WM_HTTP_HEAD_END);
  page += F("Resetting");
  page += FPSTR(WM_HTTP_END);
    
#if ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 ) 
  request->send(200, WM_HTTP_HEAD_CT, page);
#else  
    
  AsyncWebServerResponse *response = request->beginResponse(200, WM_HTTP_HEAD_CT, page);
  response->addHeader(WM_HTTP_CACHE_CONTROL, WM_HTTP_NO_STORE);
  response->addHeader(WM_HTTP_PRAGMA, WM_HTTP_NO_CACHE);
  response->addHeader(WM_HTTP_EXPIRES, "-1");
  
  request->send(response);
#endif    // ARDUINO_ESP32S2_DEV

  LOGDEBUG(F("Sent reset page"));
  delay(5000);
  
  // Temporary fix for issue of not clearing WiFi SSID/PW from flash of ESP32
  // See https://github.com/khoih-prog/ESP_WiFiManager/issues/25 and https://github.com/espressif/arduino-esp32/issues/400
  resetSettings();
  //WiFi.disconnect(true); // Wipe out WiFi credentials.
  //////

#ifdef ESP8266
  ESP.reset();
#else		//ESP32
  ESP.restart();
#endif

  delay(2000);
}

//////////////////////////////////////////

void ESPAsync_WiFiManager::handleNotFound(AsyncWebServerRequest *request)
{
  if (captivePortal(request))
  {
    // If captive portal redirect instead of displaying the error page.
    return;
  }

  String message = "File Not Found\n\n";
  
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";

  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }

#if ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 ) 
  request->send(200, WM_HTTP_HEAD_CT, message);
  
  // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
  delay(1);
#else  

  AsyncWebServerResponse *response = request->beginResponse( 404, WM_HTTP_HEAD_CT2, message );
  response->addHeader(FPSTR(WM_HTTP_CACHE_CONTROL), FPSTR(WM_HTTP_NO_STORE)); 
  response->addHeader(FPSTR(WM_HTTP_PRAGMA), FPSTR(WM_HTTP_NO_CACHE));
  response->addHeader(FPSTR(WM_HTTP_EXPIRES), "-1");
  
  request->send(response);
#endif    // ARDUINO_ESP32S2_DEV
}

//////////////////////////////////////////

/**
   HTTPD redirector
   Redirect to captive portal if we got a request for another domain.
   Return true in that case so the page handler do not try to handle the request again.
*/
bool ESPAsync_WiFiManager::captivePortal(AsyncWebServerRequest *request)
{
  if (!isIp(request->host()))
  {
    LOGDEBUG(F("Request redirected to captive portal"));
    LOGDEBUG1(F("Location http://"), toStringIp(request->client()->localIP()));
    
    AsyncWebServerResponse *response = request->beginResponse(302, WM_HTTP_HEAD_CT2, "");
    response->addHeader("Location", String("http://") + toStringIp(request->client()->localIP()));
    request->send(response);
       
    return true;
  }
  
  LOGDEBUG1(F("request host IP ="), request->host());
  
  return false;
}

//////////////////////////////////////////

// start up config portal callback
void ESPAsync_WiFiManager::setAPCallback(void(*func)(ESPAsync_WiFiManager* myWiFiManager))
{
  _apcallback = func;
}

//////////////////////////////////////////

// start up save config callback
void ESPAsync_WiFiManager::setSaveConfigCallback(void(*func)())
{
  _savecallback = func;
}

//////////////////////////////////////////

// sets a custom element to add to head, like a new style tag
void ESPAsync_WiFiManager::setCustomHeadElement(const char* element) {
  _customHeadElement = element;
}

//////////////////////////////////////////

// if this is true, remove duplicated Access Points - defaut true
void ESPAsync_WiFiManager::setRemoveDuplicateAPs(bool removeDuplicates)
{
  _removeDuplicateAPs = removeDuplicates;
}

//////////////////////////////////////////

// Scan for WiFiNetworks in range and sort by signal strength
// space for indices array allocated on the heap and should be freed when no longer required
int ESPAsync_WiFiManager::scanWifiNetworks(int **indicesptr)
{
  LOGDEBUG(F("Scanning Network"));
  
  int n = WiFi.scanNetworks();

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
            
            // set dup aps to index -1
            indices[j] = -1;
          }
        }
      }
    }

    for (int i = 0; i < n; i++)
    {
      if (indices[i] == -1)
      {
        // skip dups
        continue;
      }

      int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));

      if (!(_minimumQuality == -1 || _minimumQuality < quality))
      {
        indices[i] = -1;
        LOGDEBUG(F("Skipping low quality"));
      }
    }

#if (_ESPASYNC_WIFIMGR_LOGLEVEL_ > 2)
    for (int i = 0; i < n; i++)
    {
      if (indices[i] == -1)
      { 
        // skip dups
        continue;
      }
      else
        LOGINFO(WiFi.SSID(indices[i]));
    }
#endif

    return (n);
  }
}

//////////////////////////////////////////

int ESPAsync_WiFiManager::getRSSIasQuality(int RSSI)
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

// Is this an IP?
bool ESPAsync_WiFiManager::isIp(String str)
{
  for (unsigned int i = 0; i < str.length(); i++)
  {
    int c = str.charAt(i);

    if (c != '.' && (c < '0' || c > '9'))
    {
      return false;
    }
  }
  return true;
}

//////////////////////////////////////////

// IP to String
String ESPAsync_WiFiManager::toStringIp(IPAddress ip)
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

String ESPAsync_WiFiManager::getStoredWiFiSSID()
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

String ESPAsync_WiFiManager::getStoredWiFiPass()
{
  if (WiFi.getMode() == WIFI_MODE_NULL)
  {
    return String();
  }

  wifi_config_t conf;
  esp_wifi_get_config(WIFI_IF_STA, &conf);
  
  return String(reinterpret_cast<char*>(conf.sta.password));
}
#endif

//////////////////////////////////////////

