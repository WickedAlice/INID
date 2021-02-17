#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

unsigned int Rs = 150000;
double Vcc = 5; //3.3;
double tanalog;

int AnalogRead() {
  int val = 0;
  for(int i = 0; i < 20; i++) {
	val += analogRead(A0);
	delay(1);
  }

  val = val / 20;
  return val;
}

  double Thermister(int val) {
  double V_NTC = (double)val / 1024;
  double R_NTC = (Rs * V_NTC) / (Vcc - V_NTC);
  R_NTC = log(R_NTC);
  double Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * R_NTC * R_NTC ))* R_NTC );
  Temp = Temp - 305; //273.15;    	 
  return Temp;
}

ESP8266WebServer server(80);
#define ssid "****"
#define password "****"
#define led D3
#define ext_led LED_BUILTIN
#define buttonPin D5

String msg1, msg2, buttonState = "Off", sensorValue = "***";

void handleRoot() {
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  char html[1000];
  snprintf ( html, 1000,

" <!DOCTYPE html> \
<html>\
  <head>\
	<meta http-equiv='refresh' content='3'/>\
	<title>ESP8266 WiFi Network</title>\
	<style>\
  	body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; font-size: 1.5em; Color: #000000; }\
  	h1 { Color: #AA0000; }\
	</style>\
  </head>\
  <body>\
	<h1>Temperature</h1>\
	<p>Uptime: %02d:%02d:%02d</p>\
	<p>Temperature 1: %f%</p>\
	<p>This page refreshes every 3 seconds. Click <a href=\"javascript:window.location.reload();\">here</a> to refresh the page now.</p>\
  </body>\
</html>",
	hr, min % 60, sec % 60,
	tanalog
  );

  sprintf(html,"T = %f", tanalog);
//  client.println("HTTP/1.1 200 OK");
//  client.println("Content-Type: text/html");
//  client.println("");
//  client.println("<!DOCTYPE HTML>");
  server.send(200, "text/plain", html);

}

void handleNotFound(){
digitalWrite(led, 1);
digitalWrite(ext_led, 1);
String message = "File Not Found\n\n";
message += "URI: ";
message += server.uri();
message += "\nMethod: ";
message += (server.method() == HTTP_GET)?"GET":"POST";
message += "\nArguments: ";
message += server.args();
message += "\n";
for (uint8_t i=0; i<server.args(); i++){
message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
}
server.send(404, "text/plain", message);
digitalWrite(led, LOW);
digitalWrite(ext_led, LOW);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
	delay(500);
	Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  setup1();
}

void hOn() {
	digitalWrite(led, LOW);
	digitalWrite(ext_led, HIGH);
	msg1 += "LED ON; button status: ";
	msg1 += buttonState;
	msg1 += " sensorValue: ";
	msg1 += sensorValue;
	msg1 += "\n";
	server.send(200, "text/plain", msg1);
}

void hOff() {
	digitalWrite(led, HIGH);
	digitalWrite(ext_led, LOW);
	msg2 += "LED OFF; button status: ";
	msg2 += buttonState;
	msg2 += " sensorValue: ";
	msg2 += sensorValue;
	msg2 += "\n";
	server.send(200, "text/plain", msg2);
}

void setup1(void)
{
pinMode(led, OUTPUT);
pinMode(ext_led, OUTPUT);
pinMode(buttonPin, INPUT); //connect to ESP8266 GPIO12
digitalWrite(led, 0);
digitalWrite(ext_led, 0);

server.on("/", handleRoot);
server.on("/on", hOn);
server.on("/off", hOff);
server.onNotFound(handleNotFound);

server.begin();
Serial.println("HTTP server started");

//delay a moment,
//for terminal to receive inf, such as IP address
delay(1000);
// Serial.end();
pinMode(led, OUTPUT); //BUILTIN_LED
pinMode(ext_led, OUTPUT);
digitalWrite(led, 0);
}

void loop2()
{
  tanalog = Thermister(AnalogRead());
  delay(1000);	 
  Serial.print("T = ");
  Serial.print(tanalog);
  Serial.print(" C");
  Serial.println();
}

void loop(){
loop1(); // e.g. allocate 5000ms before it runs loop2
loop2();
//loop3();
}

void loop1(void){
  server.handleClient();
}
