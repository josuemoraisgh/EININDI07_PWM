#include <Arduino.h>
#include <math.h>

#define pinPWM 6         // Pino de saída do PWM
#define PERIODO 100      // Número de pontos para um ciclo completo da senoide

byte count = 0;
uint8_t sineTable[PERIODO];

#define TIME_DELAY_MS 1
uint32_t previousTimeMS = 0;

void setup() {
  Serial.begin(19200);
  pinMode(pinPWM, OUTPUT);

  // Pré-calcula todos os pontos de um ciclo da senoide e os armazena no vetor
  for (int i = 0; i < PERIODO; i++) {
    float angle = 2.0 * PI * i / PERIODO;
    sineTable[i] = (uint8_t)(127 * sin(angle) + 127); // Mapeia a senoide de [-1, 1] para [0, 255]
  }
}

void loop() {
  uint32_t currentTimeMS = millis();
  if ((currentTimeMS - previousTimeMS) >= TIME_DELAY_MS) {
    previousTimeMS = currentTimeMS;
    analogWrite(pinPWM, sineTable[count]);
    count = (count + 1) % PERIODO;    
  }
}
