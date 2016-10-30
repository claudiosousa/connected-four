void setdevicecolor(char * msg) {
  split_to_array(msg);

  set_device_status(String(splitted[0]).toFloat(), String(splitted[1]).toFloat(), splitted[2][0] == '1');

#ifdef DEBUG
  Serial.println(String("Hue: ") + splitted[0] + ", Lum: " + splitted[1]);
#endif
}

void addpiece(char * msg) {  // Decode the payload for a new piece
  play_submitmove();

  split_to_array(msg);

  set_device_status_blink(splitted[4][0] == '1');
  drop_piece(atoi(splitted[0]), atoi(splitted[1]), String(splitted[2]).toFloat(), String(splitted[3]).toFloat());

#ifdef DEBUG
  Serial.print(String("Row") + splitted[0] + ", col: " + splitted[1] + ", hue: " + splitted[2] + "lum : " + splitted[3]);
#endif
}


void set_game_finished(char * msg) {

  set_device_status_blink(true);

  split_to_array(msg);

  set_game_finished_pieces(String(splitted[1]).toFloat(), String(splitted[2]).toFloat(), splitted[3], splitted[0][0] == '1');
}


