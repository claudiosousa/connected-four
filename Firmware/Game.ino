boolean user_turn = false;
float devicecolor[2] = {.0, .0};
int winloststatus = -1;  // -1 = play game, 0 = lost game, 1 = win game
float blinklum = 0, dir = -1, lumstep;  // Blink the user's LED
unsigned long blinkmillis;

void handleDeviceToken(int i, char * token) {
  switch (i) {
    case 0:
      devicecolor[0] = String(token).toFloat();
      break;
    case 1:
      devicecolor[1] = String(token).toFloat();
      break;
    case 2:
      user_turn = token[0] == '1';
      break;
  }
}

void setdevicecolor(char * msg) {
  splitStr(msg, *handleDeviceToken);

#ifdef DEBUG
  Serial.print("Hue: "); Serial.print(devicecolor[0]); Serial.print(", Lum: "); Serial.print(devicecolor[1]); Serial.print(", user_turn: "); Serial.println(user_turn );
#endif
}
