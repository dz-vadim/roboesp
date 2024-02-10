// #include <Arduino.h>

// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <WebServer.h>
// #include <ESPmDNS.h>
// #include <Update.h>
// #include "SPIFFS.h"
// #include <ESPAsyncWebServer.h>
// #include <Arduino_JSON.h>



// const char* host = "esp32";
// const char* ssid = "Bogdan_Secret";
// const char* password = "kchau2023";

// AsyncWebServer server(80);

// AsyncWebSocket ws("/ws");

// void initSPIFFS() {
//   if (!SPIFFS.begin(true)) {
//     Serial.println("An error has occurred while mounting SPIFFS");
//   }
//   Serial.println("SPIFFS mounted successfully");
// }
// void initWiFi() {
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi ..");
//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print('.');
//     delay(1000);
//   }
//   Serial.println(WiFi.localIP());
// }
// void notifyClients(String state) {
//   ws.textAll(state);
// }
// void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
//   AwsFrameInfo *info = (AwsFrameInfo*)arg;
//   if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
//     data[len] = 0;
//     if (strcmp((char*)data, "states") == 0) {
//       notifyClients((char*)info);
//     }
//   }
// }
// void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type,
//              void *arg, uint8_t *data, size_t len) {
//   switch (type) {
//     case WS_EVT_CONNECT:
//       Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
//       break;
//     case WS_EVT_DISCONNECT:
//       Serial.printf("WebSocket client #%u disconnected\n", client->id());
//       break;
//     case WS_EVT_DATA:
//       handleWebSocketMessage(arg, data, len);
//       break;
//     case WS_EVT_PONG:
//     case WS_EVT_ERROR:
//       break;
//   }
// }

// void initWebSocket() {
//     ws.onEvent(onEvent);
//     server.addHandler(&ws);
// }


// void server_init() {
//     Serial.begin(115200);

//     initSPIFFS();
//     initWiFi();
//     initWebSocket();

//   /*use mdns for host name resolution*/
//   if (!MDNS.begin(host)) { //http://esp32.local
//     Serial.println("Error setting up MDNS responder!");
//     while (1) {
//       delay(1000);
//     }
//   }
//   Serial.println("mDNS responder started");
//   /*return index page which is stored in serverIndex */
//   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//     request->send(SPIFFS, "/index.html", "text/html",false);
//   });

//   server.on("/server", HTTP_GET, [](AsyncWebServerRequest *request){
//     request->send(SPIFFS, "/server.html", "text/html",false);
//   });

//   // server.on("/serverIndex", HTTP_GET, []() {
//   //   server.sendHeader("Connection", "close");
//   //   server.send(200, "text/html", serverIndex);
//   // });
  
//   /*handling uploading firmware file */
//   server.on("/update", HTTP_POST, []() {
//     server.sendHeader("Connection", "close");
//     server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
//     ESP.restart();
//   }, [](AsyncWebServerRequest *request) {
//     HTTPUpload& upload = server.upload();
//     if (upload.status == UPLOAD_FILE_START) {
//       Serial.printf("Update: %s\n", upload.filename.c_str());
//       if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
//         Update.printError(Serial);
//       }
//     } else if (upload.status == UPLOAD_FILE_WRITE) {
//       /* flashing firmware to ESP*/
//       if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
//         Update.printError(Serial);
//       }
//     } else if (upload.status == UPLOAD_FILE_END) {
//       if (Update.end(true)) { //true to set the size to the current progress
//         Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
//       } else {
//         Update.printError(Serial);
//       }
//     }
//   });

//   server.begin();
// }
// void server_runing(){
//   server.handleClient();
//   delay(1);
// }
// void setup(void) {
//   server_init(); //dont delete this line to save ota update
// }

// void loop(void) {
//   server_runing(); //dont delete this line to save ota update
//   digitalWrite(2,1);
// }
