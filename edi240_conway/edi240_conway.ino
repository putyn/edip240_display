#include <SoftwareSerial.h>
#include <string.h>

SoftwareSerial edip240(10, 11); //RX, TX
/*
   given the display size 240x128 we would need quite a lot of ram 30720k (atmega328p has only 2k)
   given the phisical pixel size we will use 4 pixels to represent a cell,giving us a 60x32 world, which will require 1920k (still too much)
   given that one cell can have only two statest alive/dead (1/0) we can save the states of 8 cells in 1 byte requiring only 256bytes(8x32) of ram :)

*/
#define WORLD_X_MAX 60
#define WORLD_Y_MAX 32
#define DEBUG 0

uint8_t current_life[256] = {0};
uint8_t next_life[256] = {0};

void setup() {
  //for debug
  Serial.begin(115200);
  //serial com for display
  edip240.begin(115200);

  Serial.print("neighbours count: ");
  Serial.println(count_neighbours(1, 1));

  //variables
  uint16_t idx, x, y;

  //random data
  randomSeed(analogRead(0));
  for (idx = 0; idx < 256; idx++)
    current_life[idx] = (uint8_t)random(0,255);
      
  edip240_send("#DL");
  for (x = 0; x < WORLD_X_MAX; x++) {
    for (y = 0; y < WORLD_Y_MAX; y++) {
      if (cell_at(x, y, current_life, 2))
        edip240_draw_cell(x * 4, y * 4, 1);
    }
  }
}

char c;
void loop() {
//  if (Serial.available()) {
//    c = Serial.read();
//    switch (c) {
//      case 'n' :
//        next_generation();
//        break;
//    }
//
//  }
  next_generation();
  delay(250);
}





