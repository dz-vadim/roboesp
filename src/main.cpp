#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "SPIFFS.h"

const char* host = "esp32";
const char* ssid = "9_malyi_repet";
const char* password = "vperova2019";

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}


WebServer server(80);

void server_init() {
    Serial.begin(115200);
   initSPIFFS();
  // Connect to WiFi network
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

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
 
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    // server.send(200, "text/html", loginIndex);
     File file = SPIFFS.open("/index.html", "r");
     server.streamFile(file, "text/html");

  });
  server.on("/server", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
     File file = SPIFFS.open("/server.html", "r");
     server.streamFile(file, "text/html");
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}
void server_runing(){
  server.handleClient();
  delay(1);
}

void setup(void) {
  server_init(); //dont delete this line to save ota update
  pinMode(2, OUTPUT);
}
long del = 0;
bool ttimer (long d) {
  if (del - d > millis())
  {
    del = millis();
    return true;
  }
  return false;
  Serial.println(del);
}

void loop(void) {
  server_runing(); //dont delete this line to save ota update
  if (ttimer(1000))
  digitalWrite(2,1);
  if (ttimer(1000))
  digitalWrite(2,0);
  ttimer(1000);  
}
