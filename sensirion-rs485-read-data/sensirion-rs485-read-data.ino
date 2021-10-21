/* Data Reading script for Sensirion SMF3300 with Nicolay RS485 clip-on cap.
 * 
 * Author: Jairo Gonzalez 
 * Date:        21.10.2021
 */

//===============================================================================
//  Includes
//==============================================================================
#include <SoftwareSerial.h>


//===============================================================================
//  RS485 variables
//==============================================================================
const int SSERIAL_RX_PIN = 10;  //Soft Serial Receive pin
const int SSERIAL_TX_PIN = 11;  //Soft Serial Transmit pin
const int SSERIAL_CTRL_PIN= 3;   //RS485 Direction control
const int LED_PIN = 13;
const int RS485_TRANSMIT = HIGH;
const int RS485_RECEIVE = LOW;

// Create Soft Serial Port object and define pins to use
SoftwareSerial RS485Serial(SSERIAL_RX_PIN, SSERIAL_TX_PIN); // RX, TX

byte byteReceived[6];
byte buff[4] = {0x01,0x05,0x00,0x31};
int i;


//===============================================================================
//  CRC Cheksum variables
//==============================================================================
#define POLYNOMIAL 0x131

typedef enum{
  CHECKSUM_ERROR = 0x04
}etError;

typedef unsigned char u8t;

//Checksum placeholder
int crc_calculation;
int checksum = 0x7D;



//===============================================================================
//  Script Initialization
//===============================================================================
void setup()
{
  Serial.begin(115200);           // Start the built-in serial port
  Serial.println("Master Device");
  Serial.println("Type in upper window, press ENTER. This script will send test command to slave.");
  
  pinMode(LED_PIN, OUTPUT);     // Configure any output pins
  pinMode(SSERIAL_CTRL_PIN, OUTPUT);    
  
  digitalWrite(SSERIAL_CTRL_PIN, RS485_RECEIVE);  // Put RS485 in receive mode  
  
  RS485Serial.begin(115200);   // Start the RS485 soft serial port
}


//===============================================================================
//  Main loop
//===============================================================================
void loop() 
{

  if (Serial.available())         // A char(byte) has been entered in the Serial Monitor
  {
    Serial.read();  // Read the byte
    digitalWrite(SSERIAL_CTRL_PIN, RS485_TRANSMIT);  // Put RS485 in Transmit mode
    Serial.println("A test command has been sent.");
    RS485Serial.write(buff,4);                 // Send bytes to Remote Arduino
    digitalWrite(SSERIAL_CTRL_PIN, RS485_RECEIVE);   // Put RS485 back into Receive mode    
  }
  
  if (RS485Serial.available())            //Data from the Slave is available
   {
    for (i = 0; i<6; i++){
      digitalWrite(LED_PIN, HIGH);          // Show activity on LED
      byteReceived[i] = RS485Serial.read();    // Read received byte
      Serial.println(byteReceived[i], HEX);
      delay(10);
      digitalWrite(LED_PIN, LOW);           // Turn LED back off
     }
     crc_calculation = SMF3000_CheckCrc(byteReceived, 6, checksum);
     if (crc_calculation == CHECKSUM_ERROR){
       Serial.println("CRC-Check Error.");
     }
     else{
       Serial.println("CRC-Check OK.");
     }
   }

}


//===============================================================================
//  CRC-8 Cheksum function
//===============================================================================
#define POLYNOMIAL 0x131 //P(x)=x^8+x^5+x^4+1 = 100110001
//============================================================
u8t SMF3000_CheckCrc (u8t data[], u8t nbrOfBytes, u8t checksum)
//============================================================
//calculates checksum for n bytes of data
//and compares it with expected checksum
//input: data[] checksum is built based on this data
// nbrOfBytes checksum is built for n bytes of data
// checksum expected checksum
//return: error: CHECKSUM_ERROR = checksum does not match
// 0 = checksum matches
//
// Source: Sensirion CRC Checksum application note.
// https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/5_Mass_Flow_Meters/Application_Notes/Sensirion_GF_AN_SFM-04_CRC_Checksum_D1.pdf
//============================================================
{
  u8t crc = 0;
  u8t byteCtr;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  {
      crc ^= (data[byteCtr]);
      for (u8t bit = 8; bit > 0; --bit)
      {
        if (crc & 0x80) 
          crc = (crc << 1) ^ POLYNOMIAL;
        else 
          crc = (crc << 1);
    }
  }
  if (crc != checksum)
  {
    return CHECKSUM_ERROR;
  }
  else
  {
    return 0;
  }
}

