#include <Arduino.h>

#ifdef ESP8266
	#include <ESP8266WiFi.h>
#else
	#include <WiFi.h>
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>

//QIoT Configuration

#define USER_NAME "your qiot things name"
#define USER_PASS "your qiot things password"
#define CLIENT_ID "your qiot things client_id"
#define server "yourdomain.myqnapcloud.com" //or your local ip
#define topic_upload "your qiot topic for upload"
#define topic_subscribe "your qiot topic for subscribe"
#define port 21883

//QIoT Configuration end


#define event_lap 1*1000  //run MQTT_NODE ervery 1000ms

//WiFi Configuration
char ssid[] = "your SSID";
char pass[] = "your WiFi password";
//WiFi Configuration end

WiFiClient client;
PubSubClient MQTT_NODE(client);


int status = WL_IDLE_STATUS;

uint32_t current_time;

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");

  Serial.print(topic);

  Serial.print("] ");

  for (int i = 0; i < length; i++) {

    Serial.print((char)payload[i]);

  }

  Serial.println();


}

void reconnect() {
  // Loop until we're reconnected
  while (!MQTT_NODE.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (MQTT_NODE.connect(CLIENT_ID, USER_NAME, USER_PASS )) {
      MQTT_NODE.subscribe(topic_subscribe);
      Serial.println("connected");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(MQTT_NODE.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    delay(15000);
  }
  Serial.println("Connected to wifi");
  MQTT_NODE.setServer(server, port);
  MQTT_NODE.setCallback(callback);
  delay(1500);
}

void loop(){
  if(millis()-current_time>event_lap) {
    current_time = millis();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& Hello = jsonBuffer.createObject();
    JsonObject& value = Hello.createNestedObject("value");
    value["SSID"] = WiFi.SSID();
    value["RSSI"] = WiFi.RSSI();
    value["ADC"] = analogRead(A0);

    if (!MQTT_NODE.connected()) {
      reconnect();
    }
    else{
      char payload[80];
      Hello.printTo(payload);
      MQTT_NODE.publish(topic_upload,payload);
      delay(100);
    }

  }
  MQTT_NODE.loop();
  delay(10);


}
