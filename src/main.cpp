#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <secrets.h> // WiFi SSID and Password and API secrets

#define FANA 14
#define FANB 12
#define DHTTYPE DHT22
#define DHTPIN 4

WiFiClient espClient;

char buffer[80];
int  temperatureInt;
char temperatureStr[10];

DHT dht(DHTPIN, DHTTYPE);
float temperatureC;


void setup() {
  Serial.begin(9600);
  
  delay(10);

  // Connecting to WiFi
  WiFi.begin(ssid, wiFiPassword);
  Serial.print("Connection à ");
  Serial.print(ssid);
  Serial.println("...");

  int i = 0;
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(++i);
    Serial.print(' ');
  }

  Serial.println("Connection établie");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
  
  // Initializing DHT (temperature and humidity sensor)
  dht.begin();
  pinMode(FANA, OUTPUT);
}


void loop() {

  if(WiFi.status() == WL_CONNECTED) {
    temperatureC = dht.readTemperature();
    if(isnan(temperatureC)) {
      Serial.println("Failed to read temperature");
      return;
    }

    temperatureInt = (int)temperatureC; // Convert float to int
    sprintf(buffer,"%s%d", api, temperatureInt); // Concatenate the API endpoint and the new temperature

    HTTPClient http;
    http.begin(espClient, buffer);
    int httpCode = http.GET();
    if(httpCode > 0) {
      StaticJsonDocument<512> doc;

      DeserializationError error = deserializeJson(doc, http.getString());
      if(error) {
        Serial.print("deserializeJson(à failed!");
        Serial.println(error.c_str());
        return;
      }

      const char* current_temperature = doc["current_temperature"];
      const char* fan_a_rule = doc["fan_a_rule"];
      const char* fan_b_rule = doc["fan_b_rule"];
      const char* force_fan_a = doc["force_fan_a"];
      const char* force_fan_b = doc["force_fan_b"];

      Serial.print("Current temperature : ");
      Serial.println(current_temperature);

      Serial.print("fan a rule : ");
      Serial.println(fan_a_rule);

      Serial.print("fan b rule :");
      Serial.println(fan_b_rule);

      Serial.print("force_fan_a :");
      Serial.println(force_fan_a);

      Serial.print("force_fan_b :");
      Serial.println(force_fan_b);
    }
  }

  delay(2000);

}