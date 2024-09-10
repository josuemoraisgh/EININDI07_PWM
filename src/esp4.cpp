#include <Arduino.h>
#include "IiKit.h"

#define FREQ1 110.0
#define FREQ2 10.0

#define TIME_DELAY 100.0 // Time em milisegundos

#define CILCE_FREQ1 (FREQ1 * TIME_DELAY / 1000000.0)
#define CILCE_PERIODO1 (1.0 / CILCE_FREQ1)
#define CILCE_FREQ2 (FREQ2 * TIME_DELAY / 1000000.0)
#define CILCE_PERIODO2 (1.0 / CILCE_FREQ2)

xQueueHandle interputQueue = xQueueCreate(10, sizeof(uint16_t *));

void plotWave(void *)
{
  uint16_t timeWave1 = 0;                          // Índice para percorrer a tabela de formas de onda
  uint16_t timeWave2 = 0;   
  AsyncDelay_c delayPlotWave(TIME_DELAY, ISMICROS); // time in micro second
  for (;;)
  {
    if (delayPlotWave.isExpired())
    {
      delayPlotWave.repeat();
      const double aux = 1.0 + 0.3*sin(2.0 * PI * CILCE_FREQ1 * timeWave1) + 0.7*sin(2.0 * PI * CILCE_FREQ2 * timeWave2);
      ledcWrite(0, uint16_t(511.0*aux));
      dacWrite(def_pin_DAC1, uint8_t(127.0*aux));
      
      const uint16_t value = analogRead(def_pin_ADC1);
      xQueueSend(interputQueue, &value, 0);
      
      if (++timeWave1 >= CILCE_PERIODO1) timeWave1 = 0;
      if (++timeWave2 >= CILCE_PERIODO2) timeWave2 = 0;      
    }
  }
}

void setup()
{
  IIKit.setup();
  ledcAttachPin(def_pin_PWM,0);
  ledcSetup(0, 1000, 10);
  xTaskCreate(plotWave,"Task Wave",5000,NULL,1,NULL);
}

uint32_t count = 0;
uint16_t pvBuffer = 0;

void loop()
{
  IIKit.loop();
  if(xQueueReceive(interputQueue, &pvBuffer, 0))
  {
    IIKit.WSerial.plot("Data",count++, pvBuffer);
  }  
}