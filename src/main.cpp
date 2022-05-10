#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "nome da rede";
const char* password = "senha";
WiFiClient wifiClient;

const char* mqttServer = "192.168.1.142";
const int mqttPort = 1883;
const char* clientId = "mqtt_1";

const int ledVermelho = 27;
const int ledAzul = 26;
const int ledVerde = 25;

const char* topicLedAzul = "ledAzul";
const char* topicLedVermelho = "ledVermelho";
const char* topicLedVerde = "ledVerde";

unsigned long lastMsg = 0;
int value = 0;
#define MSG_BUFFER_SIZE	(10)
char msg[MSG_BUFFER_SIZE];

PubSubClient MQTT(wifiClient);  

//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void recebePacote(char* topic, byte* payload, unsigned int length);

void setup(void) {
  Serial.begin(9600);
  conectaWiFi();
  MQTT.setServer(mqttServer, mqttPort); 
  MQTT.setCallback(recebePacote);

  pinMode(ledVermelho, OUTPUT);
  pinMode(ledAzul, OUTPUT);
  pinMode(ledVerde, OUTPUT);
}

void loop(void) {  

  mantemConexoes();
  
  unsigned long now = millis();
  if (now - lastMsg > 50) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "%ld", value);
    MQTT.publish("outTopic", msg);
  }

  MQTT.loop();
}

void mantemConexoes() {

  if (WiFi.status() != WL_CONNECTED) 
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita

  if (!MQTT.connected()) 
      conectaMQTT(); 

}

void conectaMQTT() { 

    Serial.print("Conectando ao Broker MQTT: ");
    Serial.print(mqttServer);
    Serial.print("  Port: ");
    Serial.println(mqttPort);

    if(MQTT.connect(clientId)){
      Serial.println("Conectado ao Broker com sucesso!");
      MQTT.subscribe(topicLedAzul);
      MQTT.subscribe(topicLedVermelho);
      MQTT.subscribe(topicLedVerde);
      return;
    }

    Serial.println("Não foi possivel se conectar ao broker.");
    Serial.println("Nova tentatica de conexao em 3s");
    delay(3000);
}

void conectaWiFi() {

  WiFi.begin(ssid, password); // Conecta na rede WI-FI  
  Serial.print("Conectando-se na rede Wi-Fi: ");
  Serial.println(ssid); 
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(ssid);  
  Serial.print("  IP: ");
  Serial.println(WiFi.localIP()); 
}

void recebePacote(char* topic, byte* payload, unsigned int length) 
{
    String msg;
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }

    if (msg == "1") {
      digitalWrite(ledVermelho, HIGH);
      digitalWrite(ledAzul, LOW);
      digitalWrite(ledVerde, LOW);
      value = 0;
    } else if (msg == "2") {
      digitalWrite(ledVermelho, LOW);
      digitalWrite(ledAzul, HIGH);
      digitalWrite(ledVerde, LOW);
      value = 0;
    } else if (msg == "3") {
      digitalWrite(ledVermelho, LOW);
      digitalWrite(ledAzul, LOW);
      digitalWrite(ledVerde, HIGH);
      value = 0;
    }
    
    Serial.print("Chegou a seguinte string via MQTT: ");
    Serial.println(msg);
}
