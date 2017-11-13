#include "ADCboost.h"
#include "Streaming.h"

//int x;
//int t1,t2;
//int i=1;
uint16_t y[800];
static int L=800;

void setup() {
  Serial.begin(115200);
  pinMode(A0,INPUT);

  //ADC_set_reference(01); //01 è pari al default VCC
  ADC_init(0);
}

//frequenza di sampling: 539 Ksps

void loop() {
  //t1=micros();
  for(uint16_t i=0;i<L;i++){
  y[i]=ADC_read();
  }
  //t2=micros();
  //Serial.println(t2-t1);
  //Serial.println(x);
  Serial.println("start"); // rimuovere per  matlab
  delay(1);
  for(uint16_t i=0;i<L;i++){
  Serial.println(y[i]);
  //Serial<< y[i] << "" << "\n";
  delayMicroseconds(1000); 
  }
  delay(1);

}

//float voltage= sensorValue * (5.0 / 1023.0);
