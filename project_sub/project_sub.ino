#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "OLED.h"

#define CDS 0
#define TEMP 1
#define HUM 2
#define PLUS 0
#define MOVE 1
#define MINUS 2
#define REDLED 0
#define GREENLED 1
#define BLUELED 2

OLED display(4, 5);
char strdata[100];

const char* ssid = "U+Net9BE3"; // 와이파이 이름
const char* password = "Gusals1466@"; // 와이파이 비밀번호

#define mqtt_server "tailor.cloudmqtt.com" // MQTT server 주소
#define mqtt_port 16421 // port 번호
#define mqtt_user "acbushzy" // username
#define mqtt_password "Baon8eKj8-jw" // password
#define mqtt_
char mqtt_msg[20];

// pin setting
int chatering[3] = {0, 0, 0};
int pinLED[3] = {14, 4, 5};
int Switch[3] = {0, 12, 13};
int colorLED[3] = {11, 16, 15};
int pinbuttonState = HIGH;
WiFiClient espClient;
PubSubClient client(espClient);

// value
int buttonState = 0;
int state[3] = {0, 0, 0};

void setup() {
  Serial.begin(115200);

  display.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  for(int i = 0; i < 3; i++)
  {
    pinMode(pinLED[i], OUTPUT);
    digitalWrite(pinLED[i], pinbuttonState);
    //pinMode(colorLED[i], OUTPUT);
    //digitalWrite(colorLED[i], 0);
    pinMode(Switch[i], INPUT_PULLUP);
  }
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

int timeSinceLastRead = 0;
void loop() {
  // mqtt server connect
  if (!client.connected()) {
    client.connect("ESP8266sub", mqtt_user, mqtt_password);
    client.subscribe("State");
    Serial.println("Connect MQTT Server!!");
  }
  
  if(timeSinceLastRead < 500)
  {
    ledOn();
  }
  else if(timeSinceLastRead < 1000)
  {
    for(int i = 0; i < 3; i++)
    {
      if(state[i] == 0)
      {
        digitalWrite(pinLED[i], HIGH);
      }
    }
  }
  else
  {
    timeSinceLastRead = 0;
  }

  for(int i = 0; i < 3; i++)
    {
      Serial.print(state[i]);
      if(state[i] == 1)
      {
        digitalWrite(pinLED[i], LOW);
      }
    }
    
  buttonEvent();
  delay(100);
  timeSinceLastRead += 100;
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String Msg = "";
  int i=0;
  while (i<length) Msg += (char)payload[i++];
  
  if (Msg == "dark"){
    state[CDS] += 1;
    //digitalWrite(pinLED[CDS], LOW);
  }
  if (Msg == "cold"){
    state[TEMP] += 1;
    //digitalWrite(pinLED[TEMP], LOW);
  }
  if (Msg == "dry"){
    state[HUM] += 1;
    //digitalWrite(pinLED[HUM], LOW);
  }
  if (Msg == "notdark"){
    state[CDS] = 0;
    //digitalWrite(pinLED[CDS], HIGH);
  }
  if (Msg == "notcold"){
    state[TEMP] = 0;
    //digitalWrite(pinLED[TEMP], HIGH);
  }
  if (Msg == "notdry"){
    state[HUM] = 0;
    //digitalWrite(pinLED[HUM], HIGH);
  }
}

//buttonEvent
void buttonEvent()
{
  // MOVE 눌릴때
  if(digitalRead(Switch[MOVE]) == 0) {
    if(chatering[MOVE] == 0) {
      chatering[MOVE] = 1;
      buttonState++;
      if(buttonState > 2) buttonState = CDS;
    }
  // 안눌릴때
  } else {
    if(chatering[MOVE] == 1) {
      chatering[MOVE] = 0;
    }
  }
  
  // PLUS 눌릴때
  if(digitalRead(Switch[PLUS]) == 0) {
    if(chatering[PLUS] == 0) {
      chatering[PLUS] = 1;
      switch(buttonState)
      {
        case CDS: client.publish("Action", "Sunlight+"); break;
        case TEMP: client.publish("Action", "Heating+"); break;
        case HUM: client.publish("Action", "Water+"); break;
      }
    }
  // 안눌릴때
  } else {
    if(chatering[PLUS] == 1) {
      chatering[PLUS] = 0;
    }
  }
  // MINUS 눌릴때
  if(digitalRead(Switch[MINUS]) == 0) {
    if(chatering[MINUS] == 0) {
      chatering[MINUS] = 1;
      switch(buttonState)
      {
        case CDS: client.publish("Action", "Sunlight-"); break;
        case TEMP: client.publish("Action", "Heating-"); break;
        case HUM: client.publish("Action", "Water-"); break;
      }
    }
  // 안눌릴때
  } else {
    if(chatering[MINUS] == 1) {
      chatering[MINUS] = 0;
    }
  }
}

// SELECT LED 
void ledOn()
{
  switch(buttonState)
  {
    case CDS:
      digitalWrite(pinLED[CDS], LOW);
      break;
    case TEMP:
      digitalWrite(pinLED[TEMP], LOW);
      break;
    case HUM:
      digitalWrite(pinLED[HUM], LOW);
      break;
  }
}
// SELECT LED 
void ledOff()
{
  switch(buttonState)
  {
    case CDS:
      digitalWrite(pinLED[CDS], HIGH);
      break;
    case TEMP:
      digitalWrite(pinLED[TEMP], HIGH);
      break;
    case HUM:
      digitalWrite(pinLED[HUM], HIGH);
      break;
  }
}
