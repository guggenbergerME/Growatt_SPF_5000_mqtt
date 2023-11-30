#include <SoftwareSerial.h>
#include <Arduino.h>


// Define TX, RX and RW (read/write) pins
#define RX_PIN D1
#define TX_PIN D2
#define RW_PIN D0

// Define the bytes of the command to send to the energy meter
#define SLAVEID 0x01
#define FUNCCODE 0x03
#define COMM1 0x10
#define COMM2 0x00
#define WORD1 0x00
#define WORD2 0x02

// Initialize SoftwareSerial
SoftwareSerial RS485(RX_PIN, TX_PIN);

// Define the function for the CRC calculation
uint16_t calc_crc(char *ptbuf, unsigned int num);
uint8_t crc1;
uint8_t crc2;

// Define the command to send to the energy meter
char requestData[6] = {SLAVEID, FUNCCODE, COMM1, COMM2, WORD1, WORD2}; //Read from device 0x01 one register from address 0x1027

// Initialize the vector for storing the response from the energy meter
uint8_t responseData[1000];

void setup()
{
  // Set the pinmode of the TX and RX pins and RW(to enable read and write)
  //RW_Pin --> Set  HIGH to write and LOW to receive

  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
  pinMode(RW_PIN, OUTPUT);
  
  // Initialize the baud rate of the connection and the serial monitor
  RS485.begin(9600);
  Serial.begin(9600);

}


void loop()
{
  /*
 // Put the MAX485 into write mode
  Serial.println("Writing process");
  digitalWrite(RW_PIN, HIGH);
  delay(100); //allow RW pin to get high value

  // Calculate the CRC of the command
  uint16_t crc = calc_crc(requestData, 6); //calcolo crc su tutti i byte del messaggio (tranne gli ultimi due che sono del crc)
  Serial.println(crc,HEX);

  // Include the CRC inside the command
  unsigned char requestDatacrc[8] = {SLAVEID, FUNCCODE, COMM1, COMM2, WORD1, WORD2, 0x00, 0x00};
  crc1 = crc >> 8;
  crc2 = crc & 0xFF;
  requestDatacrc[6] = crc1; // Insert the CRC MSB in the request
  requestDatacrc[7] = crc2; // Insert the CRC LSB in the request

  // Send the request to the energy meter
  for (int i = 0; i < sizeof(requestDatacrc); i++)
  {
    RS485.write(requestDatacrc[i]);
    Serial.println(requestDatacrc[i],HEX);
  }
  delay(15);
*/
  // Put the MAX485 into read mode
  digitalWrite(RW_PIN, LOW);
  Serial.println("Reading process");

  RS485.listen(); //listeaning
  int i = 0;

  while (RS485.available() > 0)
  {
    Serial.println("Reading...");    
    responseData[i++] = RS485.read();
    Serial.println(responseData[i],HEX);
  }
  delay(5000);

}

// CRC calculation function
uint16_t calc_crc(char *ptbuf, unsigned int num) 
 { 
  uint16_t crc16; 
  uint16_t temp; 
  unsigned char c, flag; 
  crc16 = 0xffff;
  for (num; num > 0; num--)
  { 
    temp = (uint16_t) *ptbuf;
    temp &= 0x00ff;
    crc16 = crc16 ^ temp;
    for (c = 0; c < 8; c++)
    { 
      flag = crc16 & 0x01;
      crc16 = crc16 >> 1;
      if (flag != 0) 
        crc16 = crc16 ^ 0x0a001;
    } 
    ptbuf++;
  } 
  crc16 = (crc16 >> 8) | (crc16 << 8);
  return (crc16); 
 }