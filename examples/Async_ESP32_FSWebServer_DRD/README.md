# Async_ESP32_FSWebServer_DRD Example

Compare this efficient Async_ESP32_FSWebServer_DRD example with the so complicated twin [ESP32_FSWebServer_DRD](https://github.com/khoih-prog/ESP_WiFiManager/tree/master/examples/ESP32_FSWebServer_DRD) to appreciate the powerful AsynWebServer this [ESPAsync_WiFiManager Library](https://github.com/khoih-prog/ESPAsync_WiFiManager) is relying on.

## First, how Config Portal works?

In `Configuration Portal Mode`, it starts an access point called `ESP_xxxxxx`. Connect to it using the configurable password you can define in the code. For example, `your_password` (see examples):

```cpp
// SSID and PW for Config Portal
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
const char* password = "your_password";
```
After you connected, please, go to http://192.168.4.1, you'll see this `Main` page:

<p align="center">
    <img src="https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/Images/Main.png">
</p>

Select `Information` to enter the Info page where the board info will be shown (long page)

<p align="center">
    <img src="https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/Images/Info.png">
</p>

or short page (default)

<p align="center">
    <img src="https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/Images/Info_Short.png">
</p>

Select `Configuration` to enter this page where you can select an AP and specify its WiFi Credentials

<p align="center">
    <img src="https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/Images/Configuration.png">
</p>

Enter your credentials, then click `Save`. The WiFi Credentials will be saved and the board reboots to connect to the selected WiFi AP.

If you're already connected to a listed WiFi AP and don't want to change anything, just select `Exit Portal` from the `Main` page to reboot the board and connect to the previously-stored AP. The WiFi Credentials are still intact.

---

## How to use this Async_ESP32_FSWebServer_DRD example?

This shows you how to use this example in Ubuntu (but you can use similar commands in other OSes)

### Download Data files

1. For example, you already downloaded data files from [Async_ESP32_FSWebServer_DRD data](https://github.com/khoih-prog/ESPAsync_WiFiManager/tree/master/examples/Async_ESP32_FSWebServer_DRD/data) to a local folder, for example:

```
~/Arduino/libraries/ESPAsync_WiFiManager-master/examples/Async_ESP32_FSWebServer_DRD/data
```

### HOWTO Upload files to ESP32 (SPIFFS or FFat)

Use one of these methods (preferable first)

1. Go to http://async-esp32fs.local/edit, then "Choose file" -> "Upload"
2. or Upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
3. or upload the contents of a folder by running the following commands: 
```
Ubuntu$ cd ~/Arduino/libraries/ESPAsync_WiFiManager-master/examples/Async_ESP32_FSWebServer_DRD/data
Ubuntu$ for file in \`\ls -A1\`; do curl -F "file=@$PWD/$file" http://async-esp32fs.local/edit; done
```

---

### Demonstrating pictures

<p align="center">
    <img src="https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/examples/Async_ESP32_FSWebServer_DRD/pics/async-esp32fs.local.png">
</p>

4. Edit / Delete / Download any file in the the folder by going to http://async-esp32fs.local/edit

<p align="center">
    <img src="https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/examples/Async_ESP32_FSWebServer_DRD/pics/async-esp32fs.local_edit.png">
</p>


