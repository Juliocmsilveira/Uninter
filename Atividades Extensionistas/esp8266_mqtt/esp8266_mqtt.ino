/*
 * Projeto: Sensor DHT11 + MQTT Atividades Extensionistas Uninter
 * Autor: Julio C. M. Silveira
 * Versão: 0.0.3
 * Data: 27/04/2024
 * UltimA modificação: 09/08/2024
 *
*/

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoOTA.h>
#include <FS.h> // Biblioteca para SPIFFS

// Configuração do sensor DHT11
#define DHTPIN 4
#define led 2
#define DHTTYPE DHT11

// Configurações do broker MQTT
const char* mqttServer = "mqtt.eclipseprojects.io";
const int mqttPort = 1883;
const char* mqttUser = "iot.eclipse.org";
const char* mqttTopic = "esp8266/data";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
String otaPassword = "defaultPassword"; // Senha OTA padrão

void setup() {
  Serial.begin(115200);
  delay(10);

  // Inicializa o SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Erro ao montar o sistema de arquivos SPIFFS!");
    return;
  }

  // Carrega a senha OTA do SPIFFS
  loadOTAPassword();

  // Configuração do WiFiManager
  WiFiManager wifiManager;
  if (!wifiManager.autoConnect("ESP8266-Config")) {
    Serial.println("Falha ao conectar e tempo limite alcançado!");
    ESP.reset();
    delay(1000);
  }

  Serial.println("Conectado à rede WiFi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Inicializa o cliente MQTT
  client.setServer(mqttServer, mqttPort);
  
  // Inicializa o sensor DHT
  dht.begin();

  // Configuração do ArduinoOTA
  ArduinoOTA.setPassword(otaPassword.c_str());  // Define a senha OTA carregada
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("Iniciando atualização via OTA: " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nAtualização OTA finalizada!");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Erro de autenticação");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Erro de inicialização");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Erro de conexão");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Erro de recepção");
    else if (error == OTA_END_ERROR) Serial.println("Erro de finalização");
  });

  ArduinoOTA.begin();
  Serial.println("Pronto para atualização OTA!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Conectando ao broker MQTT...");
    
    if (client.connect("ESP8266Client")) {
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
const long interval = 5000;
unsigned long previousMillis = 0;
const long intervalLedOff = 150;
unsigned long previousMillisLedOff = 0;
int ledState = LOW;
char payload[100];

void loop() {
  ArduinoOTA.handle();  // Verifica se há uma atualização OTA em andamento

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler o sensor DHT!");
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
    digitalWrite(led, HIGH);
    return;
  }

  snprintf(payload, sizeof(payload), "{\"temperature\": %.1f, \"humidity\": %.1f}", temperature, humidity);
  
  run();
}

void run(void){
  currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    previousMillisLedOff = currentMillis;
    digitalWrite(led, LOW);
    Serial.print("Publicando dados: ");
    Serial.println(payload);
    client.publish(mqttTopic, payload);
  }

  if ((currentMillis - previousMillisLedOff >= intervalLedOff) && (digitalRead(led) == 0)) {
    digitalWrite(led, HIGH);
  }
}

// Função para carregar a senha OTA do SPIFFS
void loadOTAPassword() {
  File f = SPIFFS.open("/otapass.txt", "r");
  if (!f) {
    Serial.println("Nenhuma senha OTA armazenada, usando a senha padrão.");
  } else {
    otaPassword = f.readString();
    Serial.println("Senha OTA carregada do SPIFFS: " + otaPassword);
    f.close();
  }
}

// Função para salvar a senha OTA no SPIFFS (chame isso quando quiser mudar a senha)
void saveOTAPassword(String newPassword) {
  File f = SPIFFS.open("/otapass.txt", "w");
  if (!f) {
    Serial.println("Erro ao abrir o arquivo para gravação.");
  } else {
    f.print(newPassword);
    otaPassword = newPassword;
    Serial.println("Nova senha OTA salva no SPIFFS: " + otaPassword);
    f.close();
  }
}
