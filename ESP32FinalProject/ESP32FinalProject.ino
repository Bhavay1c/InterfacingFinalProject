
/*
 * CST8227 Final Project - ESP32 FIXED WiFi
 * No more reconnection loops!
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// ========== WIFI CREDENTIALS ==========
const char* ssid = "FinalProject";        
const char* password = "12345678A"; 

WebServer server(80);

// ========== SERIAL CONFIGURATION ==========
#define RXD2 16
#define TXD2 17

// ========== GLOBAL VARIABLES ==========
String passcode = "----";
float prefTemp = 0.0;
float currentTemp = 0.0;
int humidity = 0;
String motorStatus = "OFF";
String nightMode = "NO";
String motionStatus = "NO";
unsigned long lastDataReceived = 0;

// WiFi check timing
unsigned long lastWiFiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 10000;  // Check every 10 seconds (not constantly!)
bool isConnecting = false;  // Prevent overlapping connection attempts

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n========================================");
  Serial.println("   ESP32 - FIXED WiFi Connection");
  Serial.println("========================================");
  
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("✓ Serial2 ready");
  
  // Configure WiFi for stability
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);  // Enable auto-reconnect
  WiFi.persistent(false);       // Don't save WiFi config to flash (faster)
  
  connectToWiFi();
  setupWebServer();
  
  Serial.println("\n========================================");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("  🎉 READY!");
    Serial.print("  Dashboard: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("  ⚠️  WiFi failed but continuing...");
  }
  Serial.println("========================================\n");
}

void loop() {
  // Handle web requests
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  }
  
  // Read data from MEGA-1
  if (Serial2.available()) {
    readDataFromMEGA();
  }
  
  // Check WiFi periodically (NOT every loop iteration!)
  if (millis() - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
    checkWiFiConnection();
    lastWiFiCheck = millis();
  }
}

// ========== WiFi FUNCTIONS ==========
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED && !isConnecting) {
    Serial.println("\n⚠️  WiFi disconnected!");
    Serial.println("🔄 Attempting reconnect...");
    connectToWiFi();
  } else if (WiFi.status() == WL_CONNECTED) {
    // Occasional status update
    static int statusCount = 0;
    if (statusCount++ % 6 == 0) {  // Every 60 seconds
      Serial.print("✓ WiFi OK | IP: ");
      Serial.print(WiFi.localIP());
      Serial.print(" | Signal: ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
    }
  }
}

void connectToWiFi() {
  if (isConnecting) {
    Serial.println("Already connecting, skipping...");
    return;
  }
  
  isConnecting = true;
  
  Serial.println("\n🔌 Connecting to WiFi...");
  Serial.print("   SSID: ");
  Serial.println(ssid);
  
  // Disconnect cleanly first
  WiFi.disconnect(true);
  delay(100);
  
  // Start connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection (with timeout)
  int attempts = 0;
  Serial.print("   ");
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {  // 15 seconds max
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ Connected!");
    Serial.print("   IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("   Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    if (WiFi.RSSI() < -75) {
      Serial.println("   ⚠️  Weak signal - move closer!");
    }
  } else {
    Serial.println("❌ Connection failed!");
    Serial.println("   Check:");
    Serial.println("   • Hotspot is ON");
    Serial.println("   • SSID: FinalProject");
    Serial.println("   • Password: 12345678A");
    Serial.println("   • ESP32 is close to phone");
    Serial.println("   Will retry in 10 seconds...");
  }
  
  isConnecting = false;
}

// ========== WEB SERVER ==========
void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/api/data", handleAPIData);
  server.begin();
  Serial.println("✓ Web server started");
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<meta http-equiv='refresh' content='3'>";
  html += "<title>CST8227 Dashboard</title>";
  html += "<style>";
  html += "*{margin:0;padding:0;box-sizing:border-box;}";
  html += "body{font-family:'Segoe UI',Arial,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);padding:20px;min-height:100vh;}";
  html += ".container{max-width:900px;margin:0 auto;}";
  html += "h1{text-align:center;color:white;margin-bottom:30px;font-size:32px;text-shadow:2px 2px 4px rgba(0,0,0,0.3);}";
  html += ".header{background:rgba(255,255,255,0.95);border-radius:15px;padding:20px;margin-bottom:20px;text-align:center;}";
  html += ".status{font-size:14px;color:#666;margin-top:10px;}";
  html += ".online{color:#22c55e;font-weight:bold;}";
  html += ".offline{color:#ef4444;font-weight:bold;}";
  html += ".card{background:white;border-radius:15px;padding:25px;margin:15px 0;box-shadow:0 10px 30px rgba(0,0,0,0.2);}";
  html += ".label{font-size:14px;color:#666;text-transform:uppercase;letter-spacing:1px;margin-bottom:8px;}";
  html += ".value{font-size:36px;font-weight:bold;color:#667eea;}";
  html += ".status-on{color:#22c55e;}";
  html += ".status-off{color:#ef4444;}";
  html += ".grid{display:grid;grid-template-columns:1fr 1fr;gap:15px;}";
  html += ".footer{text-align:center;color:white;margin-top:30px;opacity:0.8;font-size:14px;}";
  html += "@media(max-width:600px){.grid{grid-template-columns:1fr;}}";
  html += ".pulse{animation:pulse 2s ease-in-out infinite;}";
  html += "@keyframes pulse{0%,100%{opacity:1;}50%{opacity:0.6;}}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>🌡️ CST8227 System Monitor</h1>";
  
  // Status
  html += "<div class='header'>";
  html += "<h2 style='color:#667eea;margin-bottom:10px;'>Status</h2>";
  unsigned long timeSince = millis() - lastDataReceived;
  if (timeSince < 5000) {
    html += "<div class='status'><span class='online'>● RECEIVING DATA</span> (" + String(timeSince/1000) + "s ago)</div>";
  } else {
    html += "<div class='status'><span class='offline'>● WAITING</span></div>";
  }
  html += "<div class='status'>IP: " + WiFi.localIP().toString() + " | Signal: " + String(WiFi.RSSI()) + " dBm</div>";
  html += "</div>";
  
  // Passcode
  html += "<div class='card'>";
  html += "<div class='label'>Passcode</div>";
  html += "<div class='value'>" + passcode + "</div>";
  html += "</div>";
  
  // Temperature
  html += "<div class='grid'>";
  html += "<div class='card'><div class='label'>Preferred</div>";
  html += "<div class='value'>" + String(prefTemp, 1) + "°C</div></div>";
  html += "<div class='card'><div class='label'>Current</div>";
  html += "<div class='value'>" + String(currentTemp, 1) + "°C</div></div>";
  html += "</div>";
  
  // Humidity & Motor
  html += "<div class='grid'>";
  html += "<div class='card'><div class='label'>Humidity</div>";
  html += "<div class='value'>" + String(humidity) + "%</div></div>";
  html += "<div class='card'><div class='label'>Motor</div>";
  html += "<div class='value " + String(motorStatus=="ON"?"status-on pulse":"status-off") + "'>" + motorStatus + "</div></div>";
  html += "</div>";
  
  // Night & Motion
  html += "<div class='grid'>";
  html += "<div class='card'><div class='label'>Night</div>";
  html += "<div class='value'>" + nightMode + "</div></div>";
  html += "<div class='card'><div class='label'>Motion</div>";
  html += "<div class='value'>" + motionStatus + "</div></div>";
  html += "</div>";
  
  html += "<div class='footer'>CST8227 Project | Refresh: 3s</div>";
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

void handleAPIData() {
  StaticJsonDocument<256> doc;
  doc["passcode"] = passcode;
  doc["prefTemp"] = prefTemp;
  doc["currentTemp"] = currentTemp;
  doc["humidity"] = humidity;
  doc["motorStatus"] = motorStatus;
  doc["nightMode"] = nightMode;
  doc["motionStatus"] = motionStatus;
  doc["connected"] = (millis() - lastDataReceived < 5000);
  doc["rssi"] = WiFi.RSSI();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// ========== DATA HANDLING ==========
void readDataFromMEGA() {
  String data = Serial2.readStringUntil('\n');
  data.trim();
  
  if (data.length() > 0) {
    Serial.print("📨 ");
    Serial.println(data);
    
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, data);
    
    if (!error) {
      if (doc.containsKey("passcode")) passcode = doc["passcode"].as<String>();
      if (doc.containsKey("prefTemp")) prefTemp = doc["prefTemp"];
      if (doc.containsKey("currentTemp")) currentTemp = doc["currentTemp"];
      if (doc.containsKey("humidity")) humidity = doc["humidity"];
      if (doc.containsKey("motorStatus")) motorStatus = doc["motorStatus"].as<String>();
      if (doc.containsKey("nightMode")) nightMode = doc["nightMode"].as<bool>() ? "YES" : "NO";
      if (doc.containsKey("motion")) motionStatus = doc["motion"].as<bool>() ? "YES" : "NO";
      
      lastDataReceived = millis();
      Serial.println("✓ Updated");
    } else {
      Serial.print("❌ Parse error: ");
      Serial.println(error.c_str());
    }
  }
}

