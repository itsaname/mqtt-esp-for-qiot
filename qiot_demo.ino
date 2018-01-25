#include <Arduino.h>
#ifdef ESP8266
	#include <ESP8266WiFi.h>
#else
	#include <WiFi.h>
#endif

#include <PubSubClient.h>
#define USER_NAME "your name here"
#define USER_PASS "your pass here"
#define CLIENT_ID "your client_id here"
#define server "yourname.myqnapcloud.com"
#define port 21883
#define event_lap 1000 //upload every 1000ms
char ssid[] = "your ssid";
char pass[] = "your wifi pass";
WiFiClient client;
PubSubClient upload(client);
int status = WL_IDLE_STATUS;
char data_ADC[80];
char data_SSID[80];
char data_RSSI[80];
char data_Hello[80];
uint32_t current_time;

void reconnect() {
  // Loop until we're reconnected
  while (!upload.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
   if (upload.connect(CLIENT_ID, USER_NAME, USER_PASS )) {
      Serial.println("connected");    }
    else {
             Serial.print("failed, rc=");
             Serial.print(upload.state());
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
    upload.setServer(server, port);
    delay(1500);
}

void loop(){

if(millis()-current_time>event_lap){
  current_time = millis();

    String SSID = '\"' + WiFi.SSID() + '\"';  //for String add " to void error
    String payload_SSID =  "{\"value\":{\"SSID\":" + SSID + "}}";
    payload_SSID.toCharArray(data_SSID, (payload_SSID.length() + 1));

    String RSSI= String(WiFi.RSSI());
    String payload_RSSI = "{\"value\":{\"RSSI\":"+ RSSI+"}}";
    payload_RSSI.toCharArray(data_RSSI, (payload_RSSI.length() + 1));

    String ADC= String(analogRead(A0));
    String payload_ADC = "{\"value\":{\"ADC\":"+ADC+"}}";
    payload_ADC.toCharArray(data_ADC, (payload_ADC.length() + 1));

    String payload_Hello = "{\"value\":{\"SSID\":"+SSID+" , \"RSSI\":"+ RSSI+"  , \"ADC\":"+ADC+"}}";
    payload_Hello.toCharArray(data_Hello, (payload_Hello.length() + 1));

    if (!upload.connected()) {
      reconnect();
  }
    else{
      upload.publish("your topic 1",data_SSID); //change topict to fit your server
      upload.publish("your topic 2",data_RSSI);
      upload.publish("your topic 3",data_ADC);
      upload.publish("your topic 4",data_Hello);
      delay(100);
    }

  }
    upload.loop();
    delay(10);


  }
