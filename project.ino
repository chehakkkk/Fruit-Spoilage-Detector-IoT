#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"
#include <LiquidCrystal.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// ===== Wi-Fi =====
const char* ssid = "1";         
const char* password = "11111111"; 

// ===== ThingSpeak =====
String apiKey = "07Y7V6ANUV79Z06C";       
const char* server = "api.thingspeak.com";

// ===== Telegram =====
#define BOT_TOKEN "8373460490:AAGGlHlPUYd9XE3GD0Zd-u8lf639TfkRdY8"  
#define CHAT_ID "1602664424"                                        
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// ===== Sensors =====
#define DHTPIN 15
#define DHTTYPE DHT11
#define MQ2PIN 34
#define MQ135PIN 35
DHT dht(DHTPIN, DHTTYPE);

// ===== LCD (16x2, non-I2C) =====
LiquidCrystal lcd(19, 23, 18, 5, 21, 4); // RS, E, D4, D5, D6, D7

// ===== Fruit Future Uses =====
String getFutureUses(String fruit) {
  fruit.toLowerCase();
  if(fruit == "apple") return "- Apple cider vinegar\n- Compost\n- Animal feed\n- Homemade wine\n- Fruit fly traps";
  else if(fruit == "banana") return "- Compost (potassium rich)\n- Animal feed\n- Banana vinegar\n- Skin care mask\n- Biogas production";
  else if(fruit == "tomato") return "- Compost\n- Sauce or puree\n- Animal feed\n- Biogas fuel";
  else return "No info available.";
}

// ===== Ripeness stages based on dataset =====
struct FruitStage {
  String stageName;
  int minMQ;
  int maxMQ;
};

// Banana stages
FruitStage bananaStages[] = {
  {"Unripe", 0, 250},
  {"Fresh", 251, 420},
  {"Ripe", 421, 690},
  {"Spoiled", 701, 2000}
};

// Apple stages
FruitStage appleStages[] = {
  {"Unripe", 0, 200},
  {"Fresh", 201, 360},
  {"Ripe", 361, 560},
  {"Spoiled", 561, 2000}
};

// Tomato stages
FruitStage tomatoStages[] = {
  {"Unripe", 0, 240},
  {"Fresh", 241, 410},
  {"Ripe", 411, 620},
  {"Spoiled", 621, 2000}
};

// ===== Telegram polling =====
unsigned long lastTime = 0;
const int botRequestDelay = 1000; // 1 sec
String selectedFruit = "";

// ===== Send Telegram alert =====
void sendTelegramAlert(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    bool sent = bot.sendMessage(CHAT_ID, message, "Markdown");
    if(sent) Serial.println("✅ Alert sent to Telegram!");
    else Serial.println("❌ Failed to send Telegram alert.");
  }
}

// ===== Handle Telegram commands =====
void handleTelegram() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text;
    String chat_id = String(bot.messages[i].chat_id);

    if (text.startsWith("/fruit ")) {
      String fruitName = text.substring(7);
      fruitName.trim();
      fruitName.toLowerCase();

      if (fruitName == "apple" || fruitName == "banana" || fruitName == "tomato") {
        selectedFruit = fruitName;
        bot.sendMessage(chat_id, "✅ Fruit set to: " + selectedFruit, "");
        Serial.println("Fruit selected via Telegram: " + selectedFruit);
      } else {
        bot.sendMessage(chat_id, "❌ Unknown fruit!", "");
      }
    }
  }
}

// ===== Function to get ripeness stage based on gas only =====
String getRipenessStage(String fruit, int avgMQ) {
  FruitStage* stages;
  int size = 4;

  if(fruit == "banana") stages = bananaStages;
  else if(fruit == "apple") stages = appleStages;
  else if(fruit == "tomato") stages = tomatoStages;
  else return "Unknown";

  for(int i=0; i<size; i++){
    if(avgMQ >= stages[i].minMQ && avgMQ <= stages[i].maxMQ){
      return stages[i].stageName;
    }
  }
  return "Unknown";
}

// ===== Setup =====
void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  lcd.begin(16,2);

  // Connect Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid,password);
  int retries = 0;
  while(WiFi.status()!=WL_CONNECTED && retries < 20){
    delay(1000);
    Serial.print(".");
    retries++;
  }

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("\n✅ Wi-Fi Connected!");
    Serial.print("IP: "); Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Failed to connect Wi-Fi!");
  }

  // Telegram HTTPS
  client.setInsecure();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Food Spoilage");
  lcd.setCursor(0,1);
  lcd.print("System Ready!");
  delay(2000);

  // ✅ Added startup message for fruit selection
  if (WiFi.status() == WL_CONNECTED) {
    String startupMsg = "👋 Food Spoilage Detection System Started!\n\n"
                        "Please select the fruit to monitor:\n"
                        "🍎 /fruit apple\n"
                        "🍌 /fruit banana\n"
                        "🍅 /fruit tomato\n\n"
                        "(Type any of the above commands to begin monitoring)";
    sendTelegramAlert(startupMsg);
  }
}

// ===== Loop =====
void loop() {
  // Handle Telegram commands
  if (millis() - lastTime > botRequestDelay) {
    handleTelegram();
    lastTime = millis();
  }

  // Skip if no fruit selected
  if(selectedFruit == "") {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Select fruit via");
    lcd.setCursor(0,1);
    lcd.print("Telegram /fruit");
    Serial.println("Waiting for fruit selection via Telegram...");
    delay(2000);
    return;
  }

  // Read sensors
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int gasValue1 = analogRead(MQ2PIN);
  int gasValue2 = analogRead(MQ135PIN);
  int avgGas = (gasValue1 + gasValue2)/2; // Use raw value for stage detection

  if (isnan(t) || isnan(h)) {
    Serial.println("❌ Failed to read from DHT!");
    return;
  }

  // Determine ripeness stage
  String ripenessStage = getRipenessStage(selectedFruit, avgGas);

  // Estimate days until spoilage
  String daysMsg = "";
  String lcdmssg="";
  if(ripenessStage == "Unripe") {
    daysMsg = "Consume in ~3-4 days!";
    lcdmssg="3-4 days left";
  }
  else if(ripenessStage == "Fresh") {
    daysMsg = "Consume in 1~2 days!";
    lcdmssg="1-2 days left";
  }
  else if(ripenessStage == "Ripe") {
    daysMsg = "Best if consumed today!";
    lcdmssg="Consume today";
  }
  else if(ripenessStage == "Spoiled") {
    daysMsg = "Already spoiled! DO NOT CONSUME!!";
    lcdmssg="Don't consume";
  }

  // Update LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(selectedFruit.substring(0,6) + ": " + ripenessStage);
  lcd.setCursor(0,1);
  lcd.print(lcdmssg);

  // Send ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient clientTS;
    HTTPClient http;
    String url = "http://" + String(server) + "/update?api_key=" + apiKey +
                 "&field1=" + String(t) +
                 "&field2=" + String(h) +
                 "&field3=" + String(avgGas);
    http.begin(clientTS, url);
    int httpCode = http.GET();
    http.end();
    if (httpCode > 0) Serial.println("✅ Data sent to ThingSpeak!");
    else Serial.println("❌ Error sending data to ThingSpeak.");
  }

  // Telegram alert
  if(ripenessStage == "Spoiled") {
    String alert = "⚠ Food Spoilage Alert!\nFruit: " + selectedFruit +
                   "\nStage: " + ripenessStage + "" +
                   "\nGas(MQ avg): " + String(avgGas) +
                   "\nTemp: " + String(t) +
                   "°C  Hum: " + String(h) + "%\n\n" +
                   "💡 Future Uses:\n" + getFutureUses(selectedFruit);
    sendTelegramAlert(alert);
  } else {
    String alert = "✅ Fruit status: *" + selectedFruit +
                   "*\nStage: " + ripenessStage +
                   "\n" + daysMsg;
    sendTelegramAlert(alert);
  }

  // Serial monitor
  Serial.print("Fruit: "); Serial.print(selectedFruit);
  Serial.print(" | Temp: "); Serial.print(t);
  Serial.print(" | Hum: "); Serial.print(h);
  Serial.print(" | Gas1: "); Serial.print(gasValue1);
  Serial.print(" | Gas2: "); Serial.print(gasValue2);
  Serial.print(" | AvgGas: "); Serial.print(avgGas);
  Serial.print(" | Stage: "); Serial.println(ripenessStage);

  delay(50000); // ThingSpeak delay
}