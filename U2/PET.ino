#include <WiFi.h>
#include <HTTPClient.h>


const char *ssid = "esp_wifi";
const char *password = "password";

// Your Domain name with URL path or IP address with path
String serverName = "http://192.168.4.7:7800/";
;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
// unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void post_asc()
{
  DynamicJsonDocument json_chido(1024);
  json_chido["action"] = "asc";
  json_chido["quantity"] = 1;

  //cadena jason para enviar
  String json_str;
  serializejson(json_chido, json_str);

  //Enviar POST
  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(json_str);

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP ");
  }
}

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop()
{
    // Send an HTTP POST request every 10 minutes
    if ((millis() - lastTime) > timerDelay)
    {
        // Check WiFi connection status
        if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient http;

            String serverPath = serverName;

            // Your Domain name with URL path or IP address with path
            http.begin(serverPath.c_str());

            // If you need Node-RED/server authentication, insert user and password below
            // http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

            // Send HTTP GET request
            int httpResponseCode = http.GET();

            if (httpResponseCode > 0)
            {
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);
                String payload = http.getString();
                Serial.println(payload);
            }
            else
            {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            }
            // Free resources
            http.end();
        }
        else
        {
            Serial.println("WiFi Disconnected");
        }
        lastTime = millis();
    }
}