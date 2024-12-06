#include <esp_now.h>
#include <WiFi.h>
#include <DHT.h>

#define DHTPIN 17
#define DHTTYPE DHT11

const int NODE_ID = 1;

#pragma pack(1)
typedef struct interval_config {
  unsigned long samplingInterval;
  unsigned long communicationInterval;
} interval_config;

typedef struct slave_message {
  int nodeId;
  float temperature;
  float humidity;
  interval_config intervals;
} slave_message;
#pragma pack()

DHT dht(DHTPIN, DHTTYPE);

unsigned long samplingInterval = 5000;  // Default 5 seconds
unsigned long communicationInterval = 60000;  // Default 1 minute
unsigned long lastSamplingTime = 0;

uint8_t masterAddress[] = {0x08, 0xB6, 0x1F, 0x28, 0x60, 0x0C};
//08:B6:1F:28:60:0C 08:B6:1F:28:60:0C

void OnDataRecv(const esp_now_recv_info_t* info, const uint8_t* incomingData, int len) {
  if (len != sizeof(slave_message)) {
    Serial.printf("Expected size: %d, Received size: %d\n", sizeof(slave_message), len);
    return;
  }

  slave_message msg;
  memcpy(&msg, incomingData, sizeof(msg));

  samplingInterval = msg.intervals.samplingInterval;
  communicationInterval = msg.intervals.communicationInterval;

  Serial.printf("Received Interval Config - Sampling: %lu, Communication: %lu\n",
                samplingInterval, communicationInterval);
}

void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Transmission Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  dht.begin();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, masterAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Read temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  slave_message msg;
  msg.nodeId = NODE_ID;
  msg.temperature = temperature;
  msg.humidity = humidity;

  esp_err_t result = esp_now_send(masterAddress, (uint8_t*)&msg, sizeof(msg));
  if (result != ESP_OK) {
    Serial.println("Error sending message");
  } else {
    Serial.printf("Sent - Node ID: %d, Temperature: %.2f, Humidity: %.2f\n",
                  msg.nodeId, msg.temperature, msg.humidity);
  }

  // Set the next wakeup time
  esp_sleep_enable_timer_wakeup(samplingInterval * 1000); // Convert to microseconds
  Serial.printf("Going to sleep for %lu seconds...\n", samplingInterval / 1000);
  esp_deep_sleep_start(); // Enter deep sleep
}
