#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "name";
const char* password = "password";
WiFiClient wifiClient;

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* clientId = "mqtt_1";

const int ledVermelho = 27;
const int ledAzul = 26;
const int ledVerde = 25;

const char* topicLedAzul = "ledAzul";
const char* topicLedVermelho = "ledVermelho";
const char* topicLedVerde = "ledVerde";

PubSubClient MQTT(wifiClient);  

//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void recebePacote(char* topic, byte* payload, unsigned int length);

void setup(void) {
  Serial.begin(115200);
  conectaWiFi();
  MQTT.setServer(mqttServer, mqttPort); 
  MQTT.setCallback(recebePacote);

  pinMode(ledVermelho, OUTPUT);
  pinMode(ledAzul, OUTPUT);
  pinMode(ledVerde, OUTPUT);
}

void loop(void) {    
  mantemConexoes();
  // MQTT.publish(topicLedVermelho, "1");
  MQTT.loop();
}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
    delay(2000);
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(mqttServer);
        if (MQTT.connect(clientId)) {
            Serial.println("Conectado ao Broker com sucesso!");
            MQTT.subscribe(topicLedAzul);
            MQTT.subscribe(topicLedVermelho);
            MQTT.subscribe(topicLedVerde);
        } 
        else {
            Serial.println("Não foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 5s");
            delay(5000);
        }
    }
}

void conectaWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
     return;
  }   
  Serial.print("Conectando-se na rede: ");
  WiFi.begin(ssid, password); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(ssid);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void recebePacote(char* topic, byte* payload, unsigned int length) 
{
    String msg;
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }

    if (msg == "1") {
      digitalWrite(ledVermelho, HIGH);
      digitalWrite(ledAzul, LOW);
      digitalWrite(ledVerde, LOW);
    } else if (msg == "2") {
      digitalWrite(ledVermelho, LOW);
      digitalWrite(ledAzul, HIGH);
      digitalWrite(ledVerde, LOW);
    } else if (msg == "3") {
      digitalWrite(ledVermelho, LOW);
      digitalWrite(ledAzul, LOW);
      digitalWrite(ledVerde, HIGH);
    }
    
    Serial.print("Chegou a seguinte string via MQTT: ");
    Serial.println(msg);
}
