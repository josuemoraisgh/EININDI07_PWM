#include <Arduino.h>
#include <math.h>

#define pinPWM 6
#define MAX_RESOLUTION 256   // Tamanho máximo do vetor de seno

// Parâmetros padrão
float frequency = 100.0;       // Frequência em Hz
uint16_t resolution = 100;   // Número de pontos para um ciclo da senoide
uint8_t amplitude = 255;     // Amplitude (valor máximo de 0 a 255)

// Tabela de seno com tamanho fixo
uint8_t sineTable[MAX_RESOLUTION];

volatile uint16_t count = 0; // Índice atual na tabela
unsigned long previousTime = 0;
unsigned long timeDelayMicroSec = 0;  // Tempo entre pontos (em microssegundos)

// Recalcula os pontos da senoide e o tempo de delay entre eles
void recalcSineTable() {
  for (uint16_t i = 0; i < resolution; i++) {
    float angle = 2.0 * PI * i / resolution;
    // Mapeia a senoide de [-1, 1] para [0, amplitude]
    sineTable[i] = (uint8_t)(((float)amplitude / 2.0) * sin(angle) + ((float)amplitude / 2.0));
  }
  // Calcula o delay entre os pontos:
  // (Período do ciclo em microssegundos) dividido pelo número de pontos
  timeDelayMicroSec = (unsigned long)(1000000UL / frequency) / resolution;
  count = 0; // Reinicia o índice da tabela
}

// Processa a entrada serial utilizando switch-case para permitir apenas uma alteração por comando
void processSerialInput() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() < 3 || input.charAt(1) != ':') {
      Serial.println("Comando inválido. Use F:valor, R:valor ou A:valor.");
      return;
    }
    
    char command = input.charAt(0);
    String valueStr = input.substring(2);

    switch (command) {
      case 'F': {
          float newFreq = valueStr.toFloat();
          if (newFreq > 0) {
            frequency = newFreq;
            Serial.print("Nova frequência: ");
            Serial.println(frequency);
            recalcSineTable();
          } else {
            Serial.println("Frequência inválida. Deve ser > 0.");
          }
        }
        break;
        
      case 'R': {
          int newRes = valueStr.toInt();
          if (newRes > 0 && newRes <= MAX_RESOLUTION) {
            resolution = newRes;
            Serial.print("Nova resolução: ");
            Serial.println(resolution);
            recalcSineTable();
          } else {
            Serial.print("Resolução inválida. Deve ser entre 1 e ");
            Serial.println(MAX_RESOLUTION);
          }
        }
        break;
        
      case 'A': {
          int newAmp = valueStr.toInt();
          amplitude = (uint8_t)constrain(newAmp, 0, 255);
          Serial.print("Nova amplitude: ");
          Serial.println(amplitude);
          recalcSineTable();
        }
        break;
        
      default:
        Serial.println("Comando inválido. Use F:valor, R:valor ou A:valor.");
        break;
    }
  }
}

void setup() {
  Serial.begin(19200);
  pinMode(pinPWM, OUTPUT);
  recalcSineTable();  // Inicializa a tabela com os valores padrão
}

void loop() {
  processSerialInput();  // Verifica se há comando na Serial

  unsigned long currentTime = micros();
  if (currentTime - previousTime >= timeDelayMicroSec) {
    previousTime = currentTime;
    analogWrite(pinPWM, sineTable[count]);
    count = (count + 1) % resolution;
  }
}
