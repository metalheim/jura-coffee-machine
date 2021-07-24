
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
  String html;

  html  = "<!DOCTYPE html><html><head>";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><meta charset=\"UTF-8\">";
  html += "<style>*{ margin:0.5rem 0;} body {padding: 0 1rem;  color: #222;  background: #fff;  font: 100% system-ui;} @media (prefers-color-scheme: dark) {  body {    color: #eee;    background: #121212;  }";
  html += "</style></head>";
  html += "<body><h1>&#9749; Jura Impressa C5</h1>";
  html += "<form onsubmit=\"return s(this)\">";
  html += "<select id=\"c\" size=\"10\" style=\"width: 100%; height: auto; font-size: 1.25rem; -webkit-appearance: none;\">";
  html += "<option value=\"AN:01\">🟢 Power on</option>"; 
  html += "<option value=\"RT:00\">☕ Espresso</option>";
  html += "<option value=\"RT:00\">☕☕ Espresso</option>";
  html += "<option value=\"RT:00\">☕ Coffee</option>";
  html += "<option value=\"RT:00\">☕☕ Coffee</option>";
  html += "<option value=\"TY:\">🖶 Get Type of Machine</option>";
  html += "<option value=\"AN:02\">⭕ Power off</option>";
  html += "</select><br><br><input type=\"submit\" style=\"width: 100%; height:3em; border-radius:0.5em\">";
  html += "</form>";
  html += "<details><summary>Response Log</summary><ul style=\"font-family: monospace\" id=\"r\"></ul></details><script>function s(f) { var x = new XMLHttpRequest();";
  html += "x.open('POST', '/api', true); x.onreadystatechange = function() { if(x.readyState === XMLHttpRequest.DONE";
  //html += " && x.status === 200) { var r = document.getElementById('r'); r.innerHTML = '<li>' + Date().toLocaleString() + ";
  html += " ) { var r = document.getElementById('r'); r.innerHTML = '<li>' + Date().toLocaleString() + ";
  html += "':&emsp;' + f.c.value + '&emsp;&#8594;&emsp;' + x.responseText + '</li>' + r.innerHTML; }}; x.send(f.c.value);";
  html += "return false;}</script></body></html>";
  
  webserver.sendHeader("charset","UTF-8");
  webserver.send(200, "text/html", html);
}

void setup() {
  // put your setup code here, to run once:

  wifi_station_set_hostname(WIFINAME);

  WiFiManager wifimanager;
  wifimanager.autoConnect(WIFINAME);

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
