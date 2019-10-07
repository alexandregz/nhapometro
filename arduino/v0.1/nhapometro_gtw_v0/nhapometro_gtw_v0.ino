#include <SPI.h>
#include <RF24.h>

#define CMDBUFFER_SIZE 32

RF24 radio(9, 10);  //CE, CSN

const byte rxAddr[6] = "GTW01";
const byte txAddr[6] = "DPY01";

char read_Buff[CMDBUFFER_SIZE] = {'\0'};
int index_read_Buff = 0;
int cmd_ok = false;

void setup(void){
  Serial.begin(9600);
  Serial.println("Inicando...");

  radio.begin();
  radio.openWritingPipe(txAddr);
  radio.openReadingPipe(1,rxAddr);
  
  radio.startListening();
  //radio.stopListening();
  Serial.print("Estado PA LEVEL: ");
  Serial.println(radio.getPALevel());

  if(radio.isChipConnected() == false){
    Serial.println("No se detecta el chip nrf");
  }else{
    Serial.println("Chip nrf detectado correctamente");
  }
}

void loop() {
  procesarLecturaNRF();

  procesarLecturaSerial();

  procesarEnvioNRF();
}

void procesarLecturaNRF(){
  if (radio.available()){
    char text[32] = {0};
    radio.read(&text, sizeof(text));     
    Serial.println(text);
  }
}

void procesarLecturaSerial(){
  while(Serial.available()){
    char c = Serial.read();  //gets one byte from serial buffer
    if (c == '\n'){ //Full command received. Do your stuff here!
      read_Buff[index_read_Buff++] = '\0';
      cmd_ok = true;
    }else if(c == '\r'){
      read_Buff[index_read_Buff] = read_Buff[index_read_Buff];
    }else{
      //Serial.print(c);
      read_Buff[index_read_Buff++] = c;
    }
  }

  if(index_read_Buff >= CMDBUFFER_SIZE){ //sobrepasado
    clearReadBuff();
  }
}

void clearReadBuff(){
  index_read_Buff = 0;
  cmd_ok = false;
  for(int i=0; i++; i<CMDBUFFER_SIZE){
    read_Buff[i] = '\0';
  }
}

void procesarEnvioNRF(){
  if(cmd_ok){
    radio.stopListening();                                    // First, stop listening so we can talk.

    Serial.print("Vamos enviar: ");
    Serial.println(read_Buff); 

    bool ok = radio.write(&read_Buff, sizeof(read_Buff));
    
    if(ok){
       Serial.println("Datos enviados"); 
    }else{
       Serial.println("Datos no enviados"); 
    }

    radio.startListening();                                    // Now, continue listening
    
    clearReadBuff();
  }
}

