# ESP_FSWebServer Example

## First, how Config Portal works?
In `Configuration Portal Mode`, it starts an access point called `ESP_xxxxxx`. Connect to it using the configurable password you can define in the code. For example, `your_password` (see examples):

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

Enter your credentials, then click `Save`. The WiFi Credentials will be saved and the board reboots to connect to the selected WiFi AP.

If you're already connected to a listed WiFi AP and don't want to change anything, just select `Exit Portal` from the `Main` page to reboot the board and connect to the previously-stored AP. The WiFi Credentials are still intact.


## How to use this ESP_FSWebServer example?

This shows you how to use this example in Ubuntu (but you can use similar commands in other OSes)

1. For example, you already downloaded from (https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ESP_FSWebServer/data) to a local folder, e.g., 

~/Arduino/libraries/ESP_WiFiManager-master/examples/ESP_FSWebServer/data

1. Upload the contents of that `data folder`  with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
2. or upload the contents of a folder by running the following commands: 
 - Ubuntu$ cd ~/Arduino/libraries/ESP_WiFiManager-master/examples/ESP_FSWebServer/data
 - Ubuntu$ for file in \`\ls -A1\`; do curl -F "file=@$PWD/$file" esp8266fs.local/edit; done
3. Access the sample web page at http://esp8266fs.local

<p align="center">
    <img src="https://github.com/khoih-prog/ESP_WiFiManager/blob/master/examples/ESP_FSWebServer/pics/esp8266fs.local.png">
</p>

4. Edit / Delete / Download any file in the the folder by going to http://esp8266fs.local/edit

<p align="center">
    <img src="https://github.com/khoih-prog/ESP_WiFiManager/blob/master/examples/ESP_FSWebServer/pics/esp8266fs.local_edit.png">
</p>


