#include <FastLED.h>

#include <SPI.h>
#include <RF24.h>

#include "pitches.h"

// How many NeoPixels are attached to the Arduino?
#define NUM_LEDS      84
// Digital In (DI) of RGB Stick connected to pin 8 of the UNO
#define LED_PIN         6 

CRGB leds[NUM_LEDS]; // FastLED Library Init

RF24 radio(7, 8); //RF24 radio(9, 10);

const byte rxAddr[6] = "DPY01";

int bright = 200; //50; //200; //adjust brightness for all pixels 0-255 range,

int current = 0 ;
int currentInput = 0 ;
int color = 2;
int sonido = 0;
bool zerofill = true;

void setup(void){
  Serial.begin(9600);
  Serial.println("Inicando...");
  
  pinMode(LED_PIN, OUTPUT); // Set RGB Stick UNO pin to an OUTPUT
  FastLED.addLeds<NEOPIXEL,LED_PIN>(leds, NUM_LEDS); // Setup FastLED Library
  FastLED.clear(); // Clear the RGB Stick LEDs
  
  // Light up starting LED's
  //for(int x = 0; x != NUM_LEDS; x++) {
  //  leds[x] = CRGB::Green;
  //}
  
  FastLED.setBrightness(bright);
  FastLED.show();

  efectoCilon();

  delay(200);
  setVal(current);

  radio.begin();
  radio.openReadingPipe(0, rxAddr);
  
  radio.startListening();

  if(radio.isChipConnected() == false){
    Serial.println("No se detecta el chip nrf");
  }else{
    Serial.println("Chip nrf detectado correctamente");
  }
}

void loop() {
  procesarComandoNRF();

  if(current != currentInput){
    current = currentInput;
    setVal(current);
    Serial.print("VAL|");
    Serial.print(current);
    Serial.println("|");
    Serial.print("Valor de brillo: ");
    Serial.println(bright);
    sonar();
  }
}

void procesarComandoNRF(){
  if (radio.available()){
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    if ((text[0] == 'V') && (text[1] == 'A') && (text[2] == 'L') && (text[3] == '|')){ //valor a mostrar
      char valComando[5] = {text[4], text[5], text[6], text[7], '\0'};
      int int_valComando = 0;
      int_valComando += (text[4] - '0') * 1000;
      int_valComando += (text[5] - '0') * 100;
      int_valComando += (text[6] - '0') * 10;
      int_valComando += (text[7] - '0');
      //Serial.print("Tenemos valor decimal VAL: ");
      //Serial.println(int_valComando);
      if(int_valComando != current){
        //current = int_valComando;
        //setVal(current);
        currentInput = int_valComando;
        Serial.println("Tenemos comando VAL nuevo");
      }
    }else if((text[0] == 'C') && (text[1] == 'L') && (text[2] == 'R') && (text[3] == '|')){ //color a mostrar
      //Serial.println("Tenemos comando CLR");
      int int_valColor = 1;
      int_valColor = (text[4] - '0') * 10;
      int_valColor += (text[5] - '0');
      color = int_valColor;
      setVal(current); //actualizar color
    }else if((text[0] == 'B') && (text[1] == 'R') && (text[2] == 'T') && (text[3] == '|')){ //brillo de los leds
      char valComando[4] = {text[4], text[5], text[6], '\0'};
      int int_valComando = 0;
      int_valComando += (text[4] - '0') * 100;
      int_valComando += (text[5] - '0') * 10;
      int_valComando += (text[6] - '0');
      bright = int_valComando;
      FastLED.setBrightness(bright);
      FastLED.show();
      //setVal(current); //actualizar brillo
    }else if((text[0] == 'S') && (text[1] == 'N') && (text[2] == 'D') && (text[3] == '|')){ //brillo de los leds
      char valComando[2] = {text[4], '\0'};
      int int_valComando = 0;
      int_valComando += (text[4] - '0');
      sonido = int_valComando;
    }else if((text[0] == 'Z') && (text[1] == 'F') && (text[2] == 'L') && (text[3] == '|')){ //zero fill
      char valComando[2] = {text[4], '\0'};
      int int_valComando = 0;
      int_valComando += (text[4] - '0');
      if(int_valComando == 0){
        zerofill = false;
      }else{
        zerofill = true;
      }
    }else if((text[0] == 'E') && (text[1] == 'F') && (text[2] == 'T') && (text[3] == '|')){ //efecto
      efectoCilon();
      efectoCilon();
      current = -1;
    }else if((text[0] == 'O') && (text[1] == 'F') && (text[2] == 'F') && (text[3] == '|')){ //apagar
      FastLED.clear();
      current = 0;
      currentInput = 0;
    }
     
    //Serial.println(text);
  }
}

void setVal(int value){
  char valueString[5];
  sprintf(valueString, "%04d", value);

  //FastLED.clear(); // Clear the RGB Stick LEDs
  
  //this uses digitWrite() command to run through all numbers with one second 
  //timing on each of the five digit panels in the display simultaneously
  for(int i=0; i<4; i++){
    //digitWrite(i, 8, 0); //clear it
    //pixels.show();
    
    //int val = atoi(valueString[3-i]);
    int val = valueString[3-i] - '0';
    if(zerofill){
      digitWrite(i, val, color);
    }else{
      char tmpVal = 0;
      for(int j=i; j<4; j++){
        if(valueString[3-j] == '0'){
          tmpVal++;
        }else{
          tmpVal--;
        }
      }
      if(tmpVal == (4-i)){
        digitWrite(i, val, 0); //off
      }else{
        digitWrite(i, val, color);
      }
    }
    FastLED.show();
  }
}

////////////////////////////////////////////////////////////////////////////////
void digitWrite(int digit, int val, int col){
  //use this to light up a digit
  //digit is which digit panel one (right to left, 0 indexed)
  //val is the character value to set on the digit
  //col is the predefined color to use, R,G,B or W
  //example: 
  //        digitWrite(0, 4, 2); 
  //would set the first digit
  //on the right to a "4" in green.
 
/*
// Letters are the standard segment naming, as seen from the front,
// numbers are based upon the wiring sequence
 
          A 2     
     ----------
    |          |
    |          |
F 1 |          | B 3
    |          |
    |     G 7  |
     ----------
    |          |
    |          |
E 6 |          | C 4
    |          |
    |     D 5  |
     ----------    dp 8
 
*/
//these are the digit panel character value definitions, 
//if color argument is a 0, the segment is off
  if (val==0){
    //segments F,A,B,C,D,E,G
    segLight(digit,1,col);
    segLight(digit,2,col);
    segLight(digit,3,col);
    segLight(digit,4,col);
    segLight(digit,5,col);
    segLight(digit,6,col);
    segLight(digit,7,0);
  }
  if (val==1){
    segLight(digit,1,0);
    segLight(digit,2,0);
    segLight(digit,3,col);
    segLight(digit,4,col);
    segLight(digit,5,0);
    segLight(digit,6,0);
    segLight(digit,7,0);
  }
  if (val==2){
    segLight(digit,1,0);
    segLight(digit,2,col);
    segLight(digit,3,col);
    segLight(digit,4,0);
    segLight(digit,5,col);
    segLight(digit,6,col);
    segLight(digit,7,col);
  }
  if (val==3){
    segLight(digit,1,0);
    segLight(digit,2,col);
    segLight(digit,3,col);
    segLight(digit,4,col);
    segLight(digit,5,col);
    segLight(digit,6,0);
    segLight(digit,7,col);
  }
  if (val==4){
    segLight(digit,1,col);
    segLight(digit,2,0);
    segLight(digit,3,col);
    segLight(digit,4,col);
    segLight(digit,5,0);
    segLight(digit,6,0);
    segLight(digit,7,col);
  }
  if (val==5){
    segLight(digit,1,col);
    segLight(digit,2,col);
    segLight(digit,3,0);
    segLight(digit,4,col);
    segLight(digit,5,col);
    segLight(digit,6,0);
    segLight(digit,7,col);
  }
  if (val==6){
    segLight(digit,1,col);
    segLight(digit,2,col);
    segLight(digit,3,0);
    segLight(digit,4,col);
    segLight(digit,5,col);
    segLight(digit,6,col);
    segLight(digit,7,col);
  }          
  if (val==7){
    segLight(digit,1,0);
    segLight(digit,2,col);
    segLight(digit,3,col);
    segLight(digit,4,col);
    segLight(digit,5,0);
    segLight(digit,6,0);
    segLight(digit,7,0);
  }
  if (val==8){
    segLight(digit,1,col);
    segLight(digit,2,col);
    segLight(digit,3,col);
    segLight(digit,4,col);
    segLight(digit,5,col);
    segLight(digit,6,col);
    segLight(digit,7,col);
  }
  if (val==9){
    segLight(digit,1,col);
    segLight(digit,2,col);
    segLight(digit,3,col);
    segLight(digit,4,col);
    segLight(digit,5,col);
    segLight(digit,6,0);
    segLight(digit,7,col);
  }    
}
//END void digitWrite()
////////////////////////////////////////////////////////////////////////////////
 
 
////////////////////////////////////////////////////////////////////////////////
void segLight(char digit, int seg, int col){ 
  //use this to light up a segment
  //digit picks which neopixel strip
  //seg calls a segment
  //col is color
  int color[3];
 
  //color sets
    if (col==0){ //off
      color[0]={0};
      color[1]={0};
      color[2]={0};
    }
    if (col==1){ //red
      color[0]={255};
      color[1]={0};
      color[2]={0};
    }
    if (col==2){ //green
      color[0]={0};
      color[1]={255};
      color[2]={0};
    }
    if (col==3){ //blue
      color[0]={0};
      color[1]={0};
      color[2]={255};
    }
    if (col==4){ //white -- careful with this one, 3x power consumption
      //if 255 is used
      color[0]={100};
      color[1]={100};
      color[2]={100};
    }
 
      if (col==5){ //yellow
      color[0]={220};
      color[1]={150};
      color[2]={0};
    }
        if (col==6){ //
      color[0]={0};
      color[1]={150};
      color[2]={150};
    }
        if (col==7){ //
      color[0]={150};
      color[1]={0};
      color[2]={150};
    }
        if (col==8){ //
      color[0]={220};
      color[1]={50};
      color[2]={50};
    }
        if (col==9){ //
      color[0]={200};
      color[1]={100};
      color[2]={0};
    }
        if (col==10){ // 
      color[0]={0};
      color[1]={50};
      color[2]={200};
    }

  int offset = (21*digit);
 
  //sets are 0-7, 8-15, 16-23, 24-31, 32-39, 40-47, 48-55, 56
  //seg F
  if(seg==1){
    //light first 8
    for(int i=0; i<3; i++){
      leds[i+offset] = CRGB(color[0],color[1],color[2]);
    }  
  }
  //seg A
  if(seg==2){
     //light second 8
     for(int i=3; i<6; i++){
      leds[i+offset] = CRGB(color[0],color[1],color[2]);
     } 
  }
  //seg B
  if(seg==3){
    for(int i=6; i<9; i++){
      leds[i+offset] = CRGB(color[0],color[1],color[2]);
    }   
  }
  //seg C
  if(seg==4){
      for(int i=9; i<12; i++){
      leds[i+offset] = CRGB(color[0],color[1],color[2]);
      }   
  }
  //seg D
  if(seg==5){
      for(int i=12; i<15; i++){
      leds[i+offset] = CRGB(color[0],color[1],color[2]);
      }   
  }
  //seg E
  if(seg==6){
      for(int i=15; i<18; i++){
      leds[i+offset] = CRGB(color[0],color[1],color[2]);
      }   
  }
  //seg G
  if(seg==7){
      for(int i=18; i<21; i++){
      leds[i+offset] = CRGB(color[0],color[1],color[2]);
      }   
  }
}
//END void segLight()
////////////////////////////////////////////////////////////////////////////////

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void efectoCilon() { 
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }

  // Now go in the other direction.  
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
}

void sonidoCoin(){
  tone(3,NOTE_B5,100);
  delay(100);
  tone(3,NOTE_E6,550);
  delay(550);
  noTone(3);
}

void sonidOneUp(){
  tone(3,NOTE_E6,125);
  delay(130);
  tone(3,NOTE_G6,125);
  delay(130);
  tone(3,NOTE_E7,125);
  delay(130);
  tone(3,NOTE_C7,125);
  delay(130);
  tone(3,NOTE_D7,125);
  delay(130);
  tone(3,NOTE_G7,125);
  delay(125);
  noTone(3);
}

void sonar(){
  if(sonido == 1){
    sonidoCoin();
  }else if(sonido == 2){
    sonidOneUp();
  }
}
  
