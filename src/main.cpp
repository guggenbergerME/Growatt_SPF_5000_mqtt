#include "Arduino.h"
#include <SoftwareSerial.h>  // https://github.com/PaulStoffregen/SoftwareSerial
#include <ModbusMaster.h>

#define MAX485_TX_ENABLE  D0  // EIA-485 transmit control pin
#define EIA485_RX         D2  // EIA-485 serial receive pin
#define EIA485_TX         D3  // EIA-485 serial transmit pin

ModbusMaster node;
SoftwareSerial RS485Serial(EIA485_RX, EIA485_TX); // Hier wird der serielle Port beschrieben

/////////////////////////////////////////////////////////////////////////// Funktionsprototypen
void loop                       ();
void setup                      ();
void preTransmission            ();
void postTransmission           ();



void preTransmission() {
  digitalWrite(MAX485_TX_ENABLE, true);// Vor der Übertragung wird Pin 7 auf High gesetzt.
}

void postTransmission() {
  digitalWrite(MAX485_TX_ENABLE, false);// Nach der Übertragung wieder auf Low
}

void setup() {
  pinMode(MAX485_TX_ENABLE, OUTPUT);
  digitalWrite(MAX485_TX_ENABLE, false);

  Serial.begin(19200);
  RS485Serial.begin(19200); //Baudrate des Sensors auf 19200

  node.begin(1, RS485Serial);//Start der seriellen Schnittstelle, Slave ID 1
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.print("Elsner TH-AP Modbus");// Bezeichnung meines Sensors
}

void loop() {
  uint8_t result;
//  uint16_t data[6];

  result = node.readInputRegisters(0, 3);//Funktion : Read Input Registers, Modbus Adressen 0 bis 2
  if (result == node.ku8MBSuccess)
  {
    Serial.print("Reg 0: ");
    Serial.println(node.getResponseBuffer(0));//Temperatur, Datentyp laut Handbuch Signed 16Bit

    Serial.print("Reg 1: ");
    Serial.println(node.getResponseBuffer(1));//Feuchte, Datentyp laut Handbuch Signed 16Bit

    Serial.print("Reg 2: ");
    Serial.println(node.getResponseBuffer(2));//Taupunkt, Datentyp laut Handbuch Signed 16Bit
  }
  else {
  Serial.print("Fehler beim Auslesen des Sensors\n");
  }

  delay(2000);
}