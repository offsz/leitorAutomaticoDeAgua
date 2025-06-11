#include <Arduino.h>
#include <digitalWriteFast.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

void setupWiFi() {
  delay(1000);
  Serial.print("Conectando-se à ");
  lcd.setCursor(0, 0);
  lcd.print("Conectando-se");
  Serial.println(ssid);
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    spinner();
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConectado ao Wi-Fi");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar MQTT...");
    if (client.connect("espClient")) {
      Serial.println("Conectado");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

const int TRIGGPIN1 = 4;
const int ECHOPIN1 = 5;
const int VALVE1 = 13;
const int PUMP = 14;
const float SPEED_OF_SOUND = 0.034 / 2;

long duration1;
int distance1;
int lastDistance1 = -1;
int tankheight1 = 80;
int sensorHeight1 = 95;
int tankradius1 = 45;
double pi = 3.1415926535897932384626433832795;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  intro();
  pinMode(TRIGGPIN1, OUTPUT);
  pinMode(ECHOPIN1, INPUT);
  pinMode(VALVE1, OUTPUT);
  pinMode(PUMP, OUTPUT);
  delay(4000);
  lcd.clear();
  setupWiFi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  ULSensor1();
  int waterheight1;

  if (distance1 <= 2) {
    waterheight1 = tankheight1;
  } else if (distance1 >= sensorHeight1) {
    waterheight1 = 0;
  } else {
    waterheight1 = sensorHeight1 - distance1;
    waterheight1 = constrain(waterheight1, 0, tankheight1);
  }

  float tankFilledPercentage1 = (waterheight1 / (float)tankheight1) * 100;

  if (distance1 != lastDistance1) {
    Serial.print("Tanque - Distância: ");
    Serial.print(distance1);
    Serial.println(" cm");
    Serial.print("Tanque - Altura da água: ");
    Serial.print(waterheight1);
    Serial.println(" cm");
    Serial.print("Tanque - Porcentagem cheia: ");
    Serial.print(tankFilledPercentage1);
    Serial.println("%");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tanque: ");
    lcd.print(tankFilledPercentage1);
    lcd.print("%");

    if (distance1 >= 87) {
      digitalWriteFast(VALVE1, HIGH);
    } else if (distance1 <= 16) {
      digitalWriteFast(VALVE1, LOW);
    }

    if (digitalRead(VALVE1)) {
      digitalWriteFast(PUMP, HIGH);
    } else {
      digitalWriteFast(PUMP, LOW);
    }

    // Publicar no MQTT
    char msg1[10], d1[10];
    dtostrf(tankFilledPercentage1, 4, 2, msg1);
    sprintf(d1, "%d", distance1);

    client.publish("tanque/porcentagem", msg1);
    client.publish("tanque/distancia", d1);

    lastDistance1 = distance1;
  }

  delay(500);
}

void ULSensor1() {
  digitalWriteFast(TRIGGPIN1, LOW);
  delayMicroseconds(2);
  digitalWriteFast(TRIGGPIN1, HIGH);
  delayMicroseconds(10);
  digitalWriteFast(TRIGGPIN1, LOW);
  duration1 = pulseIn(ECHOPIN1, HIGH);
  distance1 = duration1 * SPEED_OF_SOUND + 1;
}

void intro() {
  lcd.println("MONITORAMENTO");
  lcd.setCursor(0, 1);
  lcd.println("DA AGUA V1.0");
}

void spinner() {
  static int8_t counter = 0;
  const char* glyphs = "\xa1\xa5\xdb";
  lcd.setCursor(15, 0);
  lcd.print(glyphs[counter++]);
  if (counter == strlen(glyphs)) {
    counter = 0;
  }
}