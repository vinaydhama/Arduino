/*
  Modbus-Arduino Example - Test Holding Register (Modbus IP ESP8266)
  Configure Holding Register (offset 100) with initial value 0xABCD
  You can get or set this holding register
  Original library
  Copyright by André Sarmento Barbosa
  http://github.com/andresarmento/modbus-arduino

  Current version
  (c)2017 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266
*/

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else  //ESP32
#include <WiFi.h>
#endif

#include "Arduino.h"
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


#include <ModbusIP_ESP8266.h>

// Modbus Registers Offsets
const int TEST_HREG = 1;

const char* ssid = "Vin-oppp";
const char* password = "Vin1@f!123";

int myInt;

// Rest Server 
ESP8266WebServer RestServer(80);

//ModbusIP Server
ModbusIP modbusServer;
// Rest Functions
void getTurnOnFan() {
  myInt= 1;
  String s1= "{\"name\": \"TurnOnFan ";
  s1+= myInt;
  s1+= "\"}";
  RestServer.send(200, "text/json",s1);  
  //modbusServer.Hreg(TEST_HREG)= myInt;
  //mb.addHreg(TEST_HREG, myInt);
  Serial.println(myInt);

  }

void getOperateCooler() {
 String  cmdValue = RestServer.arg(1);
  //myInt= 2;
  String s1= "{\"name\": \"OperateCooler ";
  s1+= cmdValue;
  s1+= "\"}";
  RestServer.send(200, "text/json",s1);   
  Serial.println(cmdValue);
}


// Define routing
void restServerRouting() {
  RestServer.on("/", HTTP_GET, []() {
    RestServer.send(200, F("text/html"),
                F("Welcome to the REST Web Server"));
  });
  RestServer.on(F("/TurnOnFan*"), HTTP_GET, getTurnOnFan);
  RestServer.on(F("/OperateCooler/home_timeline.json"), HTTP_GET, getOperateCooler);
}
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += RestServer.uri();
  message += "\nMethod: ";
  message += (RestServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += RestServer.args();
  message += "\n";
  for (uint8_t i = 0; i < RestServer.args(); i++) {
    message += " " + RestServer.argName(i) + ": " + RestServer.arg(i) + "\n";
  }
  RestServer.send(404, "text/plain", message);
}



void setup() {
  Serial.begin(9600);

  //WiFi.begin("Maraf!", "VINF!123456");
  //WiFi.begin("Vin vivo", "VINF!123456");
  WiFi.begin("Vin-oppp", "Vin1@f!123");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

   // Set RestServer routing
  restServerRouting();
  // Set not found response
  RestServer.onNotFound(handleNotFound);
  // Start RestServer
  RestServer.begin();
  Serial.println("HTTP RestServer started");


  modbusServer.server(502);
  modbusServer.addHreg(TEST_HREG, 0, 4);
}

void loop() {
  //Call once inside loop() - all magic here
  modbusServer.task();  
  myInt = modbusServer.Hreg(TEST_HREG);  
  
  uint8_t myBytes[2] = { highByte(myInt), lowByte(myInt) };
  //  shiftOut(dataPin, clockPin, LSBFIRST, myBytes[0]);
  RestServer.handleClient();
  delay(100);
}
