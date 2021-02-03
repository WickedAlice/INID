#include <ESP8266WiFi.h>
#include "Adafruit_Sensor.h" 
#include "DHT.h"
 
const char* ssid = "***";     // Your WiFi network
const char* password = "***"; // Your WiFi password


#define RELAY 2                               // Пин к которому подключен датчик
#define DHTPIN 2                                        // Пин к которому подключен датчик
#define DHTTYPE DHT11                                   // Используемый датчик DHT 11
DHT dht(DHTPIN, DHTTYPE);                               // Инициализируем датчик
WiFiServer server(80); // Указываем порт Web-сервера
 int Hum_norm = 50;
 int t_norm = 25;
 int value = LOW;
 
void setup(){
  delay(2200);      
  Serial.begin(115200);                       // Скорость передачи 115200 
  pinMode(RELAY,OUTPUT);                      // Указываем вывод RELAY как выход
  digitalWrite(RELAY, value);                  // Устанавливаем RELAY в HIGH
  delay(3000);
  value = HIGH;
  digitalWrite(RELAY, value); 
  delay(8000);
  value = LOW;
  digitalWrite(RELAY, value); 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  dht.begin();
  WiFi.begin(ssid, password);                 // Подключение к WiFi Сети
 
  while (WiFi.status() != WL_CONNECTED)       // Проверка подключения к WiFi сети
  { 
    delay(500);                               // Пауза 500 мкс
    Serial.print("."); 
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  server.begin();                             // Запуск сервера
  Serial.println("Server started");
  Serial.print("Use this URL to connect: ");
  Serial.print(WiFi.localIP());               // Печать выданого IP адресса          
}
 
void loop(){
  WiFiClient client = server.available();    // Получаем данные, посылаемые клиентом 
  if (!client)                                
  {
    return;
  }
  Serial.println("new client");               // Отправка "new client"
  while(!client.available())                  // Пока есть соединение с клиентом 
  {
    delay(1);                                 // пауза 1 мс
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
 /*
  if (request.indexOf("/RELAY=ON") != -1)  
  {
    Serial.println("RELAY=ON");
    digitalWrite(RELAY,HIGH);
    value = HIGH;
  }
  if (request.indexOf("/RELAY=OFF") != -1)  
  {
    Serial.println("RELAY=OFF");
    digitalWrite(RELAY,LOW);
    value = LOW;
  }
*/
  float t = dht.readTemperature();                  // Запрос на считывание температуры
  float h = dht.readHumidity();
  
  if (t <= t_norm) {
    digitalWrite(RELAY,LOW);
    value = LOW; 
  }
  if (t >= t_norm + 5) {
       digitalWrite(RELAY,HIGH);
      value = HIGH;
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>DHT RELAY Control</title></head>");
  client.print("Relay is now: ");
 
  if(value == HIGH) 
  {
    client.print("OFF");
  } 
  else 
  {
    client.print("ON");
  }
  /*
  client.println("<br><br>");
  client.println("Turn <a href=\"/RELAY=OFF\">OFF</a> RELAY<br>");
  client.println("Turn <a href=\"/RELAY=ON\">ON</a> RELAY<br>");
  */
  client.println("<h3>Temperature = ");
  client.println(t);                               // Отображение температуры
  client.println("<h3>Humidity = ");
  client.println(h);                               // Отображение влажности
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
