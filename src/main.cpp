#include "Arduino.h"
#include <SoftwareSerial.h>  // https://github.com/PaulStoffregen/SoftwareSerial
#include <PubSubClient.h>  
#include <ESP8266WiFi.h> 

/////////////////////////////////////////////////////////////////////////// Schleifen verwalten22
unsigned long previousMillis_mqttCHECK = 0; // Windstärke prüfen
unsigned long interval_mqttCHECK = 500; 

unsigned long previousMillis_RS485_auslesen = 0; // Windstärke prüfen
unsigned long interval_RS485_auslesen = 1500; 

/////////////////////////////////////////////////////////////////////////// RS485 - Variablen
#define RX        D2    //Serial Receive pin
#define TX        D3    //Serial Transmit pin
#define RTS_pin   D0    //RS485 Direction control
#define RS485Transmit    HIGH
#define RS485Receive     LOW
SoftwareSerial RS485Serial(RX, TX);

/////////////////////////////////////////////////////////////////////////// Variablen
const char* kartenID = "24V_Weihnachten_Lichter";

/////////////////////////////////////////////////////////////////////////// mqtt Client
WiFiClient espClient;
PubSubClient client(espClient);
char buffer1[10];

// Wifi Setup
const char* ssid = "GuggenbergerLinux";
const char* password = "Isabelle2014samira";
const char* mqtt_server = "192.168.150.1";

/////////////////////////////////////////////////////////////////////////// Funktionsprototypen
void loop                       ();
void callback                   (char* topic, byte* payload, unsigned int length);
void reconnect                  ();
void wifi_setup                 ();
void OTA_update                 ();
void setuo                      ();
void mqtt_connected             ();
void RS485_auslesen             ();                      


/////////////////////////////////////////////////////////////////////////// Setup
void setup() {

  pinMode(RTS_pin, OUTPUT);  
  
  // Baud Serielle Kommunikation
  Serial.begin(19200);

  // RS485 Baud
  RS485Serial.begin(4800);   
  delay(500);

  // MQTT Broker
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  wifi_setup();

}

/////////////////////////////////////////////////////////////////////////// SETUP - Wifi
void wifi_setup() {

  // WiFi 
  IPAddress ip(192, 168, 55, 35);
	IPAddress dns(192, 168, 1, 1);  
	IPAddress subnet(255, 255, 0, 0);
	IPAddress gateway(192, 168, 1, 1);
	WiFi.config(ip, dns, gateway, subnet);
  WiFi.begin(ssid, password);

  // Wifi AP deaktivieren
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi connected...");


}

/////////////////////////////////////////////////////////////////////////// VOID mqtt reconnected
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Baue Verbindung zum mqtt Server auf. IP: ");
    // Attempt to connect
    if (client.connect(kartenID,"zugang1","43b4134735")) {
      //Serial.println("connected");
      ////////////////////////////////////////////////////////////////////////// SUBSCRIBE Eintraege
      //client.subscribe("Solarpanel/001/steuerung/sturmschutz");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}

/////////////////////////////////////////////////////////////////////////// MQTT callback
void callback(char* topic, byte* payload, unsigned int length) {
/*
  /////////////////////////////////////////////////////////////////////////// Sturmschutz schalten
      if (strcmp(topic,"Solarpanel/001/steuerung/sturmschutz")==0) {

          // Sturmschutz aktivieren
          if ((char)payload[0] == 'o' && (char)payload[1] == 'n') {  
                client.publish("Solarpanel/001/codemeldung", "Sturmschutz AKTIV");  
                // mqtt_sturmschutz_status - per FHEM aktivieren
                mqtt_sturmschutz_status = 1;

                }
          // Sturmschutz deaktivieren
          if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') {  
                client.publish("Solarpanel/001/codemeldung", "Sturmschutz AUS");
                // mqtt_sturmschutz_status - per FHEM aktivieren
                mqtt_sturmschutz_status = 0;

                }
        } 
*/
}

/////////////////////////////////////////////////////////////////////////// mqtt connected
void mqtt_connected(){

    // mqtt Daten senden     
  if (!client.connected()) {
      reconnect();
    }
    client.loop(); 

}

/////////////////////////////////////////////////////////////////////////// mqtt connected
void RS485_auslesen(){

  byte Wattmeter_buf[8];
  RS485Serial.readBytes(Wattmeter_buf, 8);

  Serial.print("Wattausgabe");
  Serial.print(" -> ");
  Serial.print(Wattmeter_buf[5]);
  Serial.print(" Watt");
  Serial.println(); 

  delay(300);

  dtostrf(Wattmeter_buf[5],2, 1, buffer1); 
  client.publish("Wattmeter/HA/L1", buffer1); 



}

/////////////////////////////////////////////////////////////////////////// Loop
void loop() {

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ mqtt Checken
  if (millis() - previousMillis_mqttCHECK > interval_mqttCHECK) {
      previousMillis_mqttCHECK = millis(); 
      mqtt_connected();
    }    

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ RS485 auslesen
  if (millis() - previousMillis_RS485_auslesen > interval_RS485_auslesen) {
      previousMillis_RS485_auslesen = millis(); 
      RS485_auslesen();
    }  
                 
 

}