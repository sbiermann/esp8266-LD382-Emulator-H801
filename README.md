# Description
This project emulates an LD382 LED Controller with an ESP8266 on the H801 LED Controller.

# Hardware
The following hardware is used for this project:
* H801 LED Controller from Aliexpress: https://de.aliexpress.com/item/rgb-strip-WiFi-controller-1-port-control-15-rgb-lights-communicate-with-Android-phone-to-dim/32301423622.html
* LED Stripe, also from Aliexpress: https://de.aliexpress.com/item/New-arrival-RGBW-LED-strip-waterproof-24V-5050smd-60LED-m-5m-Roll-RGBW-LED-strip-light/32632508213.html

# Wiring
You have to solder the pins for the flashing (J3, RX, TX, GND and 3V3).
Note: For connecting you have to wire RX<->RX and TX<->TX and not crossed as normal.

# Known Issues
* W2 is not working currently
* the "on" sequenz is without any function
* Limited feature set implemented, only the things i needed for using it with FHEM

# Credits
Based on this Project: http://chaozlabs.blogspot.de/2015/08/esp8266-in-wild-wifi-led-controller-hack.html