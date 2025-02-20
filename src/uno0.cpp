#include <Arduino.h>
#include <math.h>

#define pinPWM 6                            //Configura o pino de Saida do PWM
#define PERIODO1 100                        //Em us 100ms

byte count1 = 0;

void setup() // Codigo de configuração
{
  Serial.begin(19200);
  pinMode(pinPWM, OUTPUT);
}

#define TIME_DELAY_MS 1
uint32_t previousTimeMS = 0;
void loop()
{
  const uint32_t currentTimeMS = millis();
  if ((currentTimeMS - previousTimeMS) >= TIME_DELAY_MS)
  {
    previousTimeMS = currentTimeMS;
    analogWrite(pinPWM, 127 * sin(2 * PI * count1 / PERIODO1) + 127);
    if (++count1 >= PERIODO1) count1 = 0;    
  }
}
