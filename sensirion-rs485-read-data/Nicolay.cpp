#include "Arduino.h"
#include <SoftwareSerial.h>
#include "Nicolay.h"



//===============================================================================
//  CRC Cheksum variables
//==============================================================================
#define POLYNOMIAL 0x131

typedef enum{
  CHECKSUM_ERROR = 0x04
}etError;



//==============================================================================
//  Nicolay Class methods
//==============================================================================

Nicolay::Nicolay(byte slaveAddress, int rxPin, int txPin, int ctrlPin, int ledPin){
    RS485Serial(rxPin, txPin);
    pinMode(ctrlPin, OUTPUT); 
    digitalWrite(ctrlPin, RS485_RECEIVE); 
    RS485Serial.begin(115200);
    _slaveAddress = slaveAddress;
    _rxPin = rxPin;
    _txPin = txPin;    
    _ctrlPin = ctrlPin;
    _ledPin = ledPin;
}

unsigned char Nicolay::testCommand(){
    byte bToWrite[3] = {_slaveAddress,0x05,0x00};
    byte byteReceived[6];
    unsigned char checksum = SMF3000_CalculateCrc (bToWrite[3], 3);
    digitalWrite(_ctrlPin, RS485_TRANSMIT); // Put RS485 in Transmit mode
    RS485Serial.write({bToWrite,checksum},4); // Send bytes to slave
    digitalWrite(_ctrlPin, RS485_RECEIVE); // Put RS485 back into Receive mode
    delay(10);

    if (RS485Serial.available())            //Data from the Slave is available
    {
    for (int i = 0; i<6; i++){
      digitalWrite(_ledPin, HIGH);          // Show activity on LED
      byteReceived[i] = RS485Serial.read();    // Read received byte
      delay(10);
      digitalWrite(_ledPin, LOW);           // Turn LED back off
     }
    unsigned char crc_calculation = SMF3000_CheckCrc(byteReceived, 5, byteReceived[5]);
    return crc_calculation;
   }
  
}

//===============================================================================
//  CRC-8 Cheksum function
//===============================================================================
#define POLYNOMIAL 0x131 //P(x)=x^8+x^5+x^4+1 = 100110001
//============================================================
unsigned char SMF3000_CheckCrc (unsigned char data[], unsigned char nbrOfBytes, unsigned char checksum)
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
  unsigned char crc = 0;
  unsigned char byteCtr;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  {
      crc ^= (data[byteCtr]);
      for (unsigned char bit = 8; bit > 0; --bit)
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

//===============================================================================
//  CRC-8 Cheksum calculation function
//===============================================================================
#define POLYNOMIAL 0x131 //P(x)=x^8+x^5+x^4+1 = 100110001
//============================================================
unsigned char SMF3000_CalculateCrc (unsigned char data[], unsigned char nbrOfBytes){
  unsigned char crc = 0;
  unsigned char byteCtr;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  {
      crc ^= (data[byteCtr]);
      for (unsigned char bit = 8; bit > 0; --bit)
      {
        if (crc & 0x80) 
          crc = (crc << 1) ^ POLYNOMIAL;
        else 
          crc = (crc << 1);
    }
  }
  return crc;
}
