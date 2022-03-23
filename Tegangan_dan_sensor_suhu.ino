#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 5

const char* ssid = "HOMY";
const char* password =  "Cats2021";
const char* mqttServer = "192.168.1.10";
const int mqttPort =  1883;
const char* mqttUser = "";
const char* mqttPassword = "";

WiFiClient espClient;
PubSubClient client(espClient);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
char msg_out2[20];

ADC_MODE(ADC_VCC);
char msg_out[20];

void setup() {
  Serial.begin(115200);
  sensors.begin();
  pinMode(5, INPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

void loop() {
  // if (!client.connected()) {}
  float vdd = ESP.getVcc() / 1024.0;
  Serial.print(vdd);
  dtostrf(vdd, 2, 2, msg_out);
  client.publish("vin", msg_out);
  client.loop();
  delay (1000);

  sensors.requestTemperatures();
  Serial.print(sensors.getTempCByIndex(0));
  dtostrf(sensors.getTempCByIndex(0), 2, 2, msg_out2);
  client.publish("temp", msg_out2);
  client.loop();
  delay (1000);

}
