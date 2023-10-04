#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "HAZELPC";
const char* password = "123456789h";
const char *mqtt_server = "eee4b1060c10429bac862f1363d1e087.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char *mqtt_user = "JeTsOnBoT";
const char *mqtt_pass = "t2maV!2HzMA5BPd";
Servo miServo;//12
const byte motorIzqA=5;
const byte motorIzqB=4;
const byte motorDerA=0;
const byte motorDerB=2;
const byte enableIzq=16;
const byte enableDer=14;
const byte trigg=13;
const byte echo=15;

byte velocidadInicial=0;
void controUp(byte);
void controDown(byte);
void controRight(byte);
void controLeft(byte);
void controStop(byte);
void servoWork();
int mideDistancia();
void gatilla();
WiFiClientSecure espClient;
PubSubClient client(espClient);
long lastMsg =0;
char msg[50];
void callback(char* topic, byte* payload, unsigned int leng){
  // if en donde comparemos si payload es 1/2/3/4
  //vamos a cambiar las funciones controDer/controIzq por controDelante controDetras y vuelta
  payload[leng]='\0';
  char * control = (char *)payload;
  int i=0;
  i=atoi(control);
  if (i==0)
  {
    controStop(0);
  }
  if (i==1)
  {
    controUp(200);
  }
  if (i==2)
  {
    controRight(200);
  }
  if (i==3)
  {
    controLeft(200);
  }
  if (i==4)
  {
    controDown(200);
  }
}

void reconnect(){
  while(!client.connected())
  {
    Serial.println("Intentando Conexion MQTT");

    String clientId = "iot_1_";
    clientId = clientId+String(random(0xffff), HEX);

    if(client.connect(clientId.c_str(),mqtt_user,mqtt_pass)){
      Serial.println("Conexion a MQTT exitosa");
      client.publish("salida","primer mensaje");
      client.subscribe("entrada");
    }else{
      Serial.println("Fallo la conexion");
      Serial.println(client.state());
      Serial.println("Se intentara de nuevo en 3 segundos");
      delay(3000);
    }

  }
}

void setupWifi(){
  delay(10);
  Serial.println(" ");
  Serial.print("Conectando a ->");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("Conexion Exitosa");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  setupWifi();
  espClient.setInsecure();
  client.setServer(mqtt_server,mqtt_port);
  client.setCallback(callback);
  miServo.attach(12);

  pinMode(motorIzqA,OUTPUT);
  pinMode(motorIzqB,OUTPUT);
  pinMode(motorDerA,OUTPUT);
  pinMode(motorDerB,OUTPUT);
  analogWrite(enableIzq,velocidadInicial);
  analogWrite(enableDer,velocidadInicial);

  pinMode(trigg,OUTPUT);
  pinMode(echo,INPUT);
  digitalWrite(trigg,LOW);
}

void loop() {
  if (client.connected()==false)
  {
    reconnect();
  }
  client.loop();
  servoWork();
}
void controDown(byte vel) {
  digitalWrite(motorIzqA,LOW);
  digitalWrite(motorIzqB,HIGH);
  digitalWrite(motorDerA,LOW);
  digitalWrite(motorDerB,HIGH);
  analogWrite(enableIzq,vel);
  analogWrite(enableDer,vel);
  delay(1000);
  analogWrite(enableIzq,0);
  analogWrite(enableDer,0);  
}

void controUp(byte vel) {
  digitalWrite(motorIzqA,HIGH);
  digitalWrite(motorIzqB,LOW);
  digitalWrite(motorDerA,HIGH);
  digitalWrite(motorDerB,LOW);
  analogWrite(enableIzq,vel);
  analogWrite(enableDer,vel);
  delay(1000);
  analogWrite(enableIzq,0);
  analogWrite(enableDer,0);  
}

void controRight(byte vel) {
  digitalWrite(motorIzqA,HIGH);
  digitalWrite(motorIzqB,LOW);
  digitalWrite(motorDerA,LOW);
  digitalWrite(motorDerB,HIGH);
  analogWrite(enableIzq,vel);
  analogWrite(enableDer,vel);
  delay(750);
  analogWrite(enableIzq,0);
  analogWrite(enableDer,0);  
}

void controLeft(byte vel) {
  digitalWrite(motorIzqA,LOW);
  digitalWrite(motorIzqB,HIGH);
  digitalWrite(motorDerA,HIGH);
  digitalWrite(motorDerB,LOW);
   analogWrite(enableIzq,vel);
  analogWrite(enableDer,vel);
  delay(750);
  analogWrite(enableIzq,0);
  analogWrite(enableDer,0);  
}

void controStop(byte vel) {
  digitalWrite(motorIzqA,LOW);
  digitalWrite(motorIzqB,LOW);
  digitalWrite(motorDerA,LOW);
  digitalWrite(motorDerB,LOW);
  analogWrite(enableIzq,vel);
  analogWrite(enableDer,vel);
  delay(1000);
}

void servoWork(){
  for (int i = 160; i > 125; i--)
  {
    miServo.write(i);
    if (millis()-lastMsg>2000)
    {
      lastMsg=millis();
      String message = "A->" + String(mideDistancia());
      message.toCharArray(msg,50);
      client.publish("salida",msg);
      Serial.println("Mensaje Enviado-> "+String(mideDistancia()));
    }
  }
  for (int i = 125; i > 85; i--)
  {
    miServo.write(i);
    if (millis()-lastMsg>2000)
    {
      lastMsg=millis();
      String message = "B->" + String(mideDistancia());
      message.toCharArray(msg,50);
      client.publish("salida",msg);
      Serial.println("Mensaje Enviado-> "+String(mideDistancia()));
    }
  }
  for (int i = 85; i > 45; i--)
  {
    miServo.write(i);
    if (millis()-lastMsg>2000)
    {
      lastMsg=millis();
      String message = "C->" + String(mideDistancia());
      message.toCharArray(msg,50);
      client.publish("salida",msg);
      Serial.println("Mensaje Enviado-> "+String(mideDistancia()));
    }
  }
  for (int i = 45; i > 10; i--)
  {
    miServo.write(i);
    if (millis()-lastMsg>2000)
    {
      lastMsg=millis();
      String message = "D->" + String(mideDistancia());
      message.toCharArray(msg,50);
      client.publish("salida",msg);
      Serial.println("Mensaje Enviado-> "+String(mideDistancia()));
    }
  }
  delay(500);
  for (int i = 10; i < 45; i++)
  {
    miServo.write(i);
    if (millis()-lastMsg>2000)
    {
      lastMsg=millis();
      String message = "D->" + String(mideDistancia());
      message.toCharArray(msg,50);
      client.publish("salida",msg);
      Serial.println("Mensaje Enviado-> "+String(mideDistancia()));
    }
  }
  for (int i = 45; i < 85; i++)
  {
    miServo.write(i);
    if (millis()-lastMsg>2000)
    {
      lastMsg=millis();
      String message = "C->" + String(mideDistancia());
      message.toCharArray(msg,50);
      client.publish("salida",msg);
      Serial.println("Mensaje Enviado-> "+String(mideDistancia()));
    }
  }
  for (int i = 85; i < 125; i++)
  {
    miServo.write(i);
    if (millis()-lastMsg>2000)
    {
      lastMsg=millis();
      String message = "B->" + String(mideDistancia());
      message.toCharArray(msg,50);
      client.publish("salida",msg);
      Serial.println("Mensaje Enviado-> "+String(mideDistancia()));
    }
  }
  for (int i = 125; i < 160; i++)
  {
    miServo.write(i);
    if (millis()-lastMsg>2000)
    {
      lastMsg=millis();
      String message = "A->" + String(mideDistancia());
      message.toCharArray(msg,50);
      client.publish("salida",msg);
      Serial.println("Mensaje Enviado-> "+String(mideDistancia()));
    }
  }
  
  delay(500);
}

int mideDistancia() {
  gatilla();
  long tiempo = pulseIn(echo,HIGH);
  int distancia= tiempo/58;
  return distancia;
}

void gatilla(){
  digitalWrite(trigg,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigg,LOW);
}
