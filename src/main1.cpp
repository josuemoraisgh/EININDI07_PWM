#include <Arduino.h>
#include "IiKit.h"

// 80M / RESOLUÇÃO = FREQUENCIA MÁXIMA

#define TIME_DELAY 10.0 // Time em microsegundos
// #define configTICK_RATE_HZ uint32_t(1.0/(TIME_DELAY/1000000.0))
// #define portTICK_PERIOD_US (1.0/TIME_DELAY)

void plotWave(void *)
{
  AsyncDelayUS_c delayPlotWave(TIME_DELAY); // time in micro second
  for (;;)
  {
    if (delayPlotWave.isExpired())
    {
      delayPlotWave.repeat();
      const uint16_t vlPOT1 = analogRead(def_pin_POT1);  
      dacWrite(def_pin_DAC1,map(vlPOT1, 0, 4095, 0, 255));          
      ledcWrite(0,map(vlPOT1, 0, 4095, 0, 1024));
      //vTaskDelay(10*portTICK_PERIOD_US);
    }
  }
}

void setup()
{
  IIKit.setup();
  pinMode(def_pin_PWM, OUTPUT); //Definimos o pino def_pin_PWM (LED) como saída.
  ledcAttachPin(def_pin_PWM, 0);//Atribuimos o pino def_pin_PWM ao canal 0.
  ledcSetup(0, 1000, 10);       //Atribuimos ao canal 0 a frequencia de 1000Hz com resolucao de 10bits.
  xTaskCreate(
      plotWave,    // Function name
      "Task Wave", // Task name
      5000,        // Stack size
      NULL,        // Task parameters
      1,           // Task priority
      NULL         // Task handle
  );
}

void loop()
{
  IIKit.loop();
}