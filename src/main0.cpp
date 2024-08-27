#include <Arduino.h>
#include <math.h>

#define pinANALOG A5                        //Configura o pino de leitura

void setup()                              // Codigo de configuração
{
  Serial.begin(19200);
  pinMode(pinANALOG, INPUT);
 
}

void loop() {
  Serial.print(">graf:");
  Serial.print(millis()); 
  Serial.print(":");  
  Serial.print(analogRead(pinANALOG));
  Serial.println("|g");
  delay(10);
}
