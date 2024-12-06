#include <esp_now.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi Configuration
const char* WIFI_SSID = ""; //Give your WiFi ID
const char* WIFI_PASSWORD = ""; //Give your WiFi password

// MQTT Configuration
const char* MQTT_BROKER = "broker.mqtt.cool";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC_BASE = "H00447757/";

// MQTT Topics for Interval Configuration
const char* MQTT_TOPIC_SAMPLING_INTERVAL = "H00447757/config/sampling_interval";
const char* MQTT_TOPIC_COMMUNICATION_INTERVAL = "H00447757/config/communication_interval";
const char* MQTT_TOPIC_TEMP_ALERT = "H00447757/config/temp_alert";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

#pragma pack(1) // Ensure structure alignment
// Structure for interval configuration
typedef struct interval_config {
  unsigned long samplingInterval;
  unsigned long communicationInterval;
  unsigned long tempAlert;
} interval_config;

// Structure to send to slaves
typedef struct slave_message {
  int nodeId;
  float temperature;
  float humidity;
  interval_config intervals;
} slave_message;
#pragma pack()

// Peer MAC Addresses (replace with actual MAC addresses)
uint8_t slave1_address[] = {0x08, 0xB6, 0x1F, 0x28, 0x63, 0x8C}; // Slave MAC Address
uint8_t slave2_address[] = {0x08, 0xB6, 0x1F, 0x29, 0xD1, 0xC0};//08:B6:1F:29:D1:C0
// Interval variables
unsigned long samplingInterval = 5000;  // Default 5 seconds
unsigned long communicationInterval = 60000;  // Default 1 minute
unsigned long tempAlert = 30; // Default 30 degrees

esp_now_peer_info_t peerInfo;

void configurePeer(uint8_t* macAddr) {
  memcpy(peerInfo.peer_addr, macAddr, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    //Serial.println("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32Bridge")) {
      Serial.println("MQTT Connected");
      mqttClient.subscribe(MQTT_TOPIC_SAMPLING_INTERVAL);
      mqttClient.subscribe(MQTT_TOPIC_COMMUNICATION_INTERVAL);
    } else {
      //Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.printf("Received message on topic %s: %s\n", topic, message.c_str());

  unsigned long newInterval = message.toInt();

  if (strcmp(topic, MQTT_TOPIC_SAMPLING_INTERVAL) == 0) {
    if (newInterval > 0) {
      samplingInterval = newInterval;
      Serial.printf("Updated Sampling Interval: %lu ms\n", samplingInterval);
    }
  }

  if (strcmp(topic, MQTT_TOPIC_COMMUNICATION_INTERVAL) == 0) {
    if (newInterval > 0) {
      communicationInterval = newInterval;
      Serial.printf("Updated Communication Interval: %lu ms\n", communicationInterval);
    }
  }

  if (strcmp(topic, MQTT_TOPIC_TEMP_ALERT) == 0) {
    if (newInterval > 0) {
      tempAlert = newInterval;
      Serial.printf("Updated Temperature Threshold for Alert: %lu ms\n", tempAlert);
    }
  }

  // Broadcast new intervals to slaves
  slave_message msg;
  msg.intervals.samplingInterval = samplingInterval;
  msg.intervals.communicationInterval = communicationInterval;
  msg.intervals.tempAlert = tempAlert;

  esp_err_t result = esp_now_send(slave1_address, (uint8_t*)&msg, sizeof(msg));
  if (result != ESP_OK) {
    Serial.printf("ESP-NOW send failed with error: %d\n", result);
  } else {
    Serial.printf("Broadcasting - Sampling Interval: %lu, Communication Interval: %lu\n, Temperature Threshold: %lu\n",
                  samplingInterval, communicationInterval, tempAlert);
  }
}

void OnDataRecv(const esp_now_recv_info_t* info, const uint8_t* incomingData, int len) {
  slave_message msg;
  memcpy(&msg, incomingData, sizeof(msg));

  char topic[50];
  snprintf(topic, sizeof(topic), "%snode%d/temperature", MQTT_TOPIC_BASE, msg.nodeId);
  char temp[10];
  dtostrf(msg.temperature, 4, 2, temp);
  mqttClient.publish(topic, temp);

  snprintf(topic, sizeof(topic), "%snode%d/humidity", MQTT_TOPIC_BASE, msg.nodeId);
  char hum[10];
  dtostrf(msg.humidity, 4, 2, hum);
  mqttClient.publish(topic, hum);

  Serial.printf("Received from Node %d: Temp = %.2f, Humidity = %.2f\n",
                msg.nodeId, msg.temperature, msg.humidity);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  configurePeer(slave1_address);
  configurePeer(slave2_address);

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
}
