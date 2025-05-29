#include <esp_now.h>
#include <WiFi.h>

uint8_t serverMac[] = {0x78, 0x42, 0x1C, 0x6D, 0x2D, 0x9C};
const char *message = "Truck";

// Callback when data is received
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  char msg[10];
  memcpy(msg, data, len);
  msg[len] = '\0';
  Serial.print("Received from Server: ");
  Serial.println(msg);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  // Register Callback
  esp_now_register_recv_cb(onReceive);

  // Add Server as Peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, serverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.println("Client 2 Ready");
}

void loop() {
  esp_now_send(serverMac, (uint8_t *)message, strlen(message));
  Serial.println("Message Sent: Truck");
  delay(5000);
}
