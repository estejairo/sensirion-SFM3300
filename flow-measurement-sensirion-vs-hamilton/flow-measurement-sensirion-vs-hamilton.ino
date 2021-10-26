/* This script reads a pressure sensor connected to a Hamilton flow transducer
 * and reads also the output of a sensirion SFM3300 flow meter, to
 * characterization of the Hamilton transducer.
 * 
 * @author Jairo Gonzalez 
 * @date 26.10.2021
 */

//===============================================================================
//  Includes
//==============================================================================
#include <SoftwareSerial.h>
#include "Nicolay.h"
#include "Crc8.h"
#include <SPI.h>

//==============================================================================
//  Pressure sensor defines (Hamilton transducer)
//==============================================================================
#define NULL_BYTE 0x00
#define press_max 2
#define press_min -2
float pressureHamilton;

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
  Serial.println("Type in upper window, press ENTER to start the acquisition.");
  float pressureHamilton;
}


//===============================================================================
//  Main loop
//===============================================================================
void loop() 
{

  if (Serial.available())         // A char(byte) has been entered in the Serial Monitor
  {
    unsigned long timeElapsed = millis();
    Serial.read();  // Read the byte
    while ((millis()-timeElapsed) < 60000){
      long* startOutput;
      startOutput = sfm3300.getFlowMeasurement();
      if ((*(startOutput+1) != 4) && (*startOutput!=2147483647)){
        pressureHamilton = medir_presion_SPI(press_max, press_min);
        Serial.print(*startOutput);
        Serial.print(",");
        Serial.println(pressureHamilton);
      }
    }
     Serial.println("Done");
  }
}


//==============================================================================
//  Pressure sensor read function (Hamilton transducer)
//  Author: Eduardo Valdivia
//  Date: 2020
//==============================================================================
float medir_presion_SPI(int p_max, int p_min) {

  byte data[2] = {NULL_BYTE, NULL_BYTE};
  float pressure = 0;
  unsigned int pressure_raw = 0;
  byte pressure_mask = 0x3F;
  byte stat = 0x00;

  const int out_min = 1638;
  const int out_max = 14745;

  SPI.beginTransaction (SPISettings (800000, MSBFIRST, SPI_MODE0));

  data[0] = NULL_BYTE;
  data[1] = NULL_BYTE;

  //PORTA &= ~(1 << PA3); // PIN 25 baja
  digitalWrite(25, LOW); // Solo hacer tiempo

  SPI.transfer(data, sizeof(data));
  digitalWrite(25, HIGH); // Solo hacer tiempo
  //PORTA |= (1 << PA3);  // PIN 25 SUBE
  
  stat = data[0] >> 6;
  pressure_raw = ((data[0] & pressure_mask) << 8) + data[1];
  //pressure = 0.1 * ((((float)pressure_raw - out_min) * (p_max - p_min) * 0.0000762951) + p_min);

  return pressure_raw;
}


