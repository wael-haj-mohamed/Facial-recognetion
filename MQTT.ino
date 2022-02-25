#include <ESP8266WiFi.h>         
 
//library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         
#include <DHT.h>
#include <PubSubClient.h>
 
#define DHTPIN 5 
#define LED D2

#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
 
unsigned long previousMillis = 0;
unsigned long interval = 10000;
 
const char* mqttServer = "192.168.1.114";
const int mqttPort = 1883;
 
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
    Serial.println();
   
  String message = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message+=(char)payload[i];
  }
  Serial.println("-----------------------");
  if(String(topic)=="LED"){
    if(message=="LED ON"){
      digitalWrite(LED,HIGH);
      Serial.println("LED IS ON");
    }
    else{
      digitalWrite(LED,LOW);
    }
  }
     
}
 
WiFiClient espClient;
PubSubClient client(mqttServer, mqttPort, callback, espClient);
 
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);

 
  WiFiManager wifiManager;

  wifiManager.autoConnect("AutoConnectAP");

  Serial.println("connected...EZ :)");
  dht.begin();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
    while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client" )) {
      client.subscribe("LED");
      Serial.println("connected");
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
}
}
 
void loop() {
    client.loop();
unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if(!isnan(h)&&!isnan(t)){
      Serial.println("Temperature : " +String(t));
      Serial.println("Humidity : " +String(h));
      String toSend = String(t) + "," +String(h);
      client.publish("data",toSend.c_str());
    }
 
  }
}
