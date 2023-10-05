#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

const char* ssid = "WiFi/08/2.4GHz";
const char* password = "YaGo8AlGeBrA2001";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

int analogInputPin = A0;
String analogValue;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
    // Initialize LITTLEFS
   if (!LittleFS.begin()) {
    Serial.println("Errore di montaggio di LittleFS");
    return;
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();

  webSocket.begin();
}

void loop() {
  server.handleClient();
  webSocket.loop();
 // analogValue = analogRead(analogInputPin);
    analogValue = ECGFilter(analogRead(analogInputPin));
  
  // Invia il valore acquisito a tutti i client connessi tramite WebSocket
  webSocket.broadcastTXT(analogValue);
  delay(25);
}

void handleRoot() {
File f = LittleFS.open("\index.html", "r");
  server.send(200, "text/html", f);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  // Gli eventi WebSocket possono essere gestiti qui, ma non li usiamo in questo esempio.
}

String ECGFilter(float input)
{
  float output = input;
  {
    static float z1, z2; // filter section state
    float x = output - 0.70682283*z1 - 0.15621030*z2;
    output = 0.28064917*x + 0.56129834*z1 + 0.28064917*z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2; // filter section state
    float x = output - 0.95028224*z1 - 0.54073140*z2;
    output = 1.00000000*x + 2.00000000*z1 + 1.00000000*z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2; // filter section state
    float x = output - -1.95360385*z1 - 0.95423412*z2;
    output = 1.00000000*x + -2.00000000*z1 + 1.00000000*z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2; // filter section state
    float x = output - -1.98048558*z1 - 0.98111344*z2;
    output = 1.00000000*x + -2.00000000*z1 + 1.00000000*z2;
    z2 = z1;
    z1 = x;
  }
  
  return String(output);
}
