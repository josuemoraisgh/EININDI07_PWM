#include <Arduino.h>
#include "IiKit.h"

#define TIME_DELAY 50.0 // Time em milisegundos

void plotWave(void *)
{
  AsyncDelayUS_c delayPlotWave(TIME_DELAY); // time in micro second
  for (;;)
  {
    if (delayPlotWave.isExpired())
    {
      delayPlotWave.repeat();    
      ledcWrite(0,map(analogRead(def_pin_POT1), 0, 4095, 0, 1024));
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

void loop()
{
  IIKit.loop();
}