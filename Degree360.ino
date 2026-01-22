#include <Wire.h>
#include <QMC5883LCompass.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

QMC5883LCompass compass;
ESP8266WebServer server(80);

// -------- SETTINGS --------
#define DECLINATION 1.2   // degrees (India ~0.5 to 2.0)
#define CALIBRATION_TIME 30000  // 30 seconds

// -------- CALIBRATION VARS --------
int xmin = 32767, xmax = -32768;
int ymin = 32767, ymax = -32768;
bool calibrated = false;
unsigned long calibStart;

float heading = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(D2, D1);
  compass.init();

  calibStart = millis();   // start auto-calibration

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(300);

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  readCompass();
  server.handleClient();
}

// ---------------- COMPASS ----------------
void readCompass() {
  compass.read();

  int x = compass.getX();
  int y = compass.getY();

  // -------- AUTO CALIBRATION --------
  if (!calibrated) {
    xmin = min(xmin, x);
    xmax = max(xmax, x);
    ymin = min(ymin, y);
    ymax = max(ymax, y);

    if (millis() - calibStart > CALIBRATION_TIME) {
      calibrated = true;
      Serial.println("Calibration completed");
    }
    return;
  }

  // -------- APPLY OFFSET --------
  float xoff = (xmax + xmin) / 2.0;
  float yoff = (ymax + ymin) / 2.0;

  float xc = x - xoff;
  float yc = y - yoff;

  // -------- HEADING --------
  float h = atan2(xc, yc) * 180.0 / PI;

  // True North correction
  h += DECLINATION;

  if (h < 0) h += 360.0;
  if (h >= 360) h -= 360.0;

  heading = h;

  Serial.print("Heading: ");
  Serial.println(heading);
}

// ---------------- DIRECTION LABEL ----------------
String directionLabel(float h) {
  if (h < 22.5) return "N";
  if (h < 67.5) return "NE";
  if (h < 112.5) return "E";
  if (h < 157.5) return "SE";
  if (h < 202.5) return "S";
  if (h < 247.5) return "SW";
  if (h < 292.5) return "W";
  if (h < 337.5) return "NW";
  return "N";
}

// ---------------- WEB PAGE ----------------
void handleRoot() {
  String html =
    "<!DOCTYPE html><html><head>"
    "<meta http-equiv='refresh' content='0.3'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<style>"
    "body{margin:0;background:black;color:white;"
    "display:flex;flex-direction:column;"
    "align-items:center;justify-content:center;"
    "height:100vh;font-family:Arial;}"
    "h1{font-size:20vw;font-weight:bold;margin:0;}"
    "h2{font-size:8vw;margin:0;}"
    "</style></head><body>";

  if (!calibrated) {
    html += "<h2>Calibrating...</h2>";
    html += "<h2>Rotate Sensor</h2>";
  } else {
    html += "<h1>" + String((int)heading) + "&deg;</h1>";
    html += "<h2>" + directionLabel(heading) + "</h2>";
  }

  html += "</body></html>";

  server.send(200, "text/html", html);
}
