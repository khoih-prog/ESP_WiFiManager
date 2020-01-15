/****************************************************************************************************************************
 * ESP_WiFiManager.h
 * For ESP8266 / ESP32 boards
 *
 * ESP_WiFiManager is a library for the ESP8266/Arduino platform
 * (https://github.com/esp8266/Arduino) to enable easy
 * configuration and reconfiguration of WiFi credentials using a Captive Portal
 * inspired by:
 * http://www.esp8266.com/viewtopic.php?f=29&t=2520
 * https://github.com/chriscook8/esp-arduino-apboot
 * https://github.com/esp8266/Arduino/blob/master/libraries/DNSServer/examples/CaptivePortalAdvanced/
 *
 * Forked from Tzapu https://github.com/tzapu/WiFiManager
 * and from Ken Taylor https://github.com/kentaylor
 * 
 * Built by Khoi Hoang https://github.com/khoih-prog/ESP_WiFiManager
 * Licensed under MIT license
 * Version: 1.0.3
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.0   K Hoang      07/10/2019 Initial coding
 *  1.0.1   K Hoang      13/12/2019 Fix bug. Add features. Add support for ESP32
 *  1.0.2   K Hoang      19/12/2019 Fix bug thatkeeps ConfigPortal in endless loop if Portal/Router SSID or Password is NULL.
 *  1.0.3   K Hoang      05/01/2020 Option not displaying AvailablePages in Info page. Enhance README.md. Modify examples
 *  1.0.4   K Hoang	     07/01/2020 Add RFC952 setHostname feature.
 *  1.0.5   K Hoang	     15/01/2020 Add configurable DNS feature. Thanks to @Amorphous of https://community.blynk.cc
 *****************************************************************************************************************************/

#ifndef ESP_WiFiManager_h
#define ESP_WiFiManager_h

//KH, for ESP32
#ifdef ESP8266
	#include <ESP8266WiFi.h>
	#include <ESP8266WebServer.h>
#else		//ESP32
	#include <WiFi.h>
	#include <WebServer.h>
#endif

#include <DNSServer.h>
#include <memory>
#undef min
#undef max
#include <algorithm>

//KH, for ESP32
#ifdef ESP8266
	extern "C" 
	{
		#include "user_interface.h"
	}
	#define ESP_getChipId()   (ESP.getChipId())
#else		//ESP32
	#include <esp_wifi.h>
	#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())
#endif


#define WFM_LABEL_BEFORE 1
#define WFM_LABEL_AFTER 2
#define WFM_NO_LABEL 0

//KH
//Mofidy HTTP_HEAD to HTTP_HEAD_START to avoid conflict in Arduino esp8266 core 2.6.0+
const char HTTP_200[] PROGMEM             = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
const char HTTP_HEAD_START[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>body,textarea,input,select{background: 0;border-radius: 0;font: 16px sans-serif;margin: 0}textarea,input,select{outline: 0;font-size: 14px;border: 1px solid #ccc;padding: 8px;width: 90%}.btn a{text-decoration: none}.container{margin: auto;width: 90%}@media(min-width:1200px){.container{margin: auto;width: 30%}}@media(min-width:768px) and (max-width:1200px){.container{margin: auto;width: 50%}}.btn,h2{font-size: 2em}h1{font-size: 3em}.btn{background: #0ae;border-radius: 4px;border: 0;color: #fff;cursor: pointer;display: inline-block;margin: 2px 0;padding: 10px 14px 11px;width: 100%}.btn:hover{background: #09d}.btn:active,.btn:focus{background: #08b}label>*{display: inline}form>*{display: block;margin-bottom: 10px}textarea:focus,input:focus,select:focus{border-color: #5ab}.msg{background: #def;border-left: 5px solid #59d;padding: 1.5em}.q{float: right;width: 64px;text-align: right}.l{background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==') no-repeat left center;background-size: 1em}input[type='checkbox']{float: left;width: 20px}.table td{padding:.5em;text-align:left}.table tbody>:nth-child(2n-1){background:#ddd}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEAD_END[] PROGMEM        = "</head><body><div class=\"container\">";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button class=\"btn\">Configuration</button></form><br/><form action=\"/i\" method=\"get\"><button class=\"btn\">Information</button></form><br/><form action=\"/close\" method=\"get\"><button class=\"btn\">Exit Portal</button></form><br/>";
const char HTTP_ITEM[] PROGMEM            = "<div><a href=\"#p\" onclick=\"c(this)\">{v}</a>&nbsp;<span class=\"q {i}\">{r}%</span></div>";
const char JSON_ITEM[] PROGMEM            = "{\"SSID\":\"{v}\", \"Encryption\":{i}, \"Quality\":\"{r}\"}";
const char HTTP_FORM_START[] PROGMEM      = "<form method=\"get\" action=\"wifisave\"><label>SSID</label><input id=\"s\" name=\"s\" length=32 placeholder=\"SSID\"><label>Password</label><input id=\"p\" name=\"p\" length=64 placeholder=\"password\">";
const char HTTP_FORM_LABEL[] PROGMEM      = "<label for=\"{i}\">{p}</label>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<input id=\"{i}\" name=\"{n}\" length={l} placeholder=\"{p}\" value=\"{v}\" {c}>";
const char HTTP_FORM_END[] PROGMEM        = "<button class=\"btn\" type=\"submit\">Save</button></form>";
const char HTTP_SAVED[] PROGMEM           = "<div class=\"msg\"><strong>Credentials Saved</strong><br>Trying to connect ESP to the {x} network.<br>Give it 10 seconds or so and check <a href=\"/\">how it went.</a> <p/>The {v} network you are connected to will be restarted on the radio channel of the {x} network. You may have to manually reconnect to the {v} network.</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";

//KH, added 2019/12/15 from Tzapu Development
// http
const char HTTP_HEAD_CL[]         PROGMEM = "Content-Length";
const char HTTP_HEAD_CT[]         PROGMEM = "text/html";
const char HTTP_HEAD_CT2[]        PROGMEM = "text/plain";
const char HTTP_HEAD_CORS[]       PROGMEM = "Access-Control-Allow-Origin";
const char HTTP_HEAD_CORS_ALLOW_ALL[]  PROGMEM = "*";

#if USE_AVAILABLE_PAGES
const char HTTP_AVAILABLE_PAGES[] PROGMEM = "<h3>Available Pages</h3><table class=\"table\"><thead><tr><th>Page</th><th>Function</th></tr></thead><tbody><tr><td><a href=\"/\">/</a></td><td>Menu page.</td></tr><tr><td><a href=\"/wifi\">/wifi</a></td><td>Show WiFi scan results and enter WiFi configuration.</td></tr><tr><td><a href=\"/wifisave\">/wifisave</a></td><td>Save WiFi configuration information and configure device. Needs variables supplied.</td></tr><tr><td><a href=\"/close\">/close</a></td><td>Close the configuration server and configuration WiFi network.</td></tr><tr><td><a href=\"/i\">/i</a></td><td>This page.</td></tr><tr><td><a href=\"/r\">/r</a></td><td>Delete WiFi configuration and reboot. ESP device will not reconnect to a network until new WiFi configuration data is entered.</td></tr><tr><td><a href=\"/state\">/state</a></td><td>Current device state in JSON format. Interface for programmatic WiFi configuration.</td></tr><tr><td><a href=\"/scan\">/scan</a></td><td>Run a WiFi scan and return results in JSON format. Interface for programmatic WiFi configuration.</td></tr></table>";
#else
const char HTTP_AVAILABLE_PAGES[] PROGMEM = "";
#endif

//KH
#define WIFI_MANAGER_MAX_PARAMS 20

// Thanks to @Amorphous for the feature and code, from v1.0.5
// (https://community.blynk.cc/t/esp-wifimanager-for-esp32-and-esp8266/42257/13)
#define USE_CONFIGURABLE_DNS      true

class ESP_WMParameter {
  public:
    ESP_WMParameter(const char *custom);
    ESP_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    ESP_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    ESP_WMParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom, int labelPlacement);
    
    ~ESP_WMParameter();

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    int         getLabelPlacement();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
	int         _labelPlacement;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom, int labelPlacement);

    friend class ESP_WiFiManager;
};

#define USE_DYNAMIC_PARAMS				true
#define DEFAULT_PORTAL_TIMEOUT  	60000L
   
class ESP_WiFiManager
{
  public:

		ESP_WiFiManager(const char *iHostname = "");

    ~ESP_WiFiManager();

    boolean       autoConnect(); //Deprecated. Do not use.
    boolean       autoConnect(char const *apName, char const *apPassword = NULL); //Deprecated. Do not use.

    //if you want to start the config portal
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //usefully for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, usefull if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    
#if USE_CONFIGURABLE_DNS    
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn, 
                                        IPAddress dns_address_1, IPAddress dns_address_2);
#endif   
 
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(ESP_WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    
    #if USE_DYNAMIC_PARAMS
    	//adds a custom parameter
			bool 				addParameter(ESP_WMParameter *p);
		#else
			//adds a custom parameter
			void 				addParameter(ESP_WMParameter *p);
		#endif

    //if this is set, it will exit after config, even if connection is unsucessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);
    //Scan for WiFiNetworks in range and sort by signal strength
    //space for indices array allocated on the heap and should be freed when no longer required
    int           scanWifiNetworks(int **indicesptr);
    
    // return SSID of router in STA mode got from config portal. NULL if no user's input //KH
    String				getSSID( void ) { return _ssid; }
    
    // return password of router in STA mode got from config portal. NULL if no user's input //KH
    String				getPW( void )		{ return _pass; }					
    
    //returns the list of Parameters
    ESP_WMParameter** getParameters();
    // returns the Parameters Count
    int           getParametersCount();
    
    const char*   getStatus(int status);
    
    #ifdef ESP32
    String getStoredWiFiSSID();
    String getStoredWiFiPass();
    #endif
    
    String WiFi_SSID(void)
    {
      #ifdef ESP8266
      return WiFi.SSID();
      #else
      return getStoredWiFiSSID();
      #endif
    }

    String WiFi_Pass(void)
    {
      #ifdef ESP8266
      return WiFi.psk();
      #else
      return getStoredWiFiPass();
      #endif
    }

		void setHostname(void)
		{
			if (RFC952_hostname[0] != 0)
			{
				#ifdef ESP8266
					WiFi.hostname(RFC952_hostname);
				#else		//ESP32
					// See https://github.com/espressif/arduino-esp32/issues/2537
					WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
					WiFi.setHostname(RFC952_hostname);
				#endif
			}
		}
    
  private:
    std::unique_ptr<DNSServer>        dnsServer;

//KH, for ESP32    
#ifdef ESP8266
    std::unique_ptr<ESP8266WebServer> server;
#else		//ESP32
    std::unique_ptr<WebServer>        server;
#endif    

		#define RFC952_HOSTNAME_MAXLEN      24
		char RFC952_hostname[RFC952_HOSTNAME_MAXLEN + 1];

		char* getRFC952_hostname(const char* iHostname);

    void          setupConfigPortal();
    void          startWPS();
    //const char*   getStatus(int status);

    const char*   _apName                   = "no-net";
    const char*   _apPassword               = NULL;
    String        _ssid                     = "";
    String        _pass                     = "";
    
    unsigned long _configPortalTimeout      = 0;
    
    unsigned long _connectTimeout           = 0;
    unsigned long _configPortalStart        = 0;
	
	  int numberOfNetworks;
	  int *networkIndices;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;
    
#if USE_CONFIGURABLE_DNS
    IPAddress     _sta_static_dns1;
    IPAddress     _sta_static_dns2;
#endif

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          handleRoot();
    void          handleWifi();
    void          handleWifiSave();
    void          handleServerClose();
    void          handleInfo();
    void          handleState();
    void          handleScan();
    void          handleReset();
    void          handleNotFound();
    boolean       captivePortal();
    void          reportStatus(String &page);

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       stopConfigPortal = false;
    boolean       _debug = false;     //true;
 
    void (*_apcallback)(ESP_WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;
    
    #if USE_DYNAMIC_PARAMS
      int                    _max_params;
    	ESP_WMParameter** _params;
    #else
    	ESP_WMParameter* _params[WIFI_MANAGER_MAX_PARAMS];
		#endif
		
    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};

#endif
