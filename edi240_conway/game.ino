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
