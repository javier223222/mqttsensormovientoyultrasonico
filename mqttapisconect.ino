#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
 
const char* ssid = "Redmi A2";
const char* password = "taspendejo";
const char* mqttServer = "52.5.255.119";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
const char* subscribeTopic = "safeplace/3300/update";
char* topicproxi="safeplace/3300/ultrasonic";
char* topicmov="safeplace/3300/movimiento";
int DISTANCIA = 0;
int pinLed=2;
int pinEco=12;
int pinGatillo=13;
int pir = 26;
int pirdato;
int PRAMETRODISTANCIA=20;
boolean HIGHEL = 0;
long count=0;
WiFiClient espClient;
PubSubClient client(espClient);

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  //Iniciamos el pin del emisor de reuido en salida
  pinMode(triggerPin, OUTPUT);
  //Apagamos el emisor de sonido
  digitalWrite(triggerPin, LOW);
  //Retrasamos la emision de sonido por 2 milesismas de segundo
  delayMicroseconds(2);
  // Comenzamos a emitir sonido
  digitalWrite(triggerPin, HIGH);
  //Retrasamos la emision de sonido por 2 milesismas de segundo
  delayMicroseconds(10);
  //Apagamos el emisor de sonido
  digitalWrite(triggerPin, LOW);
  //Comenzamos a escuchar el sonido
  pinMode(echoPin, INPUT);
  // Calculamos el tiempo que tardo en regresar el sonido
  return pulseIn(echoPin, HIGH);
}

 
void setup() {
 
  Serial.begin(115200);
  pinMode(pir, INPUT);
  pinMode(pinLed, OUTPUT);
  Serial.println();
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
  
 
  client.setServer(mqttServer, mqttPort);
 // Set the callback functio
  client.setCallback(callback);
  client.subscribe(subscribeTopic);

 
}
void reconnect() {

 while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP32ClientO9")) {
     
      client.subscribe(subscribeTopic);
      Serial.println("connected");

 
    } else {
 
      Serial.print("failed with state fff");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(payload, length);

    if (root.success()) {
        // Access JSON data
        int value = root["parameterprox"]; // Replace with your actual key
        PRAMETRODISTANCIA=value;
        
    } else {
        Serial.println("Error parsing JSON");
    }
}


 
void loop() {
  if (!client.connected()) {
    
		reconnect();
    
    
	}

  if (client.connected()){
   
   DISTANCIA = 0.01723 * readUltrasonicDistance(pinGatillo, pinEco);
   
    pirdato = digitalRead(pir);
  if (DISTANCIA < PRAMETRODISTANCIA &&pirdato == HIGH) {
     Serial.println(PRAMETRODISTANCIA);
     sendMessage(DISTANCIA,"PROXIMIDAD",topicproxi,1);
     sendMessage(pirdato,"MOVIMIENTO",topicmov,1);
     sendMessagetwo(1);
  }else{
     sendMessage(DISTANCIA,"PROXIMIDAD",topicproxi,0);
     sendMessage(pirdato,"MOVIMIENTO",topicmov,0);
     sendMessagetwo(0);
  } 
   delay(1000);
  }
  client.loop();
  
  // // Serial.println(PRAMETRODISTANCIA);


 
 
  
 
  
  
 
  // delay(300);

  // if (!client.connected()) {
  //    Serial.println("dddd");

  //  }   
  // client.loop();
 
}



void sendMessage(int dat,char* sensor,char* topic,boolean dang){

  if(sensor=="PROXIMIDAD"){
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

 
  JSONencoder["device"] = "ESP32";
  JSONencoder["sensorType"] = sensor;
  JSONencoder["dang"] = dang;

  JsonArray& values = JSONencoder.createNestedArray("values");
  JsonArray& parameterProx = JSONencoder.createNestedArray("parameterProx");
 
  values.add(dat);
  parameterProx.add(PRAMETRODISTANCIA);
 
  char JSONmessageBuffer[100];

  
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  
 
  client.publish(topic, JSONmessageBuffer) ;
  }else{
     StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

 
  JSONencoder["device"] = "ESP32";
  JSONencoder["sensorType"] = sensor;
  JSONencoder["dang"] = dang;
  JsonArray& values = JSONencoder.createNestedArray("values");
  JsonArray& isHigh = JSONencoder.createNestedArray("isHigh");
 
  values.add(dat);
  isHigh.add(HIGHEL);
 
  char JSONmessageBuffer[100];

  
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
 
 
  client.publish(topic, JSONmessageBuffer) ;
  }

  

  
  
}
void sendMessagetwo(int dato){
   StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

 
  
  JSONencoder["dangerous"] = dato;
   char JSONmessageBuffer[100];

  
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  
 
  client.publish("safeplace/3300/dangerous", JSONmessageBuffer) ;


}





