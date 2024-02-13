#define WIFI_SSID "Bogdan_Secret"
#define WIFI_PASS "kchau2023"
#define BOT_TOKEN "5217042610:AAGrVfBq1KyYU0uYvJqSI-r0Q9t7bus3xFA"
#define FIRMWARE_VERSION	0.1
#define UPDATE_JSON_URL		"https://raw.githubusercontent.com/dz-vadim/roboesp/main/data/versionInfo.json?token=GHSAT0AAAAAACOEIVX3FCORTD7C4SAT2TYIZOKZ6OA"
#define TIMER_PERIOD 30000  

#include <FastBot.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

WebServer server(80);
FastBot bot(BOT_TOKEN);

void connectWiFi();
void setWebface();
void newMsg(FB_msg& msg);
void checkNewFirmware();

int timer_counter = 0;      
uint32_t timer = 0;   

void setup() {
  pinMode(2, OUTPUT);   //for check
  digitalWrite(2, 1);   //for check
  connectWiFi();
  Serial.println(WiFi.localIP());
  bot.attach(newMsg);
  setWebface();
}

void loop() {
  bot.tick();
  server.handleClient();
  checkNewFirmware();
}

void connectWiFi() {
  delay(2000);
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() > 15000) ESP.restart();
  }
  Serial.println("Connected");
}

void setWebface() {
  if (!SPIFFS.begin()) {
    Serial.println("FS Error");
    return;
  }

  server.begin();
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
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
  server.onNotFound([]() {
    File file = SPIFFS.open(server.uri(), "r");
    if (!file) {
      server.send(200, "text/plain", "Error");
      return;
    }
    server.streamFile(file, "text/plain");
    file.close();
  });
}

void newMsg(FB_msg& msg) {
  Serial.println(msg.toString());
  if (msg.OTA) {
    if (msg.fileName.indexOf("mkSPIFFS") > 0 ||
        msg.fileName.indexOf("spiffs") > 0) {
      bot.updateFS(); // update spiffs
    } else {
      bot.update();   // update firmware
    }
  }
}

void checkNewFirmware() {

  if (millis() - timer >= TIMER_PERIOD) { 
    timer = millis(); 
    timer_counter++;  
    if (timer_counter > 30) timer_counter = 0;  

    Serial.println("Searching for new firmware...");

    HTTPClient http;
    http.begin(UPDATE_JSON_URL);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      JSONVar payload = http.getString();
      
      JSONVar version = payload["version"];
      if (!version) {
        Serial.println("Unable to find field \"version\" in JSON, aborting...");
        return;
      }
      double new_version = atof(version);

      if (new_version > FIRMWARE_VERSION) {
        Serial.printf("Current firmware version (%.1f) is older than available one (%.1f), updating...\n", FIRMWARE_VERSION, new_version);

      } else {
        Serial.printf("Current firmware version (%.1f) is greater than or equal to the available one (%.1f), nothing to do...\n", FIRMWARE_VERSION, new_version);
      }
    } else {
      Serial.println("Failed to download JSON file, aborting...");
      return;
    }
    http.end();

    Serial.println();
  }
}