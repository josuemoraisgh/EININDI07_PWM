#include <Arduino.h>
#include <math.h>

#define PIN_PWM_POS 6      // Saída PWM para a parte positiva
#define PIN_PWM_NEG 5      // Saída PWM para a parte negativa
#define MAX_RESOLUTION 256 // Número máximo de pontos para a metade do ciclo

// Parâmetros
float frequency = 100.0;               // Frequência de ciclo completo em Hz
uint16_t resolution = 100;             // Número de pontos para a metade do ciclo
uint8_t amplitude = 255;               // Amplitude (valor máximo de 0 a 255)
uint8_t halfSineTable[MAX_RESOLUTION]; // Tabela que armazena a metade da senoide (de 0 a π)

// Variáveis de controle do temporizador e fase
bool positivePhase = true;           // true: fase positiva, false: fase negativa
volatile uint16_t indexStepWave = 0; // Índice atual da tabela
unsigned long stepWaveMicroSec = 0;  // Tempo entre cada ponto

// Recalcula a tabela da metade da senoide e o delay entre os pontos
void recalcSineTable() {
  // Para a metade de um ciclo: ângulo varia de 0 a π.
  // Se houver mais de um ponto, usamos (resolution - 1) para que o último valor se aproxime de π.
  for (uint16_t i = 0; i < resolution; i++) {
    float angle = (resolution > 1) ? (PI * i / (resolution - 1)) : 0;
    halfSineTable[i] = (uint8_t)(amplitude * sin(angle));
  }
  // Período completo = 1/frequency (em segundos)
  // Metade de ciclo dura T/2 = (1 / frequency) / 2 segundos.
  // O delay entre os pontos (em microssegundos) é: (T/2 * 1e6) / resolution.
  stepWaveMicroSec = (unsigned long)((1000000UL / frequency) / 2) / resolution;
  indexStepWave = 0;
  positivePhase = true;
}

void setup() {
  Serial.begin(19200);
  Serial.println("Use F:0@99, R:1@256 ou A:1@255.");  
  pinMode(PIN_PWM_POS, OUTPUT);
  pinMode(PIN_PWM_NEG, OUTPUT);
  recalcSineTable();  // Inicializa a tabela com os parâmetros padrão
}

unsigned long previousTime = 0;
void loop() {
  unsigned long currentTime = micros();
  if (currentTime - previousTime >= stepWaveMicroSec) {
    previousTime = currentTime;
    // Durante a fase positiva, aciona o PWM positivo; durante a negativa, o PWM negativo.
    if (positivePhase) {
      analogWrite(PIN_PWM_POS, halfSineTable[indexStepWave]);
      analogWrite(PIN_PWM_NEG, 0);
    } else {
      analogWrite(PIN_PWM_NEG, halfSineTable[indexStepWave]);
      analogWrite(PIN_PWM_POS, 0);
    }
    indexStepWave = (indexStepWave + 1) % resolution;
    if(indexStepWave == 0) positivePhase = !positivePhase;    // Quando acabar a metade do ciclo, reinicia o índice e alterna a fase.
  }
}