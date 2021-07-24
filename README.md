# jura-coffee-machine

Fork of the awesome work of [Hajo Noerenberg](https://github.com/hn/jura-coffee-machine)

I am re-implementing the webinterface, to make it more accessible. 
No breaking changes to the API, so you can use this as a replacement for the original project.

## Tech
`jura-e65-coffee-machine.txt` lists some technical data (reverse engineered years ago) and `jura-e65-circuit-board.jpg` shows PCB photos of the Jura Impressa E65 coffee machine.

## ESP8266 / Arduino
`cmd2jura.ino` is an HTTP gateway for the (serial) Jura service port. It offers a minimal web interface available via `http://jura/` and exposes its POST API like this:

```
user@workstation:~$ curl -d 'AN:01' http://jura/api
ok:
user@workstation:~$ curl -d 'TY:' http://jura/api
ty:E30   MASK 3
user@workstation:~$ curl -d 'RT:10' http://jura/api
rt:33DA01B1000C0640AA1116B301180000000000001E02007100150000000009D5
user@workstation:~$ 
```

It uses [WiFiManager](https://github.com/tzapu/WiFiManager) to setup WiFi, so you have to connect to the `JURA` WLAN AP with your mobile phone first.

I suggest to use the [Adafruit HUZZAH ESP8266 Breakout](https://www.adafruit.com/product/2471) or the [Wemos D1 mini](https://www.wemos.cc/product/d1-mini.html) as a hardware basis. Both are super-easy to setup and you have your gateway up and running within 10 minutes.

![ESP8266](https://github.com/hn/jura-coffee-machine/blob/master/jura-esp8266-interface.jpg "Adafruit HUZZAH")

## Perl
`cmd2jura.pl` interfaces the coffee machine with the Raspberry Pi (indeed it should work with any device having a serial port and Perl). Example output:

```
root@raspberrypi:~# ./cmd2jura.pl AN:01
ok:
root@raspberrypi:~# ./cmd2jura.pl TY:
ty:E30   MASK 3
root@raspberrypi:~# ./cmd2jura.pl RT:10
rt:33DA01B1000C0640AA1116B301180000000000001E02007100150000000009D5
root@raspberrypi:~# 
```

![Raspberry](https://github.com/hn/jura-coffee-machine/blob/master/jura-raspberry-interface.jpg "Raspberry Pi connection 9600-8N1")

