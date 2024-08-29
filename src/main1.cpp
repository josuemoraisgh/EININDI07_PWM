#include <Arduino.h>
#include <math.h>

// Pinos para o PWM
#define pinLED1 13  //Configura o pino de Saida do LED
#define PINPOS 6
#define PINNEG 5
// Valores de Funcionamento
#define FRE 15.0
#define T (1.0 / FRE)
#define DELAY_US round((T/1020.0)*1000000.0)

int count = 0;
unsigned long previousMicros = 0;  // will store last time LED was updated

// Vetor de 256 posições contendo às amplitudes do PWM variando de forma senoidal
const int seno[] = {
0, 5, 11, 16, 20, 24, 27, 30, 32, 32, 33, 32, 31, 29, 27, 25, 22, 19, 17, 15, 14, 13, 13, 13, 15, 17, 20, 23, 28, 32, 38, 43, 48, 54, 59, 63, 68, 71, 74, 76, 77, 78, 77, 76, 75, 73, 70, 68, 65, 63, 60, 59, 57, 57, 57, 58, 60, 62, 66, 70, 74, 79, 84, 90, 95, 100, 105, 109, 113, 116, 118, 119, 120, 120, 119, 118, 116, 113, 111, 108, 105, 103, 101, 99, 98, 98, 99, 100, 102, 105, 109, 113, 117, 122, 128, 133, 138, 143, 147, 151, 154, 156, 158, 159, 159, 158, 157, 155, 153, 150, 147, 144, 142, 139, 137, 136, 135, 135, 136, 138, 140, 143, 147, 151, 156, 161, 166, 171, 176, 180, 184, 187, 190, 192, 193, 193, 192, 191, 189, 187, 184, 181, 178, 175, 172, 170, 168, 167, 166, 167, 168, 170, 172, 176, 179, 184, 188, 193, 198, 203, 207, 211, 214, 217, 219, 220, 220, 220, 218, 217, 214, 211, 208, 205, 202, 198, 196, 193, 192, 191, 191, 191, 192, 194, 197, 201, 205, 209, 213, 218, 222, 227, 230, 234, 236, 238, 240, 240, 240, 238, 237, 234, 231, 228, 224, 221, 217, 214, 211, 209, 208, 207, 207, 208, 209, 211, 214, 218, 221, 226, 230, 234, 238, 242, 245, 248, 250, 251, 252, 251, 250, 248, 246, 243, 239, 236, 232, 228, 225, 221, 219, 217, 215, 214, 215, 215, 217, 219, 222, 226, 230, 234, 238, 242, 245, 248
};

void AnalogWriteFunc() {
  switch(count/255){
    case 0:
      analogWrite(PINPOS,seno[count]);
      break;
    case 1:
      analogWrite(PINPOS,seno[510-count]);    
      break;
    case 2:
      analogWrite(PINNEG,seno[count-510]);      
      break;
    case 3:
      analogWrite(PINNEG,seno[1020-count]);    
      break;      
  }
  if(count >= 1020) count = 1;
  else count++;
}

void setup() {
  Serial.begin(19200);
  pinMode(PINPOS, OUTPUT);
  pinMode(PINNEG, OUTPUT);
  pinMode(pinLED1, OUTPUT);            //Habilita interrupções globais
  analogWrite(PINNEG, 0);
  analogWrite(PINPOS, 0);
}

void loop() {
  unsigned long currentMicros = micros();
  if(((currentMicros - previousMicros >= 0) && (currentMicros - previousMicros >= DELAY_US)) ||
     ((currentMicros - previousMicros <  0) && (currentMicros - 0 + 16383 - previousMicros >= DELAY_US))) {
      previousMicros = currentMicros;
     AnalogWriteFunc();
  } 
}