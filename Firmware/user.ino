/* Manage the user infos */


float devicecolor[2];                           //User's color. [0] = hue, [1] = lum

float blinklum = 0, dir = -1, lumstep;          //Blink the user's LED
unsigned long blinkmillis;                      //Duration of the blink

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup_user () {        // Setup the user's colors. By default = black, no blink.

  devicecolor[0] = 0; // hue
  devicecolor[1] = 0; // Lum
  userstatus = false;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop_user() {        //Blink the led when neded.

  float steplum;

  if (userstatus == false) {        //Not user's turn
    blinklum = devicecolor[1];
    blinkmillis = millis();
    lumstep = 20;
    dir = -1;
  }
  else {
    //Serial.print("-");
    if (millis() - blinkmillis >= 20) {

      //Serial.println(".");
      blinkmillis = millis();
      steplum = devicecolor[1] / 20;
      lumstep = lumstep + dir;
      if (lumstep >= 50)
        dir = -1;
      if (lumstep <= 0)
        dir = 1;

      blinklum = steplum * lumstep;

    }
  }

  led->SetPixelColor(Nbr_LEDS-1, HslColor(devicecolor[0], 1, blinklum));
  led->Show();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void devicecolorsetting(String msg) {           //Decode the MQTT message 

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
            userstatus = false;
          if ( struserstat == "1" )
            userstatus = true;
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
void userstatussetting(String msg) {              //Setup the user's status

  unsigned int cptposmsg, cptpossubmsg;
  int infonum;
  String strinfo;
  char submes[16];

#ifdef DEBUG
  Serial.println("Decode message for user's status.");
  Serial.print("Old status is : ");
  Serial.println(userstatus);
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
    userstatus = true;
  else
    userstatus = false;

#ifdef DEBUG
  Serial.println("Old status is : ");
  Serial.println(userstatus);
#endif

}
