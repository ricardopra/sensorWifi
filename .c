#include "LiquidCrystal_I2C.h"
#include "DHTesp.h"
#include "WiFi.h"
#include <PubSubClient.h>

const long id = 2249750;
const char* chave_e = "T437KP4DQI9FFED7";
int contador = 1;
char mensagem[30];
unsigned long lastMillis = 0;

///########### configuração do wifi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
// Defining the WiFi channel speeds up the connection:
#define WIFI_CHANNEL 1
int status = WL_IDLE_STATUS;    

//########## configuração do MQTT
const char* mqtt_server = "broker.mqtt.cool";// MQTT broker
char* my_topic_SUB = "ALERTA_SERVIDOR";// the chosen topic
char* my_topic_PUB = "ALERTA_SERVIDOR";// the chosen topic

/////##### configuração do wifi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

#define LED_T 19
#define LED_U 18

LiquidCrystal_I2C tela(0x27, 20, 4);

const int DHTPIN = 12;

DHTesp Sensor;

float t = 0;
float u = 0;

void setup() {
  Serial.begin(115200);
  Sensor.setup(DHTPIN, DHTesp::DHT22);

  Serial.begin(9600);
  Serial.print("Conectando-se ao Wi-Fi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Conectado!");

  //Wifi
  Serial.println("Conectando a rede Wifi");
  Serial.println();
  Serial.println("Conectado a Rede");

  conectawifi();
  client.setServer(mqtt_server, 1883); // Set the MQTT broker server and port
  client.setCallback(callback); // Set the callback function for incoming MQTT messages

   //definindo pinos
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);

  //configurações display
  tela.init();
  tela.backlight();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 delay(100); // Add a delay to avoid flooding the MQTT broker with messages

if (millis() - lastMillis > 7500) {

  TempAndHumidity entrada = Sensor.getTempAndHumidity();
  t = entrada.temperature;
  u = entrada.humidity;
  //imprimindo no display
  tela.setCursor(1,0);
  tela.print("Temp: " + String(entrada.temperature) + "C");
  delay(100);
    if (entrada.temperature > 35){
      digitalWrite(19, HIGH);
    }
    else {
      digitalWrite(19, LOW);
    }

  tela.setCursor(1,1);
  tela.println("==================");

  tela.setCursor(1,2);
  tela.println("Umidade: " + String(entrada.humidity) + "%");
  delay(100);
    if (entrada.humidity > 70){
      digitalWrite(18, HIGH);
    }
    else {
      digitalWrite(18, LOW);
    }

  tela.setCursor(1,3);
  tela.println("==================");
  
  delay(10); 

  if (entrada.humidity > 70){

  lastMillis = millis();
             
              sprintf(mensagem, "Servidor em Perigo", digitalWrite);
                              //sprintf(mensagem, "MQTT ESP32 - Mensagem no. %d", contador);  
              Serial.print("Mensagem enviada: ");
              Serial.println(mensagem);
            //Envia a mensagem ao broker
              client.publish(my_topic_PUB, mensagem);
              Serial.print(contador);
              Serial.println("-Mensagem enviada com sucesso...");
             
              //Incrementa o contador
              contador++;
    }

  }

}

void conectawifi()
  {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Conectando ao WiFi ");
  Serial.print(WIFI_SSID);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
     //WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
     //delay(1000);
  }
  Serial.println(" Conectado!");


  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  }

void callback(char* topic, byte* payload, unsigned int length) {
  String string;
  Serial.print("Chegou a mensagem [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
     string+=((char)payload[i]);  
  }
  Serial.print(string);
 
}

  void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT ...");
    if (client.connect("ESPClient")) {
      Serial.println("Conectado");
      client.subscribe(my_topic_SUB);
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tente novamente em 5 segundos");
     
     
  }
  }
}
