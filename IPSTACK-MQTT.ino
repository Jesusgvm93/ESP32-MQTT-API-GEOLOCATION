#include <WiFi.h>
#include <PubSubClient.h>
#include <HCSR04.h>
#include <stdlib.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "Fibertel WiFi996 2.4GHz";//put your wifi network name here
const char* password = "007138211154";//put your wifi password here

//**************************************
//*********** MQTT CONFIG **************
//**************************************

const char *mqtt_server = "**************";// You're MQTT SERVER
int mqtt_port = 1883;
const char *mqtt_user = "**************"; //You're MQTT USER
const char *mqtt_pass = "**************"; //You're MQTT PASSWORD
const char *root_topic_subscribe = "**************"; //You're MQTT TOPIC
const char *root_topic_publish = "**************"; //You're MQTT TOPIC

//**************************************
//*********** MQTT CONFIG2 *************
//**************************************

const char *mqtt_user1 = "**************"; //You're MQTT USER 2
const char *mqtt_pass1 = "**************"; //You're MQTT PASSWORD 2
const char *root_topic_subscribe1 = "**************"; //You're MQTT TOPIC
const char *root_topic_publish1 = "**************"; //You're MQTT TOPIC
//**************************************

UltraSonicDistanceSensor distanceSensor(19, 18);
int data, intentos = 0;
const char* country_name;
double distance;
double latitude;
double longitude;
char geo[40];

WiFiClient espClient;
WiFiClient espClient1;
PubSubClient client(espClient);
PubSubClient client1(espClient1);

void setup()
{
Serial.begin(9600);
setup_Wifi();
client.setServer(mqtt_server, mqtt_port);
client1.setServer(mqtt_server, mqtt_port);
client.setCallback(callback);
client1.setCallback(callback1);
ipFind();
}

void loop()
{
  if (!client.connected()) 
  
  {
    reconnect();
  }

  if (!client1.connected()) 
  
  {
    reconnect();
  }
    if (client.connected()){ 
    distance = distanceSensor.measureDistanceCm();
    char distF[7];
    dtostrf(distance, 4, 2, distF);
    Serial.println();
    delay(100);
    client.publish(root_topic_publish,distF);

    
 }
 if (client1.connected()){ 
    char latF[16];
    char lonF[16];
    dtostrf(latitude, 4, 7, latF);
    dtostrf(longitude, 4, 7, lonF);
    String str = String(lonF) + "," + String (latF);
    str.toCharArray(geo,40);
    Serial.println();
    delay(300);
    client1.publish(root_topic_publish1,geo);
    
 }
  client.loop();
  client1.loop(); 
}

void setup_Wifi()
{
WiFi.begin(ssid, password);
Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(500);
    Serial.print(".");
    if(intentos>20){
       Serial.println("It was not possible to connect to the WiFi network");
       Serial.println("Rebooting Device");
       setup();
    }
    intentos++;
  }
  Serial.print("Connected, my IP is: ");
  Serial.println(WiFi.localIP());  
}

void reconnect() {

  while (!client.connected()) 
  {
    Serial.print("Trying Mqtt connection...");
    String clientId = "iotAR_";
    clientId += String (random(0xffff), HEX);
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Connected!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Subscription ok");
      }else{
        Serial.println("Subscription failure");
      } 
    }
    else {
      Serial.print("failure :( with error -> ");
      Serial.print(client.state());
      Serial.println(" We try again in 5 seconds");
      delay(5000);
    }
  }
  while (!client1.connected())
  {
  Serial.print("Trying Mqtt2 connection...");
    String clientId1 = "Geo_";
    clientId1 += String (random(0xffff), HEX);
    // Intentamos conectar
    if (client1.connect(clientId1.c_str(),mqtt_user1,mqtt_pass1)) {
      Serial.println("Connected!");
      // Nos suscribimos
      if(client1.subscribe(root_topic_subscribe1)){
        Serial.println("Subscription 2 ok");
      }else{
        Serial.println("Subscription 2 failure");
      } 
    }
    else {
      Serial.print("failure :( with error -> ");
      Serial.print(client1.state());
      Serial.println(" We try again in 5 seconds");
      delay(5000);
    }  
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{
  String incoming = "";
  Serial.print("Message received from -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Message -> " + incoming);

}

void callback1(char* topic, byte* payload, unsigned int length)
{
  String incoming = "";
  Serial.print("Message received from -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Message -> " + incoming);

}

void ipFind()
{

  HTTPClient http;
  http.begin("*********************"); // You're IPSTACK URL
  Serial.print("[HTTPS] GET...\n");  
  int httpCode = http.GET();
  if (httpCode > 0) { //Check for the returning code
 
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);

StaticJsonDocument<128> doc;
DeserializationError error = deserializeJson(doc, payload);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

country_name = doc["country_name"]; // "Argentina"
latitude = doc["latitude"]; // -34.611778259277344
longitude = doc["longitude"]; // -58.41730880737305

Serial.println(country_name);
Serial.println(latitude,7);
Serial.println(longitude,7);    
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources

}
