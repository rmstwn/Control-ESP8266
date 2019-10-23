#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#ifndef STASSID
#define STASSID "KY"
#define STAPSK ""
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
char fan[5], Mode[] = "manual";
char data[30];
int temp = 0;

ESP8266WebServer server(80);

void fanON()
{
  char ON[] = "ON";
  digitalWrite(LED_BUILTIN, LOW);
  sprintf(fan, ON);
}

void fanOFF()
{
  char OFF[] = "OFF";
  digitalWrite(LED_BUILTIN, HIGH);
  sprintf(fan, OFF);
}

void handleRoot()
{
  for (uint8_t i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "fan")
    {
      memset(fan, NULL, sizeof(fan));

      if (server.arg(i) == "ON")
      {
        fanON();
      }
      else if (server.arg(i) == "OFF")
      {
        fanOFF();
      }
    }
    if (server.argName(i) == "mode")
    {
      if (server.arg(i) == "manual")
      {
        char temp[] = "manual";
        sprintf(Mode, temp);
      }
      else if (server.arg(i) == "automatic")
      {
        char temp[] = "automatic";
        sprintf(Mode, temp);
      }
    }
  }

  server.send(200, "text/plain", "hello from esp8266!");
}

void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  server.handleClient();
  MDNS.update();

  HTTPClient http;
  if (http.begin("http://192.168.43.45:3000/data"))
  {
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    if (strcmp(Mode, "automatic") == 0)
    {
      if (temp > 250)
        fanON();
      else
        fanOFF();
    }

    sprintf(data, "fan=%s&temp=%d", fan, temp);

    int httpCode = http.POST(data);
    Serial.printf("[HTTP] code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
    {
      String payload = http.getString();
      Serial.println(payload);
    }
    else
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  else
    Serial.printf("[HTTP} Unable to connect\n");
  temp += 2;
  if (temp > 500)
    temp = 0;
  delay(10);
}
