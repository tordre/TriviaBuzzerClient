/*
   A simple firmware for a simple buzzer system for trivia event

    Copyright 2016 by K. Gilbert <tordre@hellotoaster.com>

   This file is part of some open source application.

   Some open source application is free software: you can redistribute
   it and/or modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, either
   version 3 of the License, or (at your option) any later version.

   Some open source application is distributed in the hope that it will
   be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with TriviaBuzzerClient.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <WiFiUdp.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

unsigned int localPort = 2733;        //what you say?
IPAddress serverIP(192, 168, 137, 1); //default ip for windows shared ip

// set pin numbers:
const int buttonPin[] = {14, 5, 12, 4};
int buttonState[] = {0, 0, 0, 0};

//led indicator pins
const int WifiConnectedPin = 16;
const int MessageReceivedPin = 2;

//string for formated MAC address to be used as a unique ID
String MAC;

void sendMessage(String data) {
  WiFiUDP port2;
  port2.beginPacket(serverIP, localPort);
  port2.write(data.c_str());
  port2.endPacket();
}

void setup() {
  Serial.begin(115200);
  Serial.println("started");

  //Setup wifi
  WiFiManager wifiManager;
  wifiManager.autoConnect();
  serverIP = WiFi.gatewayIP();

  //get and format MAC address
  byte mac[6];

  MAC += " " + String(mac[5], HEX);
  for (int i = 4; i >= 0; i--) {
    MAC += ":";
    MAC += String(mac[i], HEX);
  }
  Serial.println(MAC);
  
  //connected
  pinMode(      WifiConnectedPin, OUTPUT);
  digitalWrite( WifiConnectedPin, HIGH);
  pinMode(      MessageReceivedPin, OUTPUT);
  digitalWrite( MessageReceivedPin, LOW);
  
  // initialize the pushbutton pin as an input:
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPin[i], INPUT);
  }
}

void loop() {
  for (int i = 0; i < 4; i++) {
    int temp = digitalRead(buttonPin[i]);
    if ((buttonState[i] == LOW) && (temp == HIGH)) {
      String message = MAC + " pressed " + String(i + 1, DEC);
      Serial.println(message);
      sendMessage(message);
      digitalWrite( MessageReceivedPin, HIGH);
    }
     if ((buttonState[i] == HIGH) && (temp == LOW)) {
      digitalWrite( MessageReceivedPin, LOW);
    }
    buttonState[i] = temp;
  }
}
