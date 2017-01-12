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
#define DEBUG 1

uint8_t current_life[256] = {0};
uint8_t next_life[256] = {0};
/*
    manipulate cell @x,y
    mode 0: set cell dead
    mode 1: set cell alive
    mode 2: get cell
*/
uint8_t cell_at(uint8_t x, uint8_t y, uint8_t *data,  uint8_t mode) {

  uint8_t data_idx = 0;
  uint8_t data_col = 0;
  uint8_t data_bit = 0;

  data_col = x / 8;
  data_bit = 7 - ( x % 8);
  data_idx = (y * 8) + data_col;

#if DEBUG == 1
  Serial.print("Cell @x: ");
  Serial.println(x);
  Serial.print("Cell @y: ");
  Serial.println(y);
  Serial.print("Data column: ");
  Serial.println(data_col);
  Serial.print("Data bit: ");
  Serial.println(data_bit);
  Serial.print("Data index: ");
  Serial.println(data_idx);
  Serial.print("Data: 0x");
  Serial.print(data[data_idx], HEX);
  Serial.println();
#endif

  switch (mode) {
    case 0:
      data[data_idx] &= ~(1 << data_bit);
      return 0;
      break;
    case 1:
      data[data_idx] |= (1 << data_bit);
      return 0;
    default:
      return data[data_idx] & (1 << data_bit) ? 1 : 0;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  edip240.begin(115200);
//  current_life[0] = 0x81;
//  current_life[8] = 0x01;
//  uint8_t i;
//  uint8_t j;
//  uint8_t cell;
//
//  for (j = 0; j < 2; j++) {
//    for (i = 0; i < 8; i++) {
//
//      cell = cell_at(i, j, current_life, 2);
//      Serial.print("Status at begining of world for cell ");
//      Serial.println(cell);
//    }
//  }
  edip240_send("#DL");
  edip240_draw_cell(10,10,1);
  delay(1000);
  edip240_draw_cell(10,10,0);
}

void loop() {

}





