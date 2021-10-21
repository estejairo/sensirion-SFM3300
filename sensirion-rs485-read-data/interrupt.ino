#include "sampling.h"
#include <SPI.h>

#define NULL_BYTE 0x00

#define press_max 2
#define press_min -2

//#define out_max 14745
//#define out_min 1638

#define A1 20       //area mayor en m^2
#define A2 15       //area menor en m^2

volatile float press_read;
float volatile press_final;



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



void setup(){

  Serial.begin(9600);
  
  SPI.begin();

  DDRA |= (1 << DDA3);
  PORTA |= (1 << PA3);  // PIN 25 SUBE
    
  
  noInterrupts();                 // Paramos todas las interrupciones antes de contigurar un timer
  TCCR1A = 0;                     // El registro de control A queda todo en 0
  TCCR1B = 0;                     // Activamos el modo CTC en Timer1
  TCCR1B |= (1 << WGM12); 
  TCCR1B |= (1 << CS12) | (1 << CS10); //prescaler 1024
  TCNT1 = 0;                      // Inicializamos el contador en 0
  OCR1A = 256;                    //16000 para 1KHz
  TIMSK1 |= (1 << OCIE1A);        // Inicializamos el comparador para el registro A
  interrupts();                   // Activamos interrupciones nuevamente
}



ISR(TIMER1_COMPA_vect) {
  //Serial.println(measure(press_max, press_min),6);
  //press_measure(press_max, press_min);
  Serial.println(medir_presion_SPI(2, -2));
}

void loop() {  
  //PORTA &= ~(1 << PA3); // PIN 25 baja
  //delay(2000);
  //PORTA |= (1 << PA3);  // PIN 25 SUBE
  //delay(2000);
}
