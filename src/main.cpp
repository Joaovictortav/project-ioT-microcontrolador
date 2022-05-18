#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "name";
const char* password = "password";
WiFiClient wifiClient;

const char* mqttServer = "192.168.1.142";
const int mqttPort = 1883;
const char* clientId = "mqtt_1";
int pwm = 33;
int sensor = 36;

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

  pinMode(sensor, INPUT);
  pinMode(pwm, OUTPUT);

  conectaWiFi();
  MQTT.setServer(mqttServer, mqttPort); 
  MQTT.setCallback(recebePacote);
}

void loop(void) {  

  mantemConexoes();

  digitalWrite(pwm, HIGH);
  
  unsigned long now = millis();
  if (now - lastMsg > 49) {
    lastMsg = now;
    value = analogRead(sensor)*3.3/1023.0;
    snprintf (msg, MSG_BUFFER_SIZE, "%ld,%ld", lastMsg, value);
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
    
    Serial.print("Chegou a seguinte string via MQTT: ");
    Serial.println(msg);
}
