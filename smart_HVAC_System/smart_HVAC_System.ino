#include <SoftwareSerial.h>
#include "DHT.h"

#define ESP_RX 19  
#define ESP_TX 18 
SoftwareSerial espSerial(ESP_RX, ESP_TX);

#define ESP8266 Serial1
String SSID = "amazing";
String password = "yesIdidit789";
String API = "G8QO3N4MM64OHNPI";
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
String field4 = "field4";

#define DHT_PIN 53
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);
const int photoRezistorPin = A2; 
const int CO2SensorPin = A1;
const int PIRPin = 51;

float humidity;
float temperature;
int photoRezistor;
const float GAMMA = 0.7;
const float RL10 = 50;
int CO2Sensor;
int PIR = 0;

void setup() {
  Serial.begin(115200);
  ESP8266.begin(115200);
  delay(1000);
  sendCommand("AT");
  sendCommand("AT+CWJAP=\"" + SSID + "\",\"" + password + "\"");
  dht.begin();
  pinMode(CO2SensorPin, INPUT);
  pinMode(PIRPin, INPUT_PULLUP );
}

void sendCommand(String command) {
  ESP8266.println(command);
  delay(1000);
  while (ESP8266.available()) {
    Serial.write(ESP8266.read());
  }
  delay(5000);
}


void loop() {
  temperature = dht.readTemperature();
  Serial.print(" Temp: ");
  Serial.print(temperature);
  Serial.println(" Celsius");

  photoRezistor = analogRead(photoRezistorPin);
  float voltage = photoRezistor / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));


  CO2Sensor = analogRead(CO2SensorPin);
  Serial.println("AirQua=");
  Serial.print(CO2Sensor, DEC);
  Serial.print(" PPM");

  PIR = digitalRead(PIRPin);

  String data = field1 + "=" + String(temperature) + "&" +field2+ "="+ String(lux)+ "&" +field3+ "="+ String(PIR)+ "&" +field4+ "="+ String(CO2Sensor);

  String command = "AT+CIPSTART=\"TCP\",\"" + HOST + "\",80";
  sendCommand(command);

  String httpRequest = "GET /update?api_key=" + API + "&" + data + " HTTP/1.1\r\nHost: " + HOST + "\r\nConnection: close\r\n\r\n";
  String cipSendCommand = "AT+CIPSEND=" + String(httpRequest.length() + 2);
  sendCommand(cipSendCommand);

  ESP8266.println(httpRequest);
  delay(2000);
  sendCommand("AT+CIPCLOSE");
  delay(15000);
}
