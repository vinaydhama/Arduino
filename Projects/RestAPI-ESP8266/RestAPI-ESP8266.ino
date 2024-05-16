/*
 *  Simple hello world Json REST response
  *  by Mischianti Renzo <https://mischianti.org>
 *
 *  https://mischianti.org/
 *
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

const char* ssid = "Vin-oppp";
const char* password = "Vin1@f!123";

ESP8266WebServer server(80);

// Serving Hello world
void getHelloWord() {
  server.send(200, "text/json", "{\"name\": \"Hello world1\"}");
  server.send(200, "text/json", "{\"myname\": \"Hello world2\"}");
}

void getHiWord() {
  server.send(200, "text/json", "{\"name\": \"Hi Word\"}");
}

// Define routing
void restServerRouting() {
  server.on("/", HTTP_GET, []() {
    server.send(200, F("text/html"),
                F("Welcome to the REST Web Server"));
  });
  server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
  server.on(F("/hioWorld"), HTTP_GET, getHiWord);
}

// Manage not found URL
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266qq")) {
    Serial.println("MDNS responder started");
  }

  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}