#include <Arduino.h>
#include <math.h>

#define pinPWM 6                            //Configura o pino de Saida do PWM
#define PERIODO1 100                        //Em us 100ms
#define PERIODO2 16                         //Em us 100ms

uint32_t timeDelayMS = 1;
uint32_t expiresDelayMS = 0;
byte count1 = 0;
byte count2 = 0;

void setup() // Codigo de configuração
{
  Serial.begin(19200);
  pinMode(pinPWM, OUTPUT);
}

void loop()
{
  if ((millis() - expiresDelayMS) >= timeDelayMS)
  {
    expiresDelayMS = millis();
    //analogWrite(pinPWM,100*sin(2*PI*count1/PERIODO1)+27*sin(2*PI*count2/PERIODO2)+127);
    analogWrite(pinPWM, 127 * sin(2 * PI * count1 / PERIODO1) + 127.5);
    if (++count1 >= PERIODO1) count1 = 0;
    if (++count2 >= PERIODO2) count2 = 0;    
  }
}
