#include <SoftwareSerial.h>
#include <string.h>

SoftwareSerial edip240(10, 11); //RX, TX

#define DEBUG 0

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  edip240.begin(115200);

  //reset display
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  pinMode(9, INPUT);

  edip240_send("#DL");
  edip240_send("#TA");
  //edip240_send("#ZL0,0,Hello World\r");

  //  edip240_send("#ZF5");
  //  edip240_send("#TA");
  //  edip240_send("#ZL0,0,Hello World\r");
  //  edip240_send("#ZF7");
  //  edip240_send("#TA");
  //  edip240_send("#ZL0,15,0123456789\r");
  //  //edip240_send("#GR0,0,240,128");
  //  edip240_send("#TA");
  //
  edip240_send("#DL");
  for (uint8_t x = 0; x <= 64; x += 2) {
    edip240_draw_rectangle(x, x, 128 - x, 128 - x);
    edip240_send("#TA");
  }


}

void loop() {
  while (edip240.available()) {
    Serial.print("edip240 response: ");
    Serial.println(edip240.read(), HEX);
  }
}


void edip240_draw_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
  uint8_t buff[18]  = {0};

  //format command
  sprintf(buff, "#GR%d,%d,%d,%d", x1, y1, x2, y2);
  //send command
  edip240_send(buff);
}

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


