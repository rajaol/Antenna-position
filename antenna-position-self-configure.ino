#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

ESP8266WebServer server(80);

// Structure to store settings in flash memory
struct Settings {
  char ssid[32];
  char password[64];
  bool configDone;
};

Settings userSettings;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  
  // Load settings from EEPROM
  EEPROM.get(0, userSettings);

  if (userSettings.configDone) {
    Serial.println("Attempting to connect to saved WiFi...");
    WiFi.begin(userSettings.ssid, userSettings.password);
    
    // Wait 10 seconds for connection
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 20) {
      delay(500);
      Serial.print(".");
      retry++;
    }
  }

  // If connection failed or never configured, start Hotspot
  if (WiFi.status() != WL_CONNECTED) {
    startHotspot();
  } else {
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
  }

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
}

void startHotspot() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Config_Compass", ""); // No password
  Serial.println("Hotspot started: Config_Compass");
}

void handleRoot() {
  String html = "<h1>WiFi Setup</h1>";
  html += "<form action='/save' method='POST'>";
  html += "SSID: <input type='text' name='s'><br>";
  html += "Pass: <input type='password' name='p'><br>";
  html += "<input type='submit' value='Save and Connect'>";
  html += "</form>";
  server.send(200, "text/html", html);
}

void handleSave() {
  String newSSID = server.arg("s");
  String newPass = server.arg("p");

  newSSID.toCharArray(userSettings.ssid, 32);
  newPass.toCharArray(userSettings.password, 64);
  userSettings.configDone = true;

  EEPROM.put(0, userSettings);
  EEPROM.commit();

  server.send(200, "text/html", "Settings saved! Restarting and connecting...");
  delay(2000);
  ESP.restart(); // Reboot to apply settings
}

void loop() {
  server.handleClient();
}
