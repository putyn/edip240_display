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
/*
 * return neighbours count
 */
uint8_t count_neighbours(uint8_t cell_x, uint8_t cell_y) {

  int16_t neighbours, nx, x, ny, y;

  neighbours = cell_at(cell_x, cell_y, current_life, 2) ? -1 : 0;
  for ( x = -1; x <= 1; x++) {
    for (y = -1; y <= 1; y++) {
      nx = (WORLD_X_MAX + x + cell_x) % WORLD_X_MAX;
      ny = (WORLD_Y_MAX + y + cell_y) % WORLD_Y_MAX;
      if (cell_at(nx, ny, current_life, 2)) {
        neighbours++;
      }
    }
  }
  return neighbours;
} 
/*
 * compute next generation and display it
 */
void next_generation() {
  uint8_t x, y, neighbours, is_alive;

  //calculate next life
  for (x = 0; x < WORLD_X_MAX; x++) {
    for (y = 0; y < WORLD_Y_MAX; y++) {
      neighbours = count_neighbours(x, y);
      is_alive = cell_at(x, y, current_life, 2);
      if (neighbours < 2 && is_alive)
        cell_at(x, y, next_life, 0); //dead - under-population
      if (neighbours > 3 && is_alive)
        cell_at(x, y, next_life, 0); //dead - overcrowding
      if ((neighbours >= 2 && neighbours <= 3) && is_alive)
        cell_at(x, y, next_life, 1); //alive
      if (neighbours == 3 && !is_alive)
        cell_at(x, y, next_life, 1); //alive - reproduction
    }
  }
  //display next life
  //edip240_send("#DL");
  for (x = 0; x < WORLD_X_MAX; x++) {
    for (y = 0; y < WORLD_Y_MAX; y++) {
      if (cell_at(x, y, current_life, 2) != cell_at(x, y, next_life, 2))
        edip240_draw_cell(x * 4, y * 4, cell_at(x, y, next_life, 2));
    }
  }
  //copy next live to current life
  memcpy(current_life, next_life, 256);
}
