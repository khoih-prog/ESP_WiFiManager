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
   Version: 1.1.2

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
 *****************************************************************************************************************************/

#ifndef ESP_WiFiManager_h
#define ESP_WiFiManager_h

#include "ESP_WiFiManager_Debug.h"

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

/** Handle CORS in pages */
// Default false for using only whenever necessary to avoid security issue when using CORS (Cross-Origin Resource Sharing)
#ifndef USING_CORS_FEATURE
  // Contributed by AlesSt (https://github.com/AlesSt) to solve AJAX CORS protection problem of API redirects on client side
  // See more in https://github.com/khoih-prog/ESP_WiFiManager/issues/27 and https://en.wikipedia.org/wiki/Cross-origin_resource_sharing
  #define USING_CORS_FEATURE     false
#endif

//KH
//Mofidy HTTP_HEAD to WM_HTTP_HEAD_START to avoid conflict in Arduino esp8266 core 2.6.0+
const char WM_HTTP_200[] PROGMEM = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
const char WM_HTTP_HEAD_START[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";

// KH, update from v1.0.10
const char WM_HTTP_STYLE[] PROGMEM = "<style>div{padding:2px;font-size:1em;}body,textarea,input,select{background: 0;border-radius: 0;font: 16px sans-serif;margin: 0}textarea,input,select{outline: 0;font-size: 14px;border: 1px solid #ccc;padding: 8px;width: 90%}.btn a{text-decoration: none}.container{margin: auto;width: 90%}@media(min-width:1200px){.container{margin: auto;width: 30%}}@media(min-width:768px) and (max-width:1200px){.container{margin: auto;width: 50%}}.btn,h2{font-size: 2em}h1{font-size: 3em}.btn{background: #0ae;border-radius: 4px;border: 0;color: #fff;cursor: pointer;display: inline-block;margin: 2px 0;padding: 10px 14px 11px;width: 100%}.btn:hover{background: #09d}.btn:active,.btn:focus{background: #08b}label>*{display: inline}form>*{display: block;margin-bottom: 10px}textarea:focus,input:focus,select:focus{border-color: #5ab}.msg{background: #def;border-left: 5px solid #59d;padding: 1.5em}.q{float: right;width: 64px;text-align: right}.l{background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==') no-repeat left center;background-size: 1em}input[type='checkbox']{float: left;width: 20px}.table td{padding:.5em;text-align:left}.table tbody>:nth-child(2n-1){background:#ddd}fieldset{border-radius:0.5rem;margin:0px;}</style>";
//////

// KH, update from v1.1.0
const char WM_HTTP_SCRIPT[] PROGMEM = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();document.getElementById('s1').value=l.innerText||l.textContent;document.getElementById('p1').focus();}</script>";
//////

// From v1.0.9 to permit disable or configure NTP from sketch
#ifndef USE_ESP_WIFIMANAGER_NTP
  // From v1.0.6 to enable NTP config
  #define USE_ESP_WIFIMANAGER_NTP     true
#endif

#if USE_ESP_WIFIMANAGER_NTP

const char WM_HTTP_SCRIPT_NTP_MSG[] PROGMEM = "<p>Your timezone is : <b><label id='timezone'></b><script>document.getElementById('timezone').innerHTML = timezone.name();</script></p>";

// From v1.0.9 to permit disable or configure NTP from sketch
#ifndef USE_CLOUDFLARE_NTP
  #define USE_CLOUDFLARE_NTP          false
#endif

#if USE_CLOUDFLARE_NTP
const char WM_HTTP_SCRIPT_NTP[] PROGMEM = "<script src='https://cdnjs.cloudflare.com/ajax/libs/jstimezonedetect/1.0.4/jstz.min.js'></script><script>var timezone=jstz.determine();console.log('Your timezone is:' + timezone.name());document.getElementById('timezone').innerHTML = timezone.name();</script>";
#else
const char WM_HTTP_SCRIPT_NTP[] PROGMEM = "<script>(function(e){var t=function(){\"use strict\";var e=\"s\",n=function(e){var t=-e.getTimezoneOffset();return t!==null?t:0},r=function(e,t,n){var r=new Date;return e!==undefined&&r.setFullYear(e),r.setDate(n),r.setMonth(t),r},i=function(e){return n(r(e,0,2))},s=function(e){return n(r(e,5,2))},o=function(e){var t=e.getMonth()>7?s(e.getFullYear()):i(e.getFullYear()),r=n(e);return t-r!==0},u=function(){var t=i(),n=s(),r=i()-s();return r<0?t+\",1\":r>0?n+\",1,\"+e:t+\",0\"},a=function(){var e=u();return new t.TimeZone(t.olson.timezones[e])},f=function(e){var t=new Date(2010,6,15,1,0,0,0),n={\"America/Denver\":new Date(2011,2,13,3,0,0,0),\"America/Mazatlan\":new Date(2011,3,3,3,0,0,0),\"America/Chicago\":new Date(2011,2,13,3,0,0,0),\"America/Mexico_City\":new Date(2011,3,3,3,0,0,0),\"America/Asuncion\":new Date(2012,9,7,3,0,0,0),\"America/Santiago\":new Date(2012,9,3,3,0,0,0),\"America/Campo_Grande\":new Date(2012,9,21,5,0,0,0),\"America/Montevideo\":new Date(2011,9,2,3,0,0,0),\"America/Sao_Paulo\":new Date(2011,9,16,5,0,0,0),\"America/Los_Angeles\":new Date(2011,2,13,8,0,0,0),\"America/Santa_Isabel\":new Date(2011,3,5,8,0,0,0),\"America/Havana\":new Date(2012,2,10,2,0,0,0),\"America/New_York\":new Date(2012,2,10,7,0,0,0),\"Asia/Beirut\":new Date(2011,2,27,1,0,0,0),\"Europe/Helsinki\":new Date(2011,2,27,4,0,0,0),\"Europe/Istanbul\":new Date(2011,2,28,5,0,0,0),\"Asia/Damascus\":new Date(2011,3,1,2,0,0,0),\"Asia/Jerusalem\":new Date(2011,3,1,6,0,0,0),\"Asia/Gaza\":new Date(2009,2,28,0,30,0,0),\"Africa/Cairo\":new Date(2009,3,25,0,30,0,0),\"Pacific/Auckland\":new Date(2011,8,26,7,0,0,0),\"Pacific/Fiji\":new Date(2010,11,29,23,0,0,0),\"America/Halifax\":new Date(2011,2,13,6,0,0,0),\"America/Goose_Bay\":new Date(2011,2,13,2,1,0,0),\"America/Miquelon\":new Date(2011,2,13,5,0,0,0),\"America/Godthab\":new Date(2011,2,27,1,0,0,0),\"Europe/Moscow\":t,\"Asia/Yekaterinburg\":t,\"Asia/Omsk\":t,\"Asia/Krasnoyarsk\":t,\"Asia/Irkutsk\":t,\"Asia/Yakutsk\":t,\"Asia/Vladivostok\":t,\"Asia/Kamchatka\":t,\"Europe/Minsk\":t,\"Australia/Perth\":new Date(2008,10,1,1,0,0,0)};return n[e]};return{determine:a,date_is_dst:o,dst_start_for:f}}();t.TimeZone=function(e){\"use strict\";var n={\"America/Denver\":[\"America/Denver\",\"America/Mazatlan\"],\"America/Chicago\":[\"America/Chicago\",\"America/Mexico_City\"],\"America/Santiago\":[\"America/Santiago\",\"America/Asuncion\",\"America/Campo_Grande\"],\"America/Montevideo\":[\"America/Montevideo\",\"America/Sao_Paulo\"],\"Asia/Beirut\":[\"Asia/Beirut\",\"Europe/Helsinki\",\"Europe/Istanbul\",\"Asia/Damascus\",\"Asia/Jerusalem\",\"Asia/Gaza\"],\"Pacific/Auckland\":[\"Pacific/Auckland\",\"Pacific/Fiji\"],\"America/Los_Angeles\":[\"America/Los_Angeles\",\"America/Santa_Isabel\"],\"America/New_York\":[\"America/Havana\",\"America/New_York\"],\"America/Halifax\":[\"America/Goose_Bay\",\"America/Halifax\"],\"America/Godthab\":[\"America/Miquelon\",\"America/Godthab\"],\"Asia/Dubai\":[\"Europe/Moscow\"],\"Asia/Dhaka\":[\"Asia/Yekaterinburg\"],\"Asia/Jakarta\":[\"Asia/Omsk\"],\"Asia/Shanghai\":[\"Asia/Krasnoyarsk\",\"Australia/Perth\"],\"Asia/Tokyo\":[\"Asia/Irkutsk\"],\"Australia/Brisbane\":[\"Asia/Yakutsk\"],\"Pacific/Noumea\":[\"Asia/Vladivostok\"],\"Pacific/Tarawa\":[\"Asia/Kamchatka\"],\"Africa/Johannesburg\":[\"Asia/Gaza\",\"Africa/Cairo\"],\"Asia/Baghdad\":[\"Europe/Minsk\"]},r=e,i=function(){var e=n[r],i=e.length,s=0,o=e[0];for(;s<i;s+=1){o=e[s];if(t.date_is_dst(t.dst_start_for(o))){r=o;return}}},s=function(){return typeof n[r]!=\"undefined\"};return s()&&i(),{name:function(){return r}}},t.olson={},t.olson.timezones={\"-720,0\":\"Etc/GMT+12\",\"-660,0\":\"Pacific/Pago_Pago\",\"-600,1\":\"America/Adak\",\"-600,0\":\"Pacific/Honolulu\",\"-570,0\":\"Pacific/Marquesas\",\"-540,0\":\"Pacific/Gambier\",\"-540,1\":\"America/Anchorage\",\"-480,1\":\"America/Los_Angeles\",\"-480,0\":\"Pacific/Pitcairn\",\"-420,0\":\"America/Phoenix\",\"-420,1\":\"America/Denver\",\"-360,0\":\"America/Guatemala\",\"-360,1\":\"America/Chicago\",\"-360,1,s\":\"Pacific/Easter\",\"-300,0\":\"America/Bogota\",\"-300,1\":\"America/New_York\",\"-270,0\":\"America/Caracas\",\"-240,1\":\"America/Halifax\",\"-240,0\":\"America/Santo_Domingo\",\"-240,1,s\":\"America/Santiago\",\"-210,1\":\"America/St_Johns\",\"-180,1\":\"America/Godthab\",\"-180,0\":\"America/Argentina/Buenos_Aires\",\"-180,1,s\":\"America/Montevideo\",\"-120,0\":\"Etc/GMT+2\",\"-120,1\":\"Etc/GMT+2\",\"-60,1\":\"Atlantic/Azores\",\"-60,0\":\"Atlantic/Cape_Verde\",\"0,0\":\"Etc/UTC\",\"0,1\":\"Europe/London\",\"60,1\":\"Europe/Berlin\",\"60,0\":\"Africa/Lagos\",\"60,1,s\":\"Africa/Windhoek\",\"120,1\":\"Asia/Beirut\",\"120,0\":\"Africa/Johannesburg\",\"180,0\":\"Asia/Baghdad\",\"180,1\":\"Europe/Moscow\",\"210,1\":\"Asia/Tehran\",\"240,0\":\"Asia/Dubai\",\"240,1\":\"Asia/Baku\",\"270,0\":\"Asia/Kabul\",\"300,1\":\"Asia/Yekaterinburg\",\"300,0\":\"Asia/Karachi\",\"330,0\":\"Asia/Kolkata\",\"345,0\":\"Asia/Kathmandu\",\"360,0\":\"Asia/Dhaka\",\"360,1\":\"Asia/Omsk\",\"390,0\":\"Asia/Rangoon\",\"420,1\":\"Asia/Krasnoyarsk\",\"420,0\":\"Asia/Jakarta\",\"480,0\":\"Asia/Shanghai\",\"480,1\":\"Asia/Irkutsk\",\"525,0\":\"Australia/Eucla\",\"525,1,s\":\"Australia/Eucla\",\"540,1\":\"Asia/Yakutsk\",\"540,0\":\"Asia/Tokyo\",\"570,0\":\"Australia/Darwin\",\"570,1,s\":\"Australia/Adelaide\",\"600,0\":\"Australia/Brisbane\",\"600,1\":\"Asia/Vladivostok\",\"600,1,s\":\"Australia/Sydney\",\"630,1,s\":\"Australia/Lord_Howe\",\"660,1\":\"Asia/Kamchatka\",\"660,0\":\"Pacific/Noumea\",\"690,0\":\"Pacific/Norfolk\",\"720,1,s\":\"Pacific/Auckland\",\"720,0\":\"Pacific/Tarawa\",\"765,1,s\":\"Pacific/Chatham\",\"780,0\":\"Pacific/Tongatapu\",\"780,1,s\":\"Pacific/Apia\",\"840,0\":\"Pacific/Kiritimati\"},typeof exports!=\"undefined\"?exports.jstz=t:e.jstz=t})(this);</script><script>var timezone=jstz.determine();console.log('Your timezone is:' + timezone.name());document.getElementById('timezone').innerHTML = timezone.name();</script>";
#endif

#else
const char WM_HTTP_SCRIPT_NTP_MSG[] PROGMEM = "";
const char WM_HTTP_SCRIPT_NTP[] PROGMEM = "";
#endif

// KH, update from v1.0.10
const char WM_HTTP_HEAD_END[] PROGMEM = "</head><body><div class=\"container\"><div style=\"text-align:left;display:inline-block;min-width:260px;\">";

const char WM_FLDSET_START[]  PROGMEM = "<fieldset>";
const char WM_FLDSET_END[]    PROGMEM = "</fieldset>";
//////

const char WM_HTTP_PORTAL_OPTIONS[] PROGMEM = "<form action=\"/wifi\" method=\"get\"><button class=\"btn\">Configuration</button></form><br/><form action=\"/i\" method=\"get\"><button class=\"btn\">Information</button></form><br/><form action=\"/close\" method=\"get\"><button class=\"btn\">Exit Portal</button></form><br/>";
const char WM_HTTP_ITEM[] PROGMEM = "<div><a href=\"#p\" onclick=\"c(this)\">{v}</a>&nbsp;<span class=\"q {i}\">{r}%</span></div>";
const char JSON_ITEM[] PROGMEM = "{\"SSID\":\"{v}\", \"Encryption\":{i}, \"Quality\":\"{r}\"}";

// KH, update from v1.1.0
const char WM_HTTP_FORM_START[] PROGMEM = "<form method=\"get\" action=\"wifisave\"><fieldset><div><label>SSID</label><input id=\"s\" name=\"s\" length=32 placeholder=\"SSID\"><div></div></div><div><label>Password</label><input id=\"p\" name=\"p\" length=64 placeholder=\"password\"><div></div></div><div><label>SSID1</label><input id=\"s1\" name=\"s1\" length=32 placeholder=\"SSID1\"><div></div></div><div><label>Password</label><input id=\"p1\" name=\"p1\" length=64 placeholder=\"password1\"><div></div></div></fieldset>";
//////

// KH, add from v1.0.10
const char WM_HTTP_FORM_LABEL_BEFORE[] PROGMEM = "<div><label for=\"{i}\">{p}</label><input id=\"{i}\" name=\"{n}\" length={l} placeholder=\"{p}\" value=\"{v}\" {c}><div></div></div>";
const char WM_HTTP_FORM_LABEL_AFTER[] PROGMEM = "<div><input id=\"{i}\" name=\"{n}\" length={l} placeholder=\"{p}\" value=\"{v}\" {c}><label for=\"{i}\">{p}</label><div></div></div>";
//////

const char WM_HTTP_FORM_LABEL[] PROGMEM = "<label for=\"{i}\">{p}</label>";
const char WM_HTTP_FORM_PARAM[] PROGMEM = "<input id=\"{i}\" name=\"{n}\" length={l} placeholder=\"{p}\" value=\"{v}\" {c}>";

const char WM_HTTP_FORM_END[] PROGMEM = "<button class=\"btn\" type=\"submit\">Save</button></form>";

// KH, update from v1.1.0
const char WM_HTTP_SAVED[] PROGMEM = "<div class=\"msg\"><b>Credentials Saved</b><br>Try connecting ESP to the {x}/{x1} network. Wait around 10 seconds then check <a href=\"/\">if it's OK.</a> <p/>The {v} AP will run on the same WiFi channel of the {x}/{x1} AP. You may have to manually reconnect to the {v} AP.</div>";
//////

const char WM_HTTP_END[] PROGMEM = "</div></body></html>";

//KH, from v1.1.0
const char WM_HTTP_HEAD_CL[]         PROGMEM = "Content-Length";
const char WM_HTTP_HEAD_CT[]         PROGMEM = "text/html";
const char WM_HTTP_HEAD_CT2[]        PROGMEM = "text/plain";

//KH Add repeatedly used const
const char WM_HTTP_CACHE_CONTROL[]   PROGMEM = "Cache-Control";
const char WM_HTTP_NO_STORE[]        PROGMEM = "no-cache, no-store, must-revalidate";
const char WM_HTTP_PRAGMA[]          PROGMEM = "Pragma";
const char WM_HTTP_NO_CACHE[]        PROGMEM = "no-cache";
const char WM_HTTP_EXPIRES[]         PROGMEM = "Expires";
const char WM_HTTP_CORS[]            PROGMEM = "Access-Control-Allow-Origin";
const char WM_HTTP_CORS_ALLOW_ALL[]  PROGMEM = "*";

#if USE_AVAILABLE_PAGES
const char WM_HTTP_AVAILABLE_PAGES[] PROGMEM = "<h3>Available Pages</h3><table class=\"table\"><thead><tr><th>Page</th><th>Function</th></tr></thead><tbody><tr><td><a href=\"/\">/</a></td><td>Menu page.</td></tr><tr><td><a href=\"/wifi\">/wifi</a></td><td>Show WiFi scan results and enter WiFi configuration.</td></tr><tr><td><a href=\"/wifisave\">/wifisave</a></td><td>Save WiFi configuration information and configure device. Needs variables supplied.</td></tr><tr><td><a href=\"/close\">/close</a></td><td>Close the configuration server and configuration WiFi network.</td></tr><tr><td><a href=\"/i\">/i</a></td><td>This page.</td></tr><tr><td><a href=\"/r\">/r</a></td><td>Delete WiFi configuration and reboot. ESP device will not reconnect to a network until new WiFi configuration data is entered.</td></tr><tr><td><a href=\"/state\">/state</a></td><td>Current device state in JSON format. Interface for programmatic WiFi configuration.</td></tr><tr><td><a href=\"/scan\">/scan</a></td><td>Run a WiFi scan and return results in JSON format. Interface for programmatic WiFi configuration.</td></tr></table>";
#else
const char WM_HTTP_AVAILABLE_PAGES[] PROGMEM = "";
#endif

//KH
#define WIFI_MANAGER_MAX_PARAMS 20

// Thanks to @Amorphous for the feature and code, from v1.0.5
// (https://community.blynk.cc/t/esp-wifimanager-for-esp32-and-esp8266/42257/13)
// Form v1.0.10, enable to configure from sketch
#ifndef USE_CONFIGURABLE_DNS
  #define USE_CONFIGURABLE_DNS      true
#endif


class ESP_WMParameter 
{
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

// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You have to explicitly specify false to disable the feature.
#ifndef USE_STATIC_IP_CONFIG_IN_CP
  #define USE_STATIC_IP_CONFIG_IN_CP          true
#endif

class ESP_WiFiManager
{
  public:

    ESP_WiFiManager(const char *iHostname = "");

    ~ESP_WiFiManager();

    // Update feature from v1.0.11. Can use with STA staticIP now
    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);
    //////

    //if you want to start the config portal
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();
    // get the AP password of the config portal, so it can be used in the callback
    String        getConfigPortalPW();

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
    
    // KH, new from v1.0.10 to enable dynamic/random channel
    int           setConfigPortalChannel(int channel = 1);
    //////
    
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);

#if USE_CONFIGURABLE_DNS
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn,
                                       IPAddress dns_address_1, IPAddress dns_address_2);
#endif

    //called when AP mode and config portal is started
    void          setAPCallback(void(*func)(ESP_WiFiManager*));
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback(void(*func)(void));

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
    String				getSSID(void) 
    {
      return _ssid;
    }

    // return password of router in STA mode got from config portal. NULL if no user's input //KH
    String				getPW(void) 
    {
      return _pass;
    }
    
    // New from v1.1.0
    // return SSID of router in STA mode got from config portal. NULL if no user's input //KH
    String				getSSID1(void) 
    {
      return _ssid1;
    }

    // return password of router in STA mode got from config portal. NULL if no user's input //KH
    String				getPW1(void) 
    {
      return _pass1;
    }
    
    #define MAX_WIFI_CREDENTIALS        2
    
    String				getSSID(uint8_t index) 
    {
      if (index == 0)
        return _ssid;
      else if (index == 1)
        return _ssid1;
      else     
        return String("");
    }
    
    String				getPW(uint8_t index) 
    {
      if (index == 0)
        return _pass;
      else if (index == 1)
        return _pass1;
      else     
        return String("");
    }
    //////
    
    // New from v1.1.1, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
#if USING_CORS_FEATURE
    void setCORSHeader(const char * CORSHeaders = "*")
    {
      _CORS_Header = String(CORSHeaders);
      LOGWARN1(F("Set CORS Header to : "), _CORS_Header);
    }
    
    String getCORSHeader(void)
    {
      return _CORS_Header;
    }
#endif   
    
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

    const char*   _apName = "no-net";
    const char*   _apPassword = NULL;
    
    String        _ssid   = "";
    String        _pass   = "";
    
    // New from v1.1.0
    String        _ssid1  = "";
    String        _pass1  = "";
    //////
    
    // From v1.0.6 with timezone info
    String        _timezoneName = "";

    unsigned long _configPortalTimeout = 0;

    unsigned long _connectTimeout = 0;
    unsigned long _configPortalStart = 0;

    int numberOfNetworks;
    int *networkIndices;
    
    // KH, new from v1.0.10 to enable dynamic/random channel
    // default to channel 1
    #define MIN_WIFI_CHANNEL      1
    #define MAX_WIFI_CHANNEL      11    // Channel 12,13 is flaky, because of bad number 13 ;-)

    int _WiFiAPChannel = 1;
    //////

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip = IPAddress(0, 0, 0, 0);
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;

#if USE_CONFIGURABLE_DNS
    IPAddress     _sta_static_dns1;
    IPAddress     _sta_static_dns2;
#endif

    int           _paramsCount = 0;
    int           _minimumQuality = -1;
    boolean       _removeDuplicateAPs = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS = false;

    const char*   _customHeadElement = "";

    int           status = WL_IDLE_STATUS;
    
    // New from v1.1.0, for configure CORS Header, default to WM_HTTP_CORS_ALLOW_ALL = "*"
#if USING_CORS_FEATURE
    String        _CORS_Header              = FPSTR(WM_HTTP_CORS_ALLOW_ALL);
#endif   
    //////
    
    // New v1.0.8
    void          setWifiStaticIP(void);
    
    // New v1.1.0
    int           reconnectWifi(void);
    //////
    
    // New v1.0.11
    int           connectWifi(String ssid = "", String pass = "");
    //////
    
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

    void(*_apcallback)(ESP_WiFiManager*) = NULL;
    void(*_savecallback)(void) = NULL;

#if USE_DYNAMIC_PARAMS
    int                    _max_params;
    ESP_WMParameter** _params;
#else
    ESP_WMParameter* _params[WIFI_MANAGER_MAX_PARAMS];
#endif

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(obj->fromString(s)) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      LOGINFO("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};


#include "ESP_WiFiManager-Impl.h"

#endif
