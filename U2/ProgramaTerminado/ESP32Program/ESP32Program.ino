#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "INFINITUME7D8";
const char *password = "kasumikasumi";

String serverTemperatureURL = "http://192.168.1.73:7800/temperature";
String serverHumidityURL = "http://192.168.1.73:7800/humidity";
String serverName = "http://192.168.1.73:7800/";
String serverLed = "http://192.168.1.73:7800/led"; // Endpoint para obtener el estado del LED

int ledPin = 2; // Pin al que está conectado el LED

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

int contador = 0; // Variable para el contador inicializada en 0

const int botonAumentarPin = 4; // Pin del botón para aumentar el contador
const int botonDisminuirPin = 5; // Pin del botón para disminuir el contador

#define DHTPIN 18
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void getLedState()
{
  HTTPClient http;
  http.begin(serverLed);

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200)
  {
    // Leer el JSON de la respuesta
    String payload = http.getString();
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, payload);

    // Obtener el estado del LED desde el JSON
    bool ledState = jsonDoc["status"];
    
    // Actualizar el estado del LED en la placa Arduino
    digitalWrite(ledPin, ledState ? HIGH : LOW);

    Serial.print("Estado del LED: ");
    Serial.println(ledState);
  }
  else
  {
    Serial.print("Error al obtener el estado del LED. Código de respuesta: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void post_data(String action, int quantity)
{
  DynamicJsonDocument json_chido(1024);
  json_chido["action"] = action;
  json_chido["quantity"] = quantity;

  String json_str;
  serializeJson(json_chido, json_str);

  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(json_str);

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

  http.end();
}

void post_temperature(float temperature)
{
  DynamicJsonDocument json_doc(1024);
  json_doc["temperature"] = temperature;

  String json_str;
  serializeJson(json_doc, json_str);

  HTTPClient http;
  http.begin(serverTemperatureURL);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(json_str);

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code (temperature): ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else
  {
    Serial.print("Error code (temperature): ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void post_humidity(float humidity)
{
  DynamicJsonDocument json_doc(1024);
  json_doc["humidity"] = humidity;

  String json_str;
  serializeJson(json_doc, json_str);

  HTTPClient http;
  http.begin(serverHumidityURL);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(json_str);

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code (humidity): ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else
  {
    Serial.print("Error code (humidity): ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void post_asc()
{
  post_data("asc", 1);
}

void post_desc()
{
  post_data("desc", 1);
}

void setup()
{
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Apagar el LED al inicio

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

  // No se necesita llamar a dht.begin(); en el caso del DHT11
  pinMode(botonAumentarPin, INPUT);
  pinMode(botonDisminuirPin, INPUT);

  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop()
{
  // Obtener el estado actual del LED desde el servidor y actualizar el LED en la placa Arduino
  getLedState();
  
  // Leer el estado de los botones
  int estadoBotonAumentar = digitalRead(botonAumentarPin);
  int estadoBotonDisminuir = digitalRead(botonDisminuirPin);

  if (estadoBotonAumentar == HIGH)
  {
    // Incrementa el contador cuando se presiona el botón de aumentar
    contador++;
    post_asc(); // Envía datos al servidor
  }
  else if (estadoBotonDisminuir == HIGH)
  {
    // Disminuye el contador cuando se presiona el botón de disminuir
    contador--;
    post_desc(); // Envía datos al servidor
  }

  // Muestra el valor actual del contador en el puerto serie
  Serial.print("Contador: ");
  Serial.println(contador);

  if ((millis() - lastTime) > timerDelay)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;

      String serverPath = serverName;

      http.begin(serverPath.c_str());

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

      http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humedad: "));
  Serial.print(h);
  Serial.print(F("% Temperatura: "));
  Serial.print(t);
  Serial.println(F("°C "));

  post_temperature(t);
  post_humidity(h);

  delay(1000); // Espera 5 segundos antes de enviar otra lectura
}