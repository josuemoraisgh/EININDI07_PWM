#include <Arduino.h>
#include "IiKit.h"

#define FREQ1 110.0
#define FREQ2 10.0

#define TIME_DELAY1 100.0 // Time em microsegundos

#define CILCE_FREQ1 (FREQ1 * TIME_DELAY1 / 1000000.0)
#define CILCE_PERIODO1 (1.0 / CILCE_FREQ1)
#define CILCE_FREQ2 (FREQ2 * TIME_DELAY1 / 1000000.0)
#define CILCE_PERIODO2 (1.0 / CILCE_FREQ2)

void writeWave(void *)
{
  uint16_t timeWave1 = 0; // Índice para percorrer a tabela de formas de onda
  uint16_t timeWave2 = 0;
  AsyncDelay_c delayWriteWave(TIME_DELAY1, ISMICROS); // time in micro second
  for (;;)
  {
    if (delayWriteWave.isExpired())
    {
      delayWriteWave.repeat();
      const double aux = 1.0 + 0.3 * sin(2.0 * PI * CILCE_FREQ1 * timeWave1) + 0.7 * sin(2.0 * PI * CILCE_FREQ2 * timeWave2);
      ledcWrite(0, uint16_t(511.0 * aux));
      dacWrite(def_pin_DAC1, uint8_t(127.0 * aux));

      if (++timeWave1 >= CILCE_PERIODO1)
        timeWave1 = 0;
      if (++timeWave2 >= CILCE_PERIODO2)
        timeWave2 = 0;
    }
  }
}


#define TIME_DELAY2 1.0 // Time em milisegundos
void readWave(void *)
{
  AsyncDelay_c delayReadWave(TIME_DELAY2, ISMILI); // time in mili second  
  for (;;)
  {
    if (delayReadWave.isExpired())
    {
      delayReadWave.repeat();
      IIKit.WSerial.plot("Data", analogRead(def_pin_ADC1));
    }
  }
}

void setup()
{
  IIKit.setup();
  ledcAttachPin(def_pin_PWM, 0);
  ledcSetup(0, 1000, 10);
  xTaskCreate(writeWave, "Task Wave", 5000, NULL, 1, NULL);
  xTaskCreate(readWave, "Task Wave", 5000, NULL, 1, NULL);  
}

void loop()
{
  IIKit.loop();
}