#include "DHT.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "";
const char* password = "";
const char* serverName = "https://emiweatherapi.azurewebsites.net/esp32temperature";
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

#define DHTPIN 26
#define DHTTYPE DHT11
//DHTTYPE = DHT11, but there are also DHT22 and 21

DHT dht(DHTPIN, DHTTYPE); // constructor to declare our sensor

void setup() {
  Serial.begin(115200);
  dht.begin();

  //wifi
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  delay(2000);delay(2000);
  // The DHT11 returns at most one measurement every 1s
  float h = dht.readHumidity();
  //Read the moisture content in %.
  float t = dht.readTemperature();
  //Read the temperature in degrees Celsius
  float f = dht.readTemperature(true);
  // true returns the temperature in Fahrenheit

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed reception");
    // SerialBT.println("Failed");
    return;
    //Returns an error if the ESP32 does not receive any measurements
  }


  Serial.print("Humidite: ");
  Serial.print(h);
  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.print("°C, ");
  Serial.print(f);
  Serial.println("°F");
  
  //Send an HTTP POST request every minute
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClientSecure *client = new WiFiClientSecure;
      HTTPClient http;

      if(client)
        client->setInsecure();
        
      if(http.begin(*client, serverName)){
              http.addHeader("Content-Type", "application/json");
              String one = "{\"Esp32ID\": 0,\"Temperature\":";
              one.concat(t);one.concat(",\"Humidity\":");one.concat(h);one.concat("}");

              String httpRequestData = one;
      // Send HTTP POST request
      Serial.print(httpRequestData);
      int httpResponseCode = http.POST(httpRequestData);
      Serial.print(httpResponseCode);
      }
      
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }

  //  SerialBT.print("Humidite: ");
  // SerialBT.print(h);
  // SerialBT.print("%  Temperature: ");
  // SerialBT.print(t);
  // SerialBT.print("°C, ");
  // SerialBT.print(f);
  // SerialBT.println("°F");
  // Transmits the measurements received in the serial monitor
}
