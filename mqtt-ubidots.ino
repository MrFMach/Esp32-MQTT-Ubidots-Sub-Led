#include <WiFi.h>​
#include <PubSubClient.h>​

#define WIFISSID "Put your WifiSSID here"​
#define PASSWORD "Put your wifi password here"​
#define TOKEN "Put your Ubidots' TOKEN"​
#define MQTT_CLIENT_NAME "Ubidots MQTT client Name"​

#define VARIABLE_LABEL_SUBSCRIBE "led13" // Assing the your variable label​
#define DEVICE_LABEL "myesp32" // Assig the your device label​

#define LED13 13 // Set according to the wiring

char mqttBroker[]  = "industrial.api.ubidots.com";
char payload[100];
char topic[150];
char topicSubscribe[100];

/****************************************
 * Auxiliar Functions
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  if (message == "0.0") {
    digitalWrite(LED13, LOW);
  } else {
    digitalWrite(LED13, HIGH);
  }
  
  Serial.write(payload, length);
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
      client.subscribe(topicSubscribe);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

/****************************************
 * Main Functions
 ****************************************/
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  // Assign the pin as OUTPUT 
  pinMode(LED13, OUTPUT);

  Serial.println();
  Serial.print("Wait for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);

  sprintf(topicSubscribe, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL, VARIABLE_LABEL_SUBSCRIBE);
  
  client.subscribe(topicSubscribe);
}

void loop() {
  if (!client.connected()) {
    reconnect();
    client.subscribe(topicSubscribe);   
  }
  client.loop();
}
