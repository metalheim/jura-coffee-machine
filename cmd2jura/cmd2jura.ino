
/*

   cmd2jura.ino V1.00

   ESP8266 IP Gateway for Jura coffee machines

   Usage:
   Open "http://jura/" or "curl -d 'TY:' http://jura/api"

   (C) 2017 Hajo Noerenberg

   http://www.noerenberg.de/
   https://github.com/hn/jura-coffee-machine

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 3.0 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program. If not, see <http://www.gnu.org/licenses/gpl-3.0.txt>.

*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "FS.h"
#include <WiFiManager.h>
#include <SoftwareSerial.h>

#define WIFINAME  "JURA"

#define GPIORX    12
#define GPIOTX    13

#define GPIOLEDIO     4
#define GPIOLEDPULSE  0

SoftwareSerial softserial(GPIORX, GPIOTX);
ESP8266WebServer webserver(80);

String cmd2jura(String outbytes) {
  String inbytes;
  int w = 0;

  while (softserial.available()) {
    softserial.read();
  }

  outbytes += "\r\n";
  for (int i = 0; i < outbytes.length(); i++) {
    for (int s = 0; s < 8; s += 2) {
      char rawbyte = 255;
      bitWrite(rawbyte, 2, bitRead(outbytes.charAt(i), s + 0));
      bitWrite(rawbyte, 5, bitRead(outbytes.charAt(i), s + 1));
      softserial.write(rawbyte);
    }
    delay(8);
  }

  int s = 0;
  char inbyte;
  while (!inbytes.endsWith("\r\n")) {
    if (softserial.available()) {
      byte rawbyte = softserial.read();
      bitWrite(inbyte, s + 0, bitRead(rawbyte, 2));
      bitWrite(inbyte, s + 1, bitRead(rawbyte, 5));
      if ((s += 2) >= 8) {
        s = 0;
        inbytes += inbyte;
      }
    } else {
      delay(10);
    }
    if (w++ > 500) {
      return "";
    }
  }

  return inbytes.substring(0, inbytes.length() - 2);
}

boolean InitalizeFileSystem() 
{
  bool initok = false;
  initok = SPIFFS.begin();
  if (!(initok)) // Format SPIFS, of not formatted. - Try 1
  {
    //Serial.println("Format SPIFFS");
    SPIFFS.format();
    initok = SPIFFS.begin();
  }
  if (!(initok)) // Format SPIFS, of not formatted. - Try 2
  {
    SPIFFS.format();
    initok = SPIFFS.begin();
  }
  //if (initok) { Serial.println("SPIFFS is OK"); } else { Serial.println("SPIFFS is not OK"); }
  return initok;
}

void handle_api() {
  String cmd;
  String result;

  webserver.sendHeader("Access-Control-Allow-Origin","*");
  
  if (webserver.method() != HTTP_POST) {
    webserver.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  cmd = webserver.arg("plain");
  if (cmd.length() < 3) {
    webserver.send(400, "text/plain", "Bad Request");
    return;
  }

  digitalWrite(GPIOLEDIO, HIGH);
  result = cmd2jura(cmd);
  digitalWrite(GPIOLEDIO, LOW);

  if (result.length() < 3) {
    webserver.send(503, "text/plain", "Service Unavailable");
    return;
  }

  webserver.send(200, "text/plain", result);
}

void handle_web() {
  String error;
  String path = "/UI.html";
  
  webserver.sendHeader("charset","UTF-8");
  if (SPIFFS.exists(path)) {                            // If the file exists
    File uifile = SPIFFS.open(path, "r");                 // Open it
    size_t sent = webserver.streamFile(uifile, "text/html"); // And send it to the client
    uifile.close();                                       // Then close the file again
  }else{
    error = ("Failed to open UI html file for reading.\r\n");
    error += ("Please upload it to the SPIFFS File system using the ESP Upload tool");
    webserver.send(200, "text/plain", error);
  }
}

void setup() {
  // put your setup code here, to run once:

  wifi_station_set_hostname(WIFINAME);

  WiFiManager wifimanager;
  wifimanager.autoConnect(WIFINAME);
  
  InitalizeFileSystem();

  webserver.on("/", handle_web);
  webserver.on("/api", handle_api);
  webserver.begin();

  softserial.begin(9600);

  pinMode(GPIOLEDIO, OUTPUT);
  pinMode(GPIOLEDPULSE, OUTPUT);

}

int i = 0;
void loop() {
  // put your main code here, to run repeatedly:

  webserver.handleClient();

  i = (i + 1) % 200000;
  digitalWrite(GPIOLEDPULSE, (i < 100000) ? LOW : HIGH);
}
