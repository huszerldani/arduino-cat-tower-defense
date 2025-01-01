#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// Eszköz szerepe: állítsd "CENTRAL" vagy "REMOTE" értékre
#define DEVICE_ROLE "REMOTE" // Vagy "REMOTE"

#define ACTIVATE_MESSAGE "01"

Servo servo;

// LED pin (ESP32-C3 beépített LED)
#define LED_PIN 8

#define SERVO_PIN 3

int servoStart = 20;
int servoEnd = 50;
bool activateMessageSent = false;
bool activateInProgress = false;

// Távirányító esetén szükséges MAC cím (a központi egység MAC címe)
uint8_t centralMAC[] = {0x18, 0x8B, 0x0E, 0x1D, 0xD9, 0x1C};

// Callback a bejövő adatok kezeléséhez (központi egység számára)
void onDataReceive(const esp_now_recv_info *recv_info, const uint8_t *incomingData, int len) {
  Serial.println("Adat érkezett:");
  Serial.write(incomingData, len);
  Serial.println();

  if (len == 2 && strncmp((const char *)incomingData, ACTIVATE_MESSAGE, 2) == 0) {
    handleActivateMessage();
  } else {
    Serial.println("Ismeretlen üzenet.");
  }

  digitalWrite(LED_PIN, LOW);
}

void handleActivateMessage() {
  if (activateInProgress == true) {
    return;
  }

  activateInProgress = true;

  Serial.println("ACTIVATE üzenet fogadva.");

  digitalWrite(LED_PIN, HIGH);

  if (!servo.attached()) {
    servo.attach(SERVO_PIN);
  }

  // Szervó a célpozícióba
  servo.write(servoEnd);
  Serial.println("Szervó a célpozícióba állítva.");

  delay(300); // Várunk 100 ms-ot

  // Szervó alaphelyzetbe
  servo.write(servoStart);
  Serial.println("Szervó visszaállítva az alaphelyzetbe.");

  delay(200); // Várunk, mielőtt a szervót leválasztanánk

  servo.detach();
  Serial.println("Szervó leválasztva.");

  delay(200);

  activateInProgress = false;
}

void setup() {
  Serial.begin(9600);

  // LED inicializálása
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_STA); // Csak ESP-NOW mód
  if (esp_now_init() != ESP_OK) {
    Serial.println("Hiba az ESP-NOW inicializálása közben!");
    digitalWrite(LED_PIN, HIGH); // Hiba jelzése
    return;
  }
  Serial.println("ESP-NOW inicializálva.");

  // Szerepkör alapú inicializáció
  if (strcmp(DEVICE_ROLE, "CENTRAL") == 0) {
    Serial.println("Központi egység módban.");
    esp_now_register_recv_cb(onDataReceive);

    printMacAddress();

    servo.attach(SERVO_PIN);
    servo.write(servoStart);
    Serial.println("Szervó alaphelyzetbe állítva.");
  } else if (strcmp(DEVICE_ROLE, "REMOTE") == 0) {
    Serial.println("Távirányító módban.");
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, centralMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Hiba a peer hozzáadásakor!");
      digitalWrite(LED_PIN, HIGH); // Hiba jelzése
      return;
    }
    Serial.println("Peer hozzáadva.");
  } else {
    Serial.println("Érvénytelen szerepkör! Állítsd be a DEVICE_ROLE értékét \"CENTRAL\" vagy \"REMOTE\"-ra.");
    digitalWrite(LED_PIN, HIGH); // Hiba jelzése
    return;
  }
}

void loop() {
  if (strcmp(DEVICE_ROLE, "CENTRAL") == 0) {

  }

  if (strcmp(DEVICE_ROLE, "REMOTE") == 0) {
    if (activateMessageSent) {
      return;
    }

    esp_now_send(centralMAC, (uint8_t *)ACTIVATE_MESSAGE, strlen(ACTIVATE_MESSAGE));
    Serial.println("Üzenet elküldve: ACTIVATE");

    // LED villogtatás az üzenet elküldésekor
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);

    activateMessageSent = true;
  }
}

void printMacAddress() {
  String macStr = WiFi.macAddress();
  Serial.println("Eredeti MAC cím (string):");
  Serial.println(macStr);

  // Átalakítás byte tömbre
  uint8_t macAddress[6];
  int parsed = sscanf(macStr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                      &macAddress[0], &macAddress[1], &macAddress[2],
                      &macAddress[3], &macAddress[4], &macAddress[5]);

  if (parsed != 6) {
    Serial.println("Hiba a MAC cím értelmezése közben!");
    return;
  }

  // Kiírás byte tömb formában
  Serial.println("MAC cím byte tömbként:");
  for (int i = 0; i < 6; i++) {
    Serial.printf("0x%02X", macAddress[i]);
    if (i < 5) Serial.print(", ");
  }
  Serial.println();
}
