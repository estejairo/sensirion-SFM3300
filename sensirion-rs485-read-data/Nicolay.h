/* Nicolay RS485 clip-on cap library.
 * 
 * @author Jairo Gonzalez 
 * @date 21.10.2021
 * 
 */

#ifndef Nicolay_h
#define Nicolay_h


#include <Arduino.h>
#include <SoftwareSerial.h>

class Nicolay{
  public:
    Nicolay(byte slaveAddress, int rxPin, int txPin, int ctrlPin, int ledPin);
    Nicolay() : RS485Serial(rxPin, txPin) {}
    unsigned char testCommand();
  private:
    byte _slaveAddress;
    int _rxPin;
    int _txPin;    
    int _ctrlPin;
    int _ledPin;
    const int RS485_TRANSMIT = HIGH;
    const int RS485_RECEIVE = LOW;
    SoftwareSerial RS485Serial;
};


#endif