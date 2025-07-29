#include "IIKitmini.h"
#include "util/jtask.h"
#include <math.h>

#define PERIODO 100   //Em ms 100ms

byte count = 0;

//Função que le os valores dos POT e das Entradas 4 a 20 mA e plota no display
void managerInputFunc(void) {
    const uint16_t vlPOT1 = analogRead(def_pin_ADC1);
    const uint16_t vlPOT2 = analogRead(def_pin_ADC2);    
    IIKit.disp.setText(2, ("P1:" + String(vlPOT1)).c_str());
    IIKit.disp.setText(3, ("P2:" + String(vlPOT2)).c_str());    
    IIKit.WSerial.plot("vlPOT1", vlPOT1);
    IIKit.WSerial.plot("vlPOT2", vlPOT2);
}

void setup() // Codigo de configuração
{
  Serial.begin(19200);
  IIKit.setup(); 
  jtaskAttachFunc(managerInputFunc, 50000UL); //anexa um função e sua base de tempo para ser executada   
  pinMode(def_pin_PWM, OUTPUT);
}

#define TIME_RESOLUTION 1
uint32_t previousTimeMS = 0;
void loop()
{
  IIKit.loop();
  jtaskLoop();
  const uint32_t currentTimeMS = millis();
  if ((currentTimeMS - previousTimeMS) >= TIME_RESOLUTION)
  {
    previousTimeMS = currentTimeMS;
    analogWrite(def_pin_PWM, 127 * sin(2 * PI * count / PERIODO) + 127);
    count = (count + 1) % PERIODO;  
  }
}
