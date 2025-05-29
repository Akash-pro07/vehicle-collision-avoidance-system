#include <esp_now.h>
#include <WiFi.h>

uint8_t client1Mac[] = {0x78, 0x42, 0x1C, 0x6C, 0x59, 0x44};  // Car
uint8_t client2Mac[] = {0x1C, 0x69, 0x20, 0x94, 0x6B, 0xEC};  // Truck

char carMessage[10];
char truckMessage[10];

// Callback when data is received
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  char message[10];
  memcpy(message, data, len);
  message[len] = '\0';

  Serial.print("Received from: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", info->src_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.print(" | Message: ");
  Serial.println(message);

  if (memcmp(info->src_addr, client1Mac, 6) == 0) {
    strcpy(carMessage, message);
  } else if (memcmp(info->src_addr, client2Mac, 6) == 0) {
    strcpy(truckMessage, message);
  }
}

// Send message to clients
void sendMessage(const uint8_t *mac, const char *msg) {
  esp_err_t result = esp_now_send(mac, (uint8_t *)msg, strlen(msg));
  if (result == ESP_OK) {
    Serial.println("Message sent successfully");
  } else {
    Serial.println("Failed to send message");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  // Register Callbacks
  esp_now_register_recv_cb(onReceive);

  // Add peers
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, client1Mac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  memcpy(peerInfo.peer_addr, client2Mac, 6);
  esp_now_add_peer(&peerInfo);

  Serial.println("Server Ready");
}

void loop() {
  if (strlen(carMessage) > 0 && strlen(truckMessage) > 0) {
    sendMessage(client1Mac, truckMessage);
    sendMessage(client2Mac, carMessage);
    Serial.println("Exchanged Messages between Clients");
    
    // Clear messages
    memset(carMessage, 0, sizeof(carMessage));
    memset(truckMessage, 0, sizeof(truckMessage));
  }
  delay(1000);
}
