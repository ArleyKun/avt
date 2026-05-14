#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DHT.h>

const char* ssid     = "Arley"; // my legacy
const char* password = "starboy123";

#define DHTPIN  2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

ESP8266WebServer server(80);

bool wifiConnected = false;

unsigned long lastSend = 0;
unsigned long wifiTimeout = 10000; // max wait
const unsigned long SEND_INTERVAL = 2000;

float calculateHeatIndex(float temp, float hum) {
  return temp + 0.33 * (hum / 100.0 * 6.105 * exp(17.27 * temp / (237.7 + temp))) - 4.0;
}

const char INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Environmental Monitor</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&display=swap');
        :root { --bg-gradient: linear-gradient(135deg, #0f172a 0%, #1e293b 100%); --accent-color: #38bdf8; }
        body { font-family: 'Inter', sans-serif; background: var(--bg-gradient); min-height: 100vh; color: white; display: flex; align-items: center; justify-content: center; margin: 0; overflow-x: hidden; }
        .glass-card { background: rgba(255, 255, 255, 0.03); backdrop-filter: blur(12px); -webkit-backdrop-filter: blur(12px); border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 2rem; box-shadow: 0 25px 50px -12px rgba(0, 0, 0, 0.5); transition: all 0.3s ease; }
        .stat-value { background: linear-gradient(to bottom right, #fff, #94a3b8); -webkit-background-clip: text; -webkit-text-fill-color: transparent; font-weight: 700; }
        .glow { position: absolute; width: 300px; height: 300px; background: radial-gradient(circle, rgba(56, 189, 248, 0.15) 0%, rgba(56, 189, 248, 0) 70%); z-index: -1; filter: blur(40px); border-radius: 50%; }
        .ring-container { position: relative; display: inline-flex; align-items: center; justify-content: center; }
        svg circle { fill: none; stroke-width: 8; stroke-linecap: round; transform: rotate(-90deg); transform-origin: 50% 50%; transition: stroke-dashoffset 1s ease-in-out; }
        .bg-ring { stroke: rgba(255, 255, 255, 0.05); }
        .progress-ring { stroke: var(--accent-color); }
        @keyframes pulse-slow { 0%, 100% { opacity: 0.5; transform: scale(1); } 50% { opacity: 0.8; transform: scale(1.1); } }
        .animate-pulse-slow { animation: pulse-slow 8s infinite ease-in-out; }
    </style>
</head>
<body>
    <div class="glow animate-pulse-slow" style="top: 10%; left: 10%;"></div>
    <div class="glow animate-pulse-slow" style="bottom: 10%; right: 10%; background: radial-gradient(circle, rgba(139, 92, 246, 0.15) 0%, rgba(139, 92, 246, 0) 70%);"></div>
    <div class="container max-w-5xl px-6 py-12">
        <header class="text-center mb-12">
            <h1 class="text-4xl md:text-5xl font-bold mb-4 tracking-tight">Room Temperature</h1>
            <p class="text-slate-400 text-lg">Real-time environmental sensor data</p>
            <p class="text-slate-400 text-sm mt-2 opacity-70">Aromin - Haro - Mata - Gallentes</p>
        </header>
        <div class="grid grid-cols-1 md:grid-cols-3 gap-8">
            <div class="glass-card p-8 flex flex-col items-center text-center group hover:border-sky-500/50 transition-all duration-300">
                <div class="mb-6 p-4 bg-sky-500/10 rounded-2xl text-sky-400">
                    <svg xmlns="http://www.w3.org/2000/svg" width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 4v10.54a4 4 0 1 1-4 0V4a2 2 0 0 1 4 0Z"/></svg>
                </div>
                <h3 class="text-slate-400 font-medium uppercase tracking-widest text-sm mb-2">Temperature</h3>
                <div id="temp" class="stat-value text-5xl mb-2">--.--°C</div>
                <p class="text-slate-500 text-sm">Optimal Range</p>
            </div>
            <div class="glass-card p-8 flex flex-col items-center text-center group hover:border-orange-500/50 transition-all duration-300">
                <div class="mb-6 p-4 bg-orange-500/10 rounded-2xl text-orange-400">
                    <svg xmlns="http://www.w3.org/2000/svg" width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2v2"/><path d="m4.93 4.93 1.41 1.41"/><path d="M20 12h2"/><path d="m19.07 4.93-1.41 1.41"/><path d="M15.94 14.94a4 4 0 0 0-7.88 0"/><path d="M2 12h2"/><path d="m4.93 19.07 1.41-1.41"/><path d="M12 22v-2"/><path d="m19.07 19.07-1.41-1.41"/></svg>
                </div>
                <h3 class="text-slate-400 font-medium uppercase tracking-widest text-sm mb-2">Heat Index</h3>
                <div id="hindex" class="stat-value text-5xl mb-2" style="background: linear-gradient(to bottom right, #fff, #fb923c); -webkit-background-clip: text;">--.--°C</div>
                <p class="text-orange-400/80 text-sm font-semibold">Caution Level</p>
            </div>
            <div class="glass-card p-8 flex flex-col items-center text-center group hover:border-blue-400/50 transition-all duration-300">
                <div class="ring-container mb-6">
                    <svg width="100" height="100">
                        <circle class="bg-ring" cx="50" cy="50" r="42"></circle>
                        <circle id="humRing" class="progress-ring" cx="50" cy="50" r="42" style="stroke-dasharray: 263.89; stroke-dashoffset: 263.89;"></circle>
                    </svg>
                    <div class="absolute inset-0 flex items-center justify-center text-blue-400">
                        <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"/></svg>
                    </div>
                </div>
                <h3 class="text-slate-400 font-medium uppercase tracking-widest text-sm mb-2">Humidity</h3>
                <div id="hum" class="stat-value text-5xl mb-2">--.--%</div>
                <p class="text-slate-500 text-sm">Relative value</p>
            </div>
        </div>
        <div class="mt-12 p-6 glass-card border-none bg-white/5 flex items-center gap-4">
            <div id="statusDot" class="h-3 w-3 bg-red-500 rounded-full"></div>
            <p class="text-slate-300 text-sm">System Status: <span id="statusTxt">Disconnected</span>. Last update: <span id="time">Never</span></p>
        </div>
    </div>
    <script>
        function updateData() {
            fetch('/data').then(response => response.json()).then(data => {
                document.getElementById('temp').innerText = data.temp.toFixed(2) + "°C";
                document.getElementById('hum').innerText = data.hum.toFixed(2) + "%";
                document.getElementById('hindex').innerText = data.hindex.toFixed(2) + "°C";
                const ring = document.getElementById('humRing');
                const circumference = 2 * Math.PI * 42;
                const offset = circumference - (data.hum / 100) * circumference;
                ring.style.strokeDashoffset = offset;
                document.getElementById('statusDot').className = "h-3 w-3 bg-green-500 rounded-full animate-pulse";
                document.getElementById('statusTxt').innerText = "Connected";
                document.getElementById('time').innerText = new Date().toLocaleTimeString();
            }).catch(err => {
                document.getElementById('statusDot').className = "h-3 w-3 bg-red-500 rounded-full";
                document.getElementById('statusTxt').innerText = "Fetch Error";
            });
        }
        setInterval(updateData, 2000);
        updateData();
    </script>
</body>
</html>
)=====";

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    server.send(500, "application/json", "{\"error\":\"Sensor failure\"}");
    return;
  }

  float hi = calculateHeatIndex(t, h);
  String json = "{\"temp\":" + String(t) + ",\"hum\":" + String(h) + ",\"hindex\":" + String(hi) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(9600);
  delay(100);
  dht.begin();

  // --- Non-blocking WiFi connect with timeout --- thank ai for making it easier
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < wifiTimeout) {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("dhtley")) {
      Serial.println("mDNS: dhtley.local");
    }

    server.on("/", handleRoot);
    server.on("/data", handleData);
    server.begin();
    Serial.println("Web server started.");
  } else {
    wifiConnected = false;
    Serial.println("WiFi unavailable. Running in Serial-only mode.");
  }
}

void loop() {
  if (wifiConnected) {
  MDNS.update();
  server.handleClient();
  }

  if (millis() - lastSend >= SEND_INTERVAL) {
    lastSend = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(h) && !isnan(t)) {
      float hi = calculateHeatIndex(t, h);
      String payload = "T:" + String(t, 2) + ",H:" + String(h, 2) + ",HI:" + String(hi, 2);
      Serial.println(payload);  // TX is uno pin 10 (direct)
    } else {
      Serial.println("ERR");
    }
  }
}
