/*
 * CST8227 Final Project - ESP32 
 * Features:
 * - Beautiful responsive web dashboard
 * - HTTP API for Node-RED
 * - Real-time data updates
 * - Fixed WiFi connection
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// ========== WIFI CREDENTIALS ==========
const char* ssid = "FinalProject1";        
const char* password = "12345678AB"; 

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

// Timing variables
unsigned long lastWiFiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 10000;
bool isConnecting = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n========================================");
  Serial.println("   ESP32 - CST8227 Dashboard");
  Serial.println("========================================");
  
  // Initialize Serial2 for MEGA communication
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial2 ready");
  
  // WiFi initialization with fix
  initWiFi();
  
  // Setup web server
  setupWebServer();
  
  Serial.println("\n========================================");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("READY!");
    Serial.print("  Dashboard: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi not connected, will retry...");
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
  
  // Check WiFi periodically
  if (millis() - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
    checkWiFiConnection();
    lastWiFiCheck = millis();
  }
}

// ========== WiFi FUNCTIONS (FIXED) ==========
void initWiFi() {
  // Full WiFi reset to avoid initialization errors
  WiFi.disconnect(false);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);
  delay(100);
  
  connectToWiFi();
}

void connectToWiFi() {
  if (isConnecting) {
    Serial.println("Already connecting, skipping...");
    return;
  }
  
  isConnecting = true;
  
  Serial.println("\n Connecting to WiFi...");
  Serial.print("   SSID: ");
  Serial.println(ssid);
  
  // Start connection (no disconnect here - already done in init)
  WiFi.begin(ssid, password);
  
  // Wait for connection with timeout
  int attempts = 0;
  Serial.print("   ");
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {  
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected!");
    Serial.print("   IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("   Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    if (WiFi.RSSI() < -75) {
      Serial.println("Weak signal - move closer to hotspot!");
    }
  } else {
    Serial.println("Connection failed!");
    Serial.println("   Check:");
    Serial.println("   • Hotspot is ON");
    Serial.println("   • SSID: FinalProject");
    Serial.println("   • Password: 12345678A");
    Serial.println("   • ESP32 is close to phone");
    Serial.println("   Will retry in 10 seconds...");
  }
  
  isConnecting = false;
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED && !isConnecting) {
    Serial.println("\nWiFi disconnected! Reconnecting...");
    WiFi.disconnect(false);
    delay(100);
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Reconnected! IP: ");
      Serial.println(WiFi.localIP());
    }
  } else if (WiFi.status() == WL_CONNECTED) {
    // Periodic status update
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

// ========== WEB SERVER ==========
void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/api/data", handleAPIData);
  server.on("/api/config", handleAPIConfig);
  server.begin();
  Serial.println("✓ Web server started");
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CST8227 Smart Monitor</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        :root {
            --primary: #6366f1;
            --primary-dark: #4f46e5;
            --success: #10b981;
            --warning: #f59e0b;
            --danger: #ef4444;
            --bg-dark: #0f172a;
            --bg-card: #1e293b;
            --bg-card-hover: #334155;
            --text-primary: #f8fafc;
            --text-secondary: #94a3b8;
            --border: #334155;
        }
        
        body {
            font-family: 'Segoe UI', system-ui, -apple-system, sans-serif;
            background: var(--bg-dark);
            color: var(--text-primary);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 1000px;
            margin: 0 auto;
        }
        
        /* Header */
        .header {
            text-align: center;
            margin-bottom: 30px;
            padding: 30px;
            background: linear-gradient(135deg, var(--primary) 0%, #8b5cf6 100%);
            border-radius: 20px;
            box-shadow: 0 10px 40px rgba(99, 102, 241, 0.3);
        }
        
        .header h1 {
            font-size: 28px;
            font-weight: 700;
            margin-bottom: 10px;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
        }
        
        .header .subtitle {
            font-size: 14px;
            opacity: 0.9;
        }
        
        /* Status Bar */
        .status-bar {
            display: flex;
            justify-content: center;
            gap: 20px;
            flex-wrap: wrap;
            margin-top: 15px;
            padding-top: 15px;
            border-top: 1px solid rgba(255,255,255,0.2);
        }
        
        .status-item {
            display: flex;
            align-items: center;
            gap: 6px;
            font-size: 13px;
        }
        
        .status-dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            animation: pulse 2s ease-in-out infinite;
        }
        
        .status-dot.online { background: var(--success); }
        .status-dot.offline { background: var(--danger); animation: none; }
        
        @keyframes pulse {
            0%, 100% { opacity: 1; transform: scale(1); }
            50% { opacity: 0.5; transform: scale(0.9); }
        }
        
        /* Passcode Card */
        .passcode-card {
            background: var(--bg-card);
            border-radius: 16px;
            padding: 25px;
            margin-bottom: 20px;
            text-align: center;
            border: 1px solid var(--border);
        }
        
        .passcode-label {
            font-size: 12px;
            text-transform: uppercase;
            letter-spacing: 2px;
            color: var(--text-secondary);
            margin-bottom: 10px;
        }
        
        .passcode-value {
            font-size: 48px;
            font-weight: 700;
            font-family: 'Courier New', monospace;
            letter-spacing: 15px;
            background: linear-gradient(135deg, var(--primary) 0%, #8b5cf6 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
        }
        
        /* Grid Layout */
        .grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 15px;
            margin-bottom: 20px;
        }
        
        @media (max-width: 600px) {
            .grid { grid-template-columns: 1fr; }
        }
        
        /* Cards */
        .card {
            background: var(--bg-card);
            border-radius: 16px;
            padding: 25px;
            border: 1px solid var(--border);
            transition: all 0.3s ease;
        }
        
        .card:hover {
            background: var(--bg-card-hover);
            transform: translateY(-2px);
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }
        
        .card-icon {
            font-size: 24px;
            margin-bottom: 10px;
        }
        
        .card-label {
            font-size: 12px;
            text-transform: uppercase;
            letter-spacing: 1px;
            color: var(--text-secondary);
            margin-bottom: 8px;
        }
        
        .card-value {
            font-size: 32px;
            font-weight: 700;
        }
        
        .card-unit {
            font-size: 16px;
            color: var(--text-secondary);
            margin-left: 4px;
        }
        
        /* Temperature Cards */
        .temp-preferred .card-value { color: var(--primary); }
        .temp-current .card-value { color: #06b6d4; }
        .humidity .card-value { color: #8b5cf6; }
        
        /* Status Cards */
        .status-on { color: var(--success); }
        .status-off { color: var(--text-secondary); }
        
        .motor-card.active {
            border-color: var(--success);
            box-shadow: 0 0 20px rgba(16, 185, 129, 0.2);
        }
        
        .motor-card.active .card-value {
            animation: motorSpin 2s linear infinite;
        }
        
        @keyframes motorSpin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        
        .night-card.active {
            border-color: #f59e0b;
            background: linear-gradient(135deg, var(--bg-card) 0%, rgba(245, 158, 11, 0.1) 100%);
        }
        
        .motion-card.active {
            border-color: var(--danger);
            box-shadow: 0 0 20px rgba(239, 68, 68, 0.2);
        }
        
        /* Footer */
        .footer {
            text-align: center;
            padding: 20px;
            color: var(--text-secondary);
            font-size: 13px;
        }
        
        .footer a {
            color: var(--primary);
            text-decoration: none;
        }
        
        /* Temperature Bar */
        .temp-bar {
            margin-top: 15px;
            height: 6px;
            background: var(--border);
            border-radius: 3px;
            overflow: hidden;
        }
        
        .temp-bar-fill {
            height: 100%;
            border-radius: 3px;
            transition: width 0.5s ease;
        }
        
        .temp-preferred .temp-bar-fill { background: var(--primary); }
        .temp-current .temp-bar-fill { background: #06b6d4; }
        
        /* Humidity Circle */
        .humidity-visual {
            display: flex;
            align-items: center;
            gap: 15px;
        }
        
        .humidity-circle {
            width: 60px;
            height: 60px;
            border-radius: 50%;
            background: conic-gradient(#8b5cf6 0% var(--humidity-percent), var(--border) var(--humidity-percent) 100%);
            display: flex;
            align-items: center;
            justify-content: center;
        }
        
        .humidity-circle-inner {
            width: 45px;
            height: 45px;
            border-radius: 50%;
            background: var(--bg-card);
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 12px;
            font-weight: 600;
        }
        
        /* Connection Info */
        .connection-info {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
            margin-top: 10px;
        }
        
        .connection-item {
            background: rgba(255,255,255,0.05);
            padding: 8px 12px;
            border-radius: 8px;
            font-size: 12px;
            text-align: center;
        }
        
        .connection-item span {
            display: block;
            color: var(--text-secondary);
            font-size: 10px;
            margin-bottom: 2px;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🌡️ CST8227 Smart Monitor</h1>
            <p class="subtitle">Multi-Sensor Monitoring & Control System</p>
            <div class="status-bar">
                <div class="status-item">
                    <div class="status-dot" id="statusDot"></div>
                    <span id="statusText">Connecting...</span>
                </div>
                <div class="status-item">
                    📶 <span id="rssiValue">--</span> dBm
                </div>
                <div class="status-item">
                    🌐 <span id="ipValue">--</span>
                </div>
            </div>
        </div>
        
        <div class="passcode-card">
            <div class="passcode-label">🔐 System Passcode</div>
            <div class="passcode-value" id="passcodeValue">----</div>
        </div>
        
        <div class="grid">
            <div class="card temp-preferred">
                <div class="card-icon">🎯</div>
                <div class="card-label">Preferred Temperature</div>
                <div class="card-value">
                    <span id="prefTempValue">0.0</span>
                    <span class="card-unit">°C</span>
                </div>
                <div class="temp-bar">
                    <div class="temp-bar-fill" id="prefTempBar" style="width: 0%"></div>
                </div>
            </div>
            
            <div class="card temp-current">
                <div class="card-icon">🌡️</div>
                <div class="card-label">Current Temperature</div>
                <div class="card-value">
                    <span id="currentTempValue">0.0</span>
                    <span class="card-unit">°C</span>
                </div>
                <div class="temp-bar">
                    <div class="temp-bar-fill" id="currentTempBar" style="width: 0%"></div>
                </div>
            </div>
        </div>
        
        <div class="grid">
            <div class="card humidity">
                <div class="card-icon">💧</div>
                <div class="card-label">Humidity</div>
                <div class="humidity-visual">
                    <div class="humidity-circle" id="humidityCircle" style="--humidity-percent: 0%">
                        <div class="humidity-circle-inner" id="humidityInner">0%</div>
                    </div>
                    <div class="card-value">
                        <span id="humidityValue">0</span>
                        <span class="card-unit">%</span>
                    </div>
                </div>
            </div>
            
            <div class="card motor-card" id="motorCard">
                <div class="card-icon" id="motorIcon">⚙️</div>
                <div class="card-label">Fan Motor</div>
                <div class="card-value" id="motorValue">OFF</div>
            </div>
        </div>
        
        <div class="grid">
            <div class="card night-card" id="nightCard">
                <div class="card-icon">🌙</div>
                <div class="card-label">Night Mode</div>
                <div class="card-value" id="nightValue">NO</div>
            </div>
            
            <div class="card motion-card" id="motionCard">
                <div class="card-icon">👁️</div>
                <div class="card-label">Motion Detected</div>
                <div class="card-value" id="motionValue">NO</div>
            </div>
        </div>
        
        <div class="footer">
            <p>CST8227 Interfacing Final Project | Auto-refresh: 2s</p>
            <p style="margin-top: 5px;">Algonquin College - School of Advanced Technology</p>
        </div>
    </div>
    
    <script>
        function updateDashboard() {
            fetch('/api/data')
                .then(response => response.json())
                .then(data => {
                    // Update passcode
                    document.getElementById('passcodeValue').textContent = data.passcode || '----';
                    
                    // Update temperatures
                    const prefTemp = data.prefTemp || 0;
                    const currentTemp = data.currentTemp || 0;
                    document.getElementById('prefTempValue').textContent = prefTemp.toFixed(1);
                    document.getElementById('currentTempValue').textContent = currentTemp.toFixed(1);
                    document.getElementById('prefTempBar').style.width = Math.min(prefTemp * 2, 100) + '%';
                    document.getElementById('currentTempBar').style.width = Math.min(currentTemp * 2, 100) + '%';
                    
                    // Update humidity
                    const humidity = data.humidity || 0;
                    document.getElementById('humidityValue').textContent = humidity;
                    document.getElementById('humidityInner').textContent = humidity + '%';
                    document.getElementById('humidityCircle').style.setProperty('--humidity-percent', humidity + '%');
                    
                    // Update motor
                    const motorOn = data.motorStatus === 'ON';
                    document.getElementById('motorValue').textContent = data.motorStatus || 'OFF';
                    document.getElementById('motorValue').className = 'card-value ' + (motorOn ? 'status-on' : 'status-off');
                    document.getElementById('motorCard').className = 'card motor-card' + (motorOn ? ' active' : '');
                    document.getElementById('motorIcon').textContent = motorOn ? '🌀' : '⚙️';
                    
                    // Update night mode
                    const nightOn = data.nightMode === 'YES';
                    document.getElementById('nightValue').textContent = data.nightMode || 'NO';
                    document.getElementById('nightValue').className = 'card-value ' + (nightOn ? 'status-on' : 'status-off');
                    document.getElementById('nightCard').className = 'card night-card' + (nightOn ? ' active' : '');
                    
                    // Update motion
                    const motionOn = data.motionStatus === 'YES';
                    document.getElementById('motionValue').textContent = data.motionStatus || 'NO';
                    document.getElementById('motionValue').className = 'card-value ' + (motionOn ? 'status-on' : 'status-off');
                    document.getElementById('motionCard').className = 'card motion-card' + (motionOn ? ' active' : '');
                    
                    // Update connection status
                    const connected = data.connected;
                    document.getElementById('statusDot').className = 'status-dot ' + (connected ? 'online' : 'offline');
                    document.getElementById('statusText').textContent = connected ? 'Receiving Data' : 'Waiting...';
                    document.getElementById('rssiValue').textContent = data.rssi || '--';
                    
                    // Get IP from config endpoint
                    fetch('/api/config')
                        .then(r => r.json())
                        .then(config => {
                            document.getElementById('ipValue').textContent = config.ip || '--';
                        });
                })
                .catch(error => {
                    console.error('Error:', error);
                    document.getElementById('statusDot').className = 'status-dot offline';
                    document.getElementById('statusText').textContent = 'Connection Error';
                });
        }
        
        // Initial update
        updateDashboard();
        
        // Auto-refresh every 2 seconds
        setInterval(updateDashboard, 2000);
    </script>
</body>
</html>
)rawliteral";
  
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

void handleAPIConfig() {
  StaticJsonDocument<128> doc;
  doc["ip"] = WiFi.localIP().toString();
  
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
      if (doc.containsKey("fanStatus")) motorStatus = doc["fanStatus"].as<String>();
      if (doc.containsKey("nightMode")) nightMode = doc["nightMode"].as<bool>() ? "YES" : "NO";
      if (doc.containsKey("motion")) motionStatus = doc["motion"].as<bool>() ? "YES" : "NO";
      
      lastDataReceived = millis();
      Serial.println("✓ Updated");
    } else {
      Serial.print("Parse error: ");
      Serial.println(error.c_str());
    }
  }
}