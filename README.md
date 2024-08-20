# WiFi-Capture-Page-M5Cardputer
Wi-Fi login capture page for M5Cardputer Stamp ESP32 S3.

## Device information:

https://docs.m5stack.com/en/core/Cardputer

## A running application on the device:

![fot1](https://raw.githubusercontent.com/ZrutrA/WiFi-Capture-Page-M5Cardputer/main/WiFi-Capture-Page-M5Cardputer.jpg)


## How to install an application from a .bin file?

Open https://web.esphome.io/ in a browser that supports WebSerial (Google Chrome or Microsoft Edge). Click "Connect", select the appropriate port, click "Connect". Then click "Install", point to the WiFi-Capture-Page-M5Cardputer.bin file and click "Install". Ready.

## Start-up and operation

Power on the programmed M5Cardputer module. Using your smartphone or computer, search for and log in to an open Wi-Fi network called "Free WiFi" (or another one if you changed the name during programming). Find and click "Use this network as is" in the menu. 
In your browser, go to 172.0.0.1/pass. Passwords will be visible there (if they are entered by people trying to log in to the access point).

![pass](https://github.com/ZrutrA/ESP32-WiFi-Captive-Portal/assets/155777818/65cd339a-011c-47a4-a990-e3840e6ea986)

If you click on "Index" you will see what the access point's home page looks like.

![index](https://github.com/ZrutrA/ESP32-WiFi-Captive-Portal/assets/155777818/fc16452e-c852-463b-8f13-caf5db87392b)

If you click "Change SSID" you will go to a subpage where you can change the network name of the access point.

![ssid](https://github.com/ZrutrA/ESP32-WiFi-Captive-Portal/assets/155777818/21155a40-538b-43b5-aad4-2023c474aaf5)

If you click "Clear Passwords" you will be able to clear the passwords saved on your device.

![clear](https://github.com/ZrutrA/ESP32-WiFi-Captive-Portal/assets/155777818/871341df-d318-4f31-bfc3-61934a300981)
