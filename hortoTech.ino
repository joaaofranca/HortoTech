// Importando as bibliotecas necessárias
# include <ESP8266WiFi.h>
# include <PubSubClient.h>

// Define configs do MQTT
# define ID_MQTT "hortoTech_mqtt"
# define TOPICO_PUBLISH_UMIDADE "topico_sensor_umidade"

// Endereço e a porta do broker
const char * BROKER = "test.mosquitto.org";
int PORTA = 1883;

// Definindo os pinos dos dispositivos
# define rele 2
# define sensor_umidade 17

// Informações da rede WiFi
char * SSID = "";
char * PASSWORD = "";

WiFiClient espClient;
PubSubClient MQTT(espClient);

//Criando as váriaveis globais
int umidade = 1024;
char * msg = "";

// Função que se conecta ao MQTT
void ReconectMQTT(void) {
  if (!MQTT.connected()){
    while (!MQTT.connected()) {
      Serial.print("* Conectando-se ao MQTT: ");
      Serial.println(BROKER);
      if (MQTT.connect(ID_MQTT)) {
        Serial.println("Conectado com sucesso");
      } else {
        Serial1.println("Falha ao se conectar.");
        Serial.println("Reiniciando em 2s");
        delay(2000);
      }
    }
  }
}

// Iniciando o MQTT
void StartMQTT(void) {
  MQTT.setServer(BROKER, PORTA);
}

// Função que reconecta o serviço de WiFi
void ReconectWIFI(void) {
  if (WiFi.status() == WL_CONNECTED)
    return;
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Conectado na rede ");
}

void ConsultaConexoes(void) {
  ReconectWIFI(); 
  ReconectMQTT(); 
}

void setup() {

  Serial.begin(115200);

  pinMode(sensor_umidade, INPUT);
  pinMode(rele, OUTPUT);

  StartMQTT();
}

void loop() {
  ConsultaConexoes();
  umidade = analogRead(sensor_umidade);
  sprintf(msg, "%d", umidade);
  
  
  Serial.println("Valor da umidade");
  Serial.println(umidade);

  MQTT.publish(TOPICO_PUBLISH_UMIDADE, msg);
  MQTT.loop();
  delay(10000);
  
  // Caso a umidade seja ALTA
  if (umidade>=0 && umidade<400 ) {
    Serial.println("UMIDADE BOA");
  }

  else if (umidade>=400 && umidade<1025 ) {
    Serial.println("UMIDADE BAIXA");
    digitalWrite(rele, HIGH);
    delay (4000);
    digitalWrite(rele, LOW);
  }
  
  delay(10000);
}
