#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ESP8266WebServer.h>

#include "index.h"
//#include "DHT.h"

//#define DHTPIN 14     // what digital pin the DHT22 is conected to
//#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

//DHT dht(DHTPIN, DHTTYPE);

#define CDS 0
#define TEMP 1
#define HUM 2

// sersor value
float cds_value;            // Cds value
float temp_value = 24;      // Temperature value
float hum_value = 50;       // Humidity value
int cor[3] = {0, 0, 0};
int limit[3] = {200, 0, 30};
int automode = 0;
// pin value
int chatering = 0;
int pinLED = 2;
int pinSwitch = 0;
int pinState = HIGH;

const char* ssid = "U+Net9BE3"; // 와이파이 이름
const char* password = "Gusals1466@"; // 와이파이 비밀번호
// mqtt setting
#define mqtt_server "tailor.cloudmqtt.com" // MQTT server 주소
#define mqtt_port 16421 // port 번호
#define mqtt_user "acbushzy" // username
#define mqtt_password "Baon8eKj8-jw" // password
#define mqtt_
char mqtt_msg[20];
WiFiClient espClient;
PubSubClient client(espClient);

//=======================================
//       webserver setting
//=======================================
ESP8266WebServer server(80);
void handleRoot()
{
  String s = MAIN_page;
  server.send(200, "text/html", s);
}

void getCDS() {
 int a = cds_value;
 String adcValue = String(a);
 
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}
void getTEMP() {
 int a = temp_value;
 String adcValue = String(a);
 
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}
void getHUM() {
 int a = hum_value;
 String adcValue = String(a);
 
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}
void handleCDS() {
  String limitCds = server.arg("CDSinput");
  limit[CDS] = limitCds.toInt();
  Serial.print("limit cds : ");
  Serial.println(limit[CDS]);
  
  server.send(200, "text/plane", limitCds);
}
void handleTEMP() {
  String limitTemp = server.arg("TEMPinput");
  limit[TEMP] = limitTemp.toInt();
  Serial.print("limit temp : ");
  Serial.println(limit[TEMP]);
  
  server.send(200, "text/plane", limitTemp);
}
void handleHUM() {
  String limitHum = server.arg("HUMinput");
  limit[HUM] = limitHum.toInt();
  Serial.print("limit hum : ");
  Serial.println(limit[HUM]);
  
  server.send(200, "text/plane", limitHum);
}
void handleAUTO() {
  automode = automode?0:1;
}
void getAUTO() {
  String autoState = automode?"ON":"OFF";
  
  server.send(200, "text/plane", autoState);
}

//=======================================
//               SETUP
//=======================================
void setup() {
  Serial.begin(115200);
  // connect wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  // pin init
  pinMode(pinLED, OUTPUT);
  pinMode(pinSwitch, INPUT_PULLUP);
  pinState = digitalRead(pinSwitch);
  digitalWrite(pinLED, pinState);
  // mqtt init
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  // server init
  server.on("/", handleRoot);
  server.on("/setCDS", handleCDS);
  server.on("/setTEMP", handleTEMP);
  server.on("/setHUM", handleHUM);
  server.on("/readCDS", getCDS);
  server.on("/readTEMP", getTEMP);
  server.on("/readHUM", getHUM);
  server.on("/setAUTO", handleAUTO);
  server.on("/getAUTO", getAUTO);
  
  server.begin();
  Serial.println("HTTP server started");
}

int timeSinceLastRead = 0;
//=======================================
//                 LOOP
//=======================================
void loop() {
  // mqtt connect
  if (!client.connected()) {
    client.connect("ESP8266pub", mqtt_user, mqtt_password);
    client.subscribe("Action");
    Serial.println("Connect MQTT Server!!");
  }
  // server

  if(timeSinceLastRead > 5000)
  {
    // read sensor
    cds_value = analogRead(0);
    //temp_value = dht.readTemperature();
    //hum_value = dht.readHumidity();
    temp_value = 24;
    hum_value = 50;
  
    // correct data
    cds_value += cor[CDS];
    temp_value += cor[TEMP];
    hum_value += cor[HUM];
  
    serialPrintSensor(cds_value, temp_value, hum_value);
    publishData(cds_value, temp_value, hum_value);
    publishState(cds_value, temp_value, hum_value);
    if(automode == 1)
    {
      autoProc(cds_value, temp_value, hum_value);
    }
  
    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;
  client.loop();

  /* WEB */
  server.handleClient();
}
//=======================================
//             CALLBACK FUNC
//=======================================
void callback(char* topic, byte* payload, unsigned int length) {
  String Msg = "";
  int i=0;
  while (i<length) Msg += (char)payload[i++];
  
  if (Msg == "Sunlight+") cor[CDS] += 100;
  if (Msg == "Heating+") cor[TEMP] += 5;
  if (Msg == "Water+") cor[HUM] += 5;
  if (Msg == "Sunlight-") cor[CDS] -= 100;
  if (Msg == "Heating-") cor[TEMP] -= 5;
  if (Msg == "Water-") cor[HUM] -= 5;
}

//=======================================
//              FUNCTION
//=======================================

// serial print data
void serialPrintSensor(int c, int t, int h)
{
  Serial.print("조도 값 : ");
  Serial.println(c);
  Serial.print("온도 값 : ");
  Serial.println(t);
  Serial.print("습도 값 : ");
  Serial.println(h);
}

// state publish
void publishState(int c, int t, int h)
{
  if(c < 200) client.publish("State", "dark");
  else client.publish("State", "notdark");
  if(t < 0) client.publish("State", "cold");
  else client.publish("State", "notcold");
  if(h < 30) client.publish("State", "dry");
  else client.publish("State", "notdry");
}

// data publish
void publishData(int c, int t, int h)
{
  char text[20];
  
  dtostrf(c, 3, 0, text);
  client.publish("Data/Cds", text);
  dtostrf(t, 3, 0, text);
  client.publish("Data/Temperature", text);
  dtostrf(h, 3, 0, text);
  client.publish("Data/Humidity", text);
}

void autoProc(int c, int t, int h)
{
  if(c < limit[CDS]) client.publish("Action", "Sunlight+");
  if(t < limit[TEMP]) client.publish("Action", "Heating+");
  if(h < limit[HUM]) client.publish("Action", "Water+");
}
