#include <Arduino.h>
#include <math.h>

#define pinPWM 6
#define MAX_RESOLUTION 256   // Tamanho máximo do vetor de seno

// Parâmetros
float frequency = 5.0;               // Frequência em Hz
uint16_t resolution = 100;           // Número de pontos para um ciclo da senoide
uint8_t amplitude = 255;             // Amplitude (valor máximo de 0 a 255)
uint8_t sineTable[MAX_RESOLUTION];   // Tabela de seno com tamanho fixo
volatile uint16_t indexStepWave = 0; // Índice atual na tabela
unsigned long stepWaveMicroSec = 0;  // Tempo entre pontos (em microssegundos)

// Recalcula os pontos da senoide e o tempo de delay entre eles
void recalcSineTable() {
  for (uint16_t i = 0; i < resolution; i++) {
    float angle = 2.0 * PI * i / resolution;
    // Mapeia a senoide de [-1, 1] para [0, amplitude]
    sineTable[i] = (uint8_t)(((float)amplitude / 2.0) * sin(angle) + ((float)amplitude / 2.0));
  }
  // Calcula o delay entre os pontos:
  // (Período do ciclo em microssegundos) dividido pelo número de pontos
  stepWaveMicroSec = (unsigned long)(1000000UL / frequency) / resolution;
  indexStepWave = 0; // Reinicia o índice da tabela
}

void setup() {
  Serial.begin(19200);
  pinMode(pinPWM, OUTPUT);
  recalcSineTable();  // Inicializa a tabela com os valores padrão
}

unsigned long previousTime = 0;
void loop() {
  unsigned long currentTime = micros();
  if (currentTime - previousTime >= stepWaveMicroSec) {
    previousTime = currentTime;
    analogWrite(pinPWM, sineTable[indexStepWave]);
    indexStepWave = (indexStepWave + 1) % resolution;
  }
}
