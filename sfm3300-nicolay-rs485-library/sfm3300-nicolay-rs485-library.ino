/* Demonstrative script for Sensirion SMF3300 with Nicolay RS485 clip-on cap.
 * 
 * @author Jairo Gonzalez 
 * @date 21.10.2021
 */

//===============================================================================
//  Includes
//==============================================================================
#include <SoftwareSerial.h>
#include "Nicolay.h"
#include "Crc8.h"


// typedef enum{
//   CHECKSUM_ERROR = 0x04
// }etError;


//==============================================================================
//  RS485 variables
//==============================================================================
const int SSERIAL_RX_PIN = 10;  //Soft Serial Receive pin
const int SSERIAL_TX_PIN = 11;  //Soft Serial Transmit pin
const int SSERIAL_CTRL_PIN= 3;   //RS485 Direction control
const int LED_PIN = 13;
const int SLAVE_ADDR = 1;

// Create Nicolay object and define pins to use
Nicolay sfm3300(SLAVE_ADDR, SSERIAL_RX_PIN, SSERIAL_TX_PIN, SSERIAL_CTRL_PIN, LED_PIN);


//===============================================================================
//  Script Initialization
//===============================================================================
void setup()
{
  Serial.begin(115200);           // Start the built-in serial port
  Serial.println("Master Device");
  Serial.println("Type in upper window, press ENTER. This script will send Get Flow Command to slave.");
}


//===============================================================================
//  Main loop
//===============================================================================
void loop() 
{

  if (Serial.available())         // A char(byte) has been entered in the Serial Monitor
  {
    Serial.read();  // Read the byte
    unsigned long startTime = millis();


    unsigned long* articleNo = sfm3300.getArticleNo();
    while( *(articleNo+3) == 4){
      Serial.println("Cheksum failed. Not starting yet.");
      articleNo = sfm3300.getArticleNo();
    }
    Serial.print("\n-------------------------------\nSensirion SFM-3300 | Article Number: ");
    Serial.print(*(articleNo+2));
    Serial.print("-");
    Serial.print(*(articleNo+1));
    Serial.print("-");
    Serial.println(*(articleNo));
    Serial.print("-------------------------------\n");


    while (sfm3300.start()==4){
      Serial.println("Cheksum failed. Not starting yet.");
    }
    Serial.println("Starting data acquisition.");

    unsigned long timeElapsed = millis()-startTime;
    while (timeElapsed < 60000){
      long* startOutput;
      startOutput = sfm3300.getFlowMeasurement();
      if ((*startOutput==2147483647)){ //0x7FFFFFFF Data not readable
        Serial.println("Data not readable, please perform a hardware reset.");
      }
      else if((*(startOutput+1) != 4)){
        Serial.println(*startOutput);
      }
      timeElapsed = millis()-startTime;
    }
    Serial.print("Done. Time elapsed (ms):");
    Serial.println(timeElapsed);
  }
}


