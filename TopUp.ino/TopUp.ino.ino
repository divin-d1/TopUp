#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#define SS_PIN D2
#define RST_PIN D1
MFRC522 rfid(SS_PIN, RST_PIN);

const char* ssid = "EdNet";
const char* password = "Huawei@123";

const char* mqtt_server = "157.173.101.159";
const int mqtt_port = 1883;
const char* team_id = "deltaForce001";

WiFiClient espClient;
PubSubClient client(espClient);

struct CardBalance {
  String uid;
  int balance;
};

CardBalance cards[20];  // Support up to 20 cards
int cardCount = 0;

// ======================== MQTT CALLBACK ========================
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  DynamicJsonDocument doc(256);
  deserializeJson(doc, msg);

  String uid = doc["uid"];
  int amount = doc["amount"];

  // Update balance for this UID
  for (int i = 0; i < cardCount; i++) {
    if (cards[i].uid == uid) {
      cards[i].balance += amount;
      publishBalance(uid, cards[i].balance);
      return;
    }
  }

  // If new card, add it
  if (cardCount < 20) {
    cards[cardCount].uid = uid;
    cards[cardCount].balance = amount;
    cardCount++;
    publishBalance(uid, amount);
  }
}

// ======================== CONNECT TO MQTT ========================
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      client.subscribe(("rfid/" + String(team_id) + "/card/topup").c_str());
    } else {
      delay(5000);
    }
  }
}

// ======================== SETUP ========================
void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  EEPROM.begin(512);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// ======================== LOOP ========================
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uid += String(rfid.uid.uidByte[i], HEX);
    }

    int balance = getBalance(uid);
    publishStatus(uid, balance);
  }
}

// ======================== BALANCE MANAGEMENT ========================
int getBalance(String uid) {
  for (int i = 0; i < cardCount; i++) {
    if (cards[i].uid == uid) return cards[i].balance;
  }

  // New card defaults to 0
  if (cardCount < 20) {
    cards[cardCount].uid = uid;
    cards[cardCount].balance = 0;
    cardCount++;
  }

  return 0;
}

// ======================== PUBLISH STATUS ========================
void publishStatus(String uid, int balance) {
  DynamicJsonDocument doc(256);
  doc["uid"] = uid;
  doc["balance"] = balance;

  String payload;
  serializeJson(doc, payload);
  client.publish(("rfid/" + String(team_id) + "/card/status").c_str(), payload.c_str());
}

// ======================== PUBLISH NEW BALANCE ========================
void publishBalance(String uid, int new_balance) {
  DynamicJsonDocument doc(256);
  doc["uid"] = uid;
  doc["new_balance"] = new_balance;

  String payload;
  serializeJson(doc, payload);
  client.publish(("rfid/" + String(team_id) + "/card/balance").c_str(), payload.c_str());
}