# jura-coffee-machine
`cmd2jura.ino` is an HTTP gateway for the (serial) Jura service port that runs on an ESP8266.
This allows you to control your Jura coffee maker from a webinterface (power on Machine, make coffee etc.)
It also exposes a simple REST / POST API that can be utilized by Home automaization software like IO Broker, Homie, ITTT etc.

Fork of the awesome work of [Hajo Noerenberg](https://github.com/hn/jura-coffee-machine)
I re-implemented the webinterface, to make it more accessible. 
No breaking changes to the API, so you can use this as a replacement for the original project.

# Step-By-Step Setup

1. Download this repository
2. Install the Arduino IDE (if you don't have it already) and open it
3. Add the Board files to your Arduino IDE (File -> Settings -> Additional Board Manager URLs)
4. Connect ESP8266 board to your computer
5. Choose correct board and COM Port
6. open `cmd2jura/cmd2jura.ino` in the Arduino IDE
6. Install [WiFiManager](https://github.com/tzapu/WiFiManager) as library (Sketch->Libraries->Install from .zip)
7. Compile and upload .ino 
8. Upload Sketch Data (Tools -> ESP8266 Sketch Data Upload)

It uses [WiFiManager](https://github.com/tzapu/WiFiManager) to setup WiFi, so you have to connect to the `JURA` WLAN AP with your mobile phone first.
Once you put in your Wifi Credentials in WifiManager, you should be able access the Webinterface at:

`http://jura/` 

8. Connect ESP8266 Board to your Jura coffee makers "Service Port".

If you are using a [Wemos D1 mini](https://www.wemos.cc/product/d1-mini.html) (or clone) board, connect it up like this: 

![ESP8266](https://github.com/metalheim/jura-coffee-machine/blob/master/images/jura-esp8266-interface.jpg "Adafruit HUZZAH")


## Web UI

![Dark Mode](https://github.com/metalheim/jura-coffee-machine/blob/master/images/UI.html_Pixel2_prefer-dark.png)
![Light Mode](https://github.com/metalheim/jura-coffee-machine/blob/master/images/UI.html_Pixel2_prefer-light.png)

## API
Only accepts POST requests, the request body should be the "JURA Command".
The response will be plain text from the JURA serial connection.

```
user@workstation:~$ curl -d 'AN:01' http://jura/api
ok:
user@workstation:~$ curl -d 'TY:' http://jura/api
ty:E30   MASK 3
user@workstation:~$ curl -d 'RT:10' http://jura/api
rt:33DA01B1000C0640AA1116B301180000000000001E02007100150000000009D5
user@workstation:~$ 
```

