#include "DHTesp.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//ensure select wemos d1 mini esp12 nodemcu board
const char* ssid = "stayaway"; // Enter your WiFi name
const char* password =  "xxxx"; // Enter WiFi password
const char* mqttServer = "xxxm";
const int mqttPort = 10140;
const char* mqttUser = "xxxx";
const char* mqttPassword = "xxxx";

const int ledPin =  LED_BUILTIN;
IPAddress staticIP(192, 168, 0, 49);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 0, 1);

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dht;
static char temp[15];
static char humidity[15];
static char voltage[15];
static char adc[15];
static char rssi[15];
unsigned int raw = 0;
String raw_string;
#define durationSleep 3600000000
//10 Minutes = 600000000 Microseconds
// 3600000000 microsecond = 60 minute
//120 Minutes = 7200000000 Microseconds
//180 Minutes = 10800000000 Microseconds
void setup() {
  Serial.begin(115200);
  //turn wifi radio off - saves a bit of power
  WiFi.mode ( WIFI_OFF );
  WiFi.forceSleepBegin();
  delay(1);
  // ----------

  //setup sensoors and read Sensor data and set variables
  dht.setup(D5, DHTesp::DHT22);
  pinMode(A0, INPUT);
  pinMode(ledPin, OUTPUT);
  //flash led to show start
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay (100);
  digitalWrite(ledPin, HIGH);

  //temp
  float t = dht.getTemperature();
  float temp_read = dht.getTemperature();
  int i;
  for (i = 0; i < 10; i++) {
    temp_read = dht.getTemperature();
    t += temp_read;
    Serial.print(i);
    Serial.print(" and temp :");
    Serial.println(temp_read);
    delay(100);
  }

  t /= 11;
  Serial.println("final average.....");
  Serial.println(t);
  dtostrf(t, 5, 2, temp);

  //voltage
  raw = analogRead(A0);
  int temp_raw = analogRead(A0);
  for (i = 0; i < 10; i++) {
    raw = analogRead(A0);
    temp_raw += raw;
    Serial.print(i);
    Serial.print(" and ADC :");
    Serial.println(raw);
    delay(100);
  }
  temp_raw /= 11;
  float volt = temp_raw / 89.8;
  Serial.println("final average.....");
  Serial.println(temp_raw);

  //volt = volt * 4.2;
  dtostrf(volt, 5, 2, voltage);
  dtostrf(raw, 5, 2, adc);

  // humidity
  float h = dht.getHumidity();
  dtostrf(h, 5, 2, humidity);
  //wifi db
  long db = WiFi.RSSI();
  ltoa(db, rssi, 10);
  Serial.print("RSSI:");
  Serial.println(rssi);
  //-----------

  //bring up wifi and connect
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay (100);
  digitalWrite(ledPin, HIGH);
  
  WiFi.forceSleepWake();
  delay(1);
  WiFi.persistent( false );
  WiFi.mode ( WIFI_STA ) ;
  WiFi.begin(ssid, password);
  WiFi.config(staticIP, dns, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to Wifi..");
  Serial.println(WiFi.localIP());
  db = WiFi.RSSI();
  ltoa(db, rssi, 10);
  Serial.print("RSSI....after connect:");
  Serial.println(rssi);
 
  client.setServer(mqttServer, mqttPort);
  
  
  while (!client.connected()) {
    Serial.println("Connecting MQTT....");
    if (client.connect("ESP12Board", mqttUser, mqttPassword )) {
   
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      delay(1000);
    }
  }
  //------------------
  //publish mqtt
  Serial.println("publishing to mqtt..");
  client.publish("xxxxx/esp12board/sensor/esp_temperature/state", temp); //Topic name
  client.publish("xxxx/esp12board/sensor/battery_voltage/state", voltage); //A0 value
  client.publish("xxxx/esp12board/sensor/esp32_wifisignal/state", rssi); //Topic name
  client.publish("xxxx/esp12board/sensor/esp_humidity/state", humidity); //Topic name
  client.publish("xxxx/esp12board/sensor/adc/state", adc); //Topic name
  
  //flash led to show done
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay (100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay (100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay (100);
  digitalWrite(ledPin, HIGH);
  
  Serial.println("entering deep sleep.....");
  //enter sleep
  WiFi.disconnect(true);
  delay(1);
  ESP.deepSleep(durationSleep, WAKE_RF_DISABLED );
  //ESP.deepSleep(5e6, WAKE_RF_DISABLED);
}

void loop() {
  int raw = analogRead(A0);
  float volt = 0.1;
  volt = raw / 193.00;
  Serial.begin(115200);
  Serial.println(raw);
  Serial.println(volt);
  //raw_string = String (raw);
  //raw_string.toCharArray(voltage,5);
  //client.publish("tele/esp12board/sensor/battery_voltage/state", voltage);
  delay(2000);
}
