# Corsair SP120 RGB Custom Controller
Made on a NodeMCU ESP8266.
This allows you to access to a WebServer (IP of your ESP8266) and control the status of the fans, color and effects (Work in Progress)

## What will you need?
* ESP 8266 (Wemos D1 Mini, etc)
* Jumper Wires
* Corsair SP120 RGB Fans Controller

### Wiring:
![Wiring Image](https://github.com/petealves/Corsair-SP120-RGB-ESP8266-Custom-Controller/blob/master/SP120-ESP-Wiring.png)

Note: D2 can be changed to another pin, just change it on the code!

### Steps:

1- On [Arduino IDE](https://www.arduino.cc/en/main/software), add the [ESP8266 Boards] (https://github.com/esp8266/Arduino);<br/>
2- Download and add [FastLED Library] (https://github.com/FastLED/FastLED);<br/>
3- Open the code on Arduino IDE;<br/>
4- Change the NR_FANS to the number of your fans;<br/>
5- Change SSID and Password to match your network;<br/>
6- Select the proper Nodemcu ESP8266 board and uload your code;<br/>
7- Go to your Router page and look for the IP of "Corsair-SP120-RGB Controller" or open Serial Monitor on BAUD 115200 to see the IP;<br/>
8- Go to your browser and type the IP;<br/>
9- Done!

### Working:

* Status ON/OFF;
* Brightness adjustments;
* Color Changing;
* Effects:
  * -Rainbow Cycle (with speed adjustment;
  * -Bpm;
  * -Juggle;
  
  ![WebPage Image](https://github.com/petealves/Corsair-SP120-RGB-ESP8266-Custom-Controller/blob/master/WebSite_Example.PNG)
  
 
 ### Work in progress:
 * More effects;
 * Control individual Fans
