#include <WiFi.h>
#include <PubSubClient.h>

#define LM35_PIN 35

const char* ssid = "";
const char* password = "";

const char* mqtt_server = "raspberrypi";
const int mqtt_port = 1883;
const char* mqtt_topic = "mendix/iot";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();
  float temperature = readTemperature();
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");
  if (temperature > 0) {
    publishTemperature(temperature);
  }
  delay(1000);
}

void setup_wifi() {
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s");
      delay(5000);
    }
  }
}

float readTemperature() {
  float temperature = analogRead(LM35_PIN) / 9.31;
  return temperature;
}

void publishTemperature(float temp) {
  char msg[10];
  dtostrf(temp, 6, 2, msg); 
  client.publish(mqtt_topic, msg);
  Serial.println("Temperatura publicada no MQTT!");
}
