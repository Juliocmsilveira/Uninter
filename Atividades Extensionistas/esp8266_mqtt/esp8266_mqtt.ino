/*
 * Projeto: Sensor DHT11 + MQTT Atividades Extensionistas Uninter
 * Autor: Julio C. M. Silveira
 * Versão: 0.0.1
 * Data: 27/04/2024
 *
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Configuração do sensor DHT11
#define DHTPIN 4     // Pino onde o sensor está conectado
#define led 2
#define DHTTYPE DHT11 // Tipo do sensor DHT

// Configuração da rede Wi-Fi
const char* ssid = "ssid";
const char* password = "Senha";

// Configurações do broker MQTT
const char* mqttServer = "mqtt.eclipseprojects.io"; // Endereço do seu broker MQTT
const int mqttPort = 1883;
const char* mqttUser = "iot.eclipse.org";             // Se necessário
//const char* mqttPassword = "SUA_SENHA_MQTT";           // Se necessário
const char* mqttTopic = "esp8266/data";                // Tópico MQTT para publicação dos dados

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Conecta-se à rede Wi-Fi
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // While aguarda a conecçao com o WIFI, pisca o led em quanto nao a conexao
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    digitalWrite(led, LOW);
    delay(250);
    digitalWrite(led, HIGH);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Conectado à rede WiFi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Inicializa o cliente MQTT
  client.setServer(mqttServer, mqttPort);
  
  // Inicializa o sensor DHT
  dht.begin();
}

void reconnect() {
  // Tenta conectar ao broker MQTT
  while (!client.connected()) {
    Serial.println("Conectando ao broker MQTT...");
    
    if (client.connect("ESP8266Client"/*, mqttUser, mqttPassword */)) {
      Serial.println("Conectado ao broker MQTT!");
    } else {
      Serial.print("Falha na conexão ao broker MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

float humidity;
float temperature;
unsigned long currentMillis;
const long interval = 5000;              // Intervalo para atualizaçao MQTT
unsigned long previousMillis = 0;
const long intervalLedOff = 150;         // Intervalo para desligar o led
unsigned long previousMillisLedOff = 0;
int ledState = LOW;
char payload[100];                      // Cria a string JSON com os dados

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Mantém a conexão MQTT

  // Leitura da temperatura e umidade
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  // Verifica se a leitura foi bem-sucedida
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler o sensor DHT!");

    // pisca o led caso tenha erro de leitura do sensor
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
    digitalWrite(led, HIGH);
    return;
  }

  snprintf(payload, sizeof(payload), "{\"temperature\": %.1f, \"humidity\": %.1f}", temperature, humidity);
  
  // Chama a funçao que envia os dados por MQTT
  run();
  
}


void run(void){
  currentMillis = millis();

  // delay para envio de dados por MQTT
  if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      previousMillisLedOff = currentMillis;

      // Liga o led para indicar que houve envio de dado
      digitalWrite(led, LOW);
      Serial.print("Publicando dados: ");
      Serial.println(payload);

      // Publica os dados no tópico MQTT
      client.publish(mqttTopic, payload);
  }

  // desliga o led se o dado foi enviado apos o tempo corrido
  if ((currentMillis - previousMillisLedOff >= intervalLedOff) & (digitalRead(led) == 0)) {
    digitalWrite(led, HIGH);
  }
  
}
