#include <LowPower.h>
#include <SPI.h>
#include <RF24.h>

#define INTERRUPTPIN  2

RF24 radio(7, 8);

const byte rxAddr[6] = "GTW01";

void wakeUp()
{
    delay(1);
}

void setup()
{
  randomSeed(analogRead(0));
  
  pinMode(INTERRUPTPIN, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  pinMode(9, OUTPUT);
}

void loop()
{
  durmirNRF24();
  
  // Allow wake up pin to trigger interrupt on low.
  attachInterrupt(digitalPinToInterrupt(INTERRUPTPIN), wakeUp, FALLING);
  
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 

  // Disable external pin interrupt on wake up pin.
  detachInterrupt(digitalPinToInterrupt(INTERRUPTPIN)); 

  //Serial.begin(9600);
  //Serial.println("Despertando...");

  sendPulsador();

  while(digitalRead(INTERRUPTPIN) == LOW){ //reducir rebote
    delay(100);
  }
}

void durmirNRF24(){
  radio.begin();
  delay(2);
  radio.stopListening ();
  delay(2);
  radio.powerDown();
  delay(2);
}

void sendPulsador(){
  digitalWrite(4, HIGH);
  delay(5);

  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(rxAddr);
  
  radio.stopListening ();

  delay(5);
  /*
  if(radio.isChipConnected() == false){
    Serial.println("No se detecta el chip nrf");
  }else{
    Serial.println("Chip nrf detectado correctamente");
  }
  
  Serial.print("Estado PA LEVEL: ");
  Serial.println(radio.getPALevel());
  */
  //radio.printDetails();
  
  unsigned int genCheck = random(9999); //check unico
  char valueString[5];
  sprintf(valueString, "%04d", genCheck);
  
  char text[10] = {'\0'};
  text[0] = 'P';
  text[1] = '0';
  text[2] = '1';
  text[3] = '|';
  text[4] = valueString[0];
  text[5] = valueString[1];
  text[6] = valueString[2];
  text[7] = valueString[3];
  text[8] = '|';
  text[9] = '\0';
  bool ok = radio.write(&text, sizeof(text));
  if(ok == false){ //un intento mas
    ok = radio.write(&text, sizeof(text));
  }

  radio.powerDown();

  delay(5);
  digitalWrite(4, LOW);
  
  if(ok){
    //Serial.println("Datos enviados"); 
    for(char i=0; i<3; i++){
      digitalWrite(9, HIGH);
      delay(50);
      digitalWrite(9, LOW);
      delay(50);
    }
  }/*else{
    Serial.println("Datos no enviados"); 
  }*/
  digitalWrite(9, LOW);
}

