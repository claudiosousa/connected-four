boolean game_start = true;
boolean user_turn = false; 
float devicecolor[2] = {0, 0};
int winloststatus = -1;           // -1 = play game, 0 = lost game, 1 = win game
float blinklum = 0, dir = -1, lumstep;          //Blink the user's LED
unsigned long blinkmillis;                     


void devicecolorsetting(String msg) {       
  unsigned int cptposmsg, cptpossubmsg;
  int infonum;
  String strledhue, strledlum, struserstat;
  char submes[16];

#ifdef DEBUG
  Serial.println("Decode message for device's color.");
  Serial.print("Old color are : ");
  Serial.print(devicecolor[0]);
  Serial.print(" / ");
  Serial.println(devicecolor[1]);
#endif

  infonum = 0;
  cptpossubmsg = 0;

  for ( cptposmsg = 0; cptposmsg <= msg.length(); cptposmsg++) {

    if ( msg[cptposmsg] != '|' && /*msg[cptposmsg] == '\0'*/ cptposmsg != msg.length() ) {
      submes[cptpossubmsg] = msg[cptposmsg];
      cptpossubmsg++;
    }
    else {
      submes[cptpossubmsg] = '\0';
      cptpossubmsg = 0;
      infonum++;

#ifdef DEBUG
      Serial.print("infonum = ");
      Serial.println(infonum);
      Serial.print("submes = ");
      Serial.println(submes);
#endif

      switch (infonum) {

        case 1:
          strledhue = String(submes);
          break;

        case 2:
          strledlum = String(submes);
          break;

        case 3:
          struserstat = String(submes);
#ifdef DEBUG
          Serial.print("struserstat = ");
          Serial.println(struserstat);
#endif
          if ( struserstat == "0" )
            user_turn = false;
          if ( struserstat == "1" )
            user_turn = true;
          break;

        default:
#ifdef DEBUG
          Serial.println("case default reach !");
#endif
          ;

      }
    }
  }
  devicecolor[0] = strledhue.toFloat();
  devicecolor[1] = strledlum.toFloat();
#ifdef DEBUG
  Serial.print("Data are : ");
  Serial.print(", hue : ");
  Serial.print(devicecolor[0]);
  Serial.print(", lum : ");
  Serial.println(devicecolor[1]);
#endif

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void user_turnsetting(String msg) {              //Setup the user's status

  unsigned int cptposmsg, cptpossubmsg;
  int infonum;
  String strinfo;
  char submes[16];

#ifdef DEBUG
  Serial.println("Decode message for user's status.");
  Serial.print("Old status is : ");
  Serial.println(user_turn);
#endif

  infonum = 0;
  cptpossubmsg = 0;

  for ( cptposmsg = 0; cptposmsg <= msg.length(); cptposmsg++) {

    if ( msg[cptposmsg] != '|' && /*msg[cptposmsg] == '\0'*/ cptposmsg != msg.length() ) {
      submes[cptpossubmsg] = msg[cptposmsg];
      cptpossubmsg++;
    }
    else {
      submes[cptpossubmsg] = '\0';
      cptpossubmsg = 0;
      infonum++;

#ifdef DEBUG
      Serial.print("infonum = ");
      Serial.println(infonum);
      Serial.print("submes = ");
      Serial.println(submes);
#endif

      switch (infonum) {

        case 1:
          strinfo = String(submes);
          break;

        default:
#ifdef DEBUG
          Serial.println("case default reach !");
#endif
          ;

      }
    }
  }
  if ( 1 == strinfo.toInt() )
    user_turn = true;
  else
    user_turn = false;

#ifdef DEBUG
  Serial.println("Old status is : ");
  Serial.println(user_turn);
#endif

}
