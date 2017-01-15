/*
 * draw/delete cell on display @x,y 
 */
void edip240_draw_cell(uint8_t x, uint8_t y, uint8_t state) {
  uint8_t buff[18]  = {0};

  if(state) {
    sprintf(buff, "#RS%d,%d,%d,%d", x, y, x+3, y+3);
  } else {
    sprintf(buff, "#RL%d,%d,%d,%d", x, y, x+3, y+3);
  }

  edip240_send(buff);
  edip240_send("#TA");
}
/*
 * low level function to send ASCII data to display
 */
void edip240_send(const uint8_t *data) {

  uint8_t len, bcc, i, ans;
retry:
  //send DC1
  edip240.write(0x11);
  bcc = 0x11;

  //send data length
  len = strlen(data);
  edip240.write(len);
  bcc = bcc + len;

  //send data
  for (i = 0; i < len; i++) {
    edip240.write(data[i]);
    bcc = bcc + data[i];
  }
  //send BCC
  edip240.write(bcc);

  while (edip240.available() == 0);
  ans = edip240.read();

#if DEBUG == 1
  Serial.print("Data sent: \"");
  for (i = 0; i < len; i++) {
    Serial.write(data[i]);
  }
  Serial.print("\" length: ");
  Serial.println(len);
  Serial.print("Answer after command: ");
  Serial.println(ans, HEX);
#endif

  if ( ans != 0x06)
    goto retry;
}
