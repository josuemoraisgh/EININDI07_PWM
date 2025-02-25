#include <Arduino.h>
#include <math.h>

#define PIN_PWM_POS 6      // Saída PWM para a parte positiva
#define PIN_PWM_NEG 5      // Saída PWM para a parte negativa
#define MAX_RESOLUTION 256 // Número máximo de pontos para a metade do ciclo

// Parâmetros padrão
float frequency = 100.0;       // Frequência de ciclo completo em Hz
uint16_t resolution = 100;   // Número de pontos para a metade do ciclo
uint8_t amplitude = 255;     // Amplitude (valor máximo de 0 a 255)

// Tabela que armazena a metade da senoide (de 0 a π)
uint8_t halfSineTable[MAX_RESOLUTION];

// Variáveis de controle do temporizador e fase
volatile uint16_t count = 0;   // Índice atual da tabela
bool positivePhase = true;     // true: fase positiva, false: fase negativa
unsigned long previousTime = 0;
unsigned long timeDelayMicroSec = 0; // Tempo entre cada ponto

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
  timeDelayMicroSec = (unsigned long)((1000000UL / frequency) / 2) / resolution;
  count = 0;
  positivePhase = true;
}

// Processa a entrada serial utilizando switch-case para permitir apenas uma alteração por comando
void processSerialInput() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() < 3 || input.charAt(1) != ':') {
      Serial.println("Comando invalido. Use F:valor, R:valor ou A:valor.");
      return;
    }
    
    char command = input.charAt(0);
    String valueStr = input.substring(2);

    switch (command) {
      case 'F': {
          float newFreq = valueStr.toFloat();
          if (newFreq > 0) {
            frequency = newFreq;
            Serial.print("Nova frequencia: ");
            Serial.println(frequency);
            recalcSineTable();
          } else {
            Serial.println("Frequencia invalida. Deve ser > 0.");
          }
        }
        break;
        
      case 'R': {
          int newRes = valueStr.toInt();
          if (newRes > 0 && newRes <= MAX_RESOLUTION) {
            resolution = newRes;
            Serial.print("Nova resolucao: ");
            Serial.println(resolution);
            recalcSineTable();
          } else {
            Serial.print("Resolucao invalida. Deve ser entre 1 e ");
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
        Serial.println("Comando invalido. Use F:valor, R:valor ou A:valor.");
        break;
    }
  }
}

void setup() {
  Serial.begin(19200);
  Serial.println("Use F:0@99, R:1@256 ou A:1@255.");  
  pinMode(PIN_PWM_POS, OUTPUT);
  pinMode(PIN_PWM_NEG, OUTPUT);
  recalcSineTable();  // Inicializa a tabela com os parâmetros padrão
}

void loop() {
  processSerialInput();  // Verifica e processa comandos via Serial

  unsigned long currentTime = micros();
  if (currentTime - previousTime >= timeDelayMicroSec) {
    previousTime = currentTime;
    
    // Durante a fase positiva, aciona o PWM positivo; durante a negativa, o PWM negativo.
    if (positivePhase) {
      analogWrite(PIN_PWM_POS, halfSineTable[count]);
      analogWrite(PIN_PWM_NEG, 0);
    } else {
      analogWrite(PIN_PWM_NEG, halfSineTable[count]);
      analogWrite(PIN_PWM_POS, 0);
    }
    
    count++;
    // Quando acabar a metade do ciclo, reinicia o índice e alterna a fase.
    if (count >= resolution) {
      count = 0;
      positivePhase = !positivePhase;
    }
  }
}
