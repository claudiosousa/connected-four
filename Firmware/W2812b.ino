#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* leds;  // Initialise a pointer for the led's datas.

boolean newpiece = false, canplayswinlost = true;  // Is a new piece is adding.
// Used when last piece is
// added ( don't stop the
// fall down process)
unsigned long addpiecemillis, winlostmillis;  // Time delay
int cptledrow;  // For the loops process, add a piece with fall down
float ledcol, ledrow, ledhue, ledlum,
      cptledlum;  // For the loops process, add a piece with fall down
int Nbr_LEDS;

int nbrwinleds, winlostonoffstat;  // number of winner's leds. On off blink
// status for win, lost game
float huewin, lumwin;              // hue and lum of winner's pieces
int rowwin[7], colwin[7];          // Winner's leds coordinates

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int indexled(int row,
             int col) {  // Give the lineare index depending if display is with
  // 43 or 79 leds from matrix coordinates
  int index;

#ifdef WS_79_LEDS
  index = (row * 7 + col) * 2 - row;
#else
  index = row * 7 + col;
#endif

  return index;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int indexled(int ledindex) {  // Convert the lineare index depending if display
  // is with 43 or 79 leds
  int index;

#ifdef WS_79_LEDS
  index = ledindex * 2 - ledindex / 7;
#else
  index = ledindex;
#endif
  return index;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup_ws() {  // Setup the display

#ifdef WS_79_LEDS
  Nbr_LEDS = 79;
#else
  Nbr_LEDS = 43;
#endif

  // led = new NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod>(Nbr_LEDS,
  // PIN_WS);

  // Pin Not used by the methode "Neo800KbpsMethod". This methode use the RX0
  // pin (GIPO03)
  leds = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(Nbr_LEDS, 2);

  leds->Begin();

  HslColor black = HslColor(0, 0, 0);

  for (int i = 0; i < Nbr_LEDS; i++) 
    leds->SetPixelColor(i, black);  // i, i % 2 == 0 ? HslColor(0, 1, 0.5) : HslColor(.3, 1, 0.5));
  
  leds->Show();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void addpiece_ws(String msg) {  // Decode the payload for a new piece
  unsigned int cptposmsg, cptpossubmsg;
  int infonum, lednum;
  String strledcol, strledrow, strledhue, strledlum, struserstat;
  char submes[16];

#ifdef DEBUG
  Serial.println("Decoding message for addpiece.");
#endif
  play_submitmove();  // Start to play "submit move" sound

  // Extract character by character the informations from payload message.

  infonum = 0;
  cptpossubmsg = 0;

  for (cptposmsg = 0; cptposmsg <= msg.length();
       cptposmsg++) {  // Char by char, from the begin to the end of the payload
    // message

    if (msg[cptposmsg] != '|' &&
        /*msg[cptposmsg] == '\0'*/ cptposmsg != msg.length()) {
      submes[cptpossubmsg] = msg[cptposmsg];
      cptpossubmsg++;
    } else {
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
          strledrow = String(submes);
          break;

        case 2:
          // ledcol = submes.string().ToFloat();
          strledcol = String(submes);
          break;

        case 3:
          strledhue = String(submes);
          break;

        case 4:
          strledlum = String(submes);
          break;

        case 5:
          struserstat = String(submes);
#ifdef DEBUG
          Serial.print("struserstat = ");
          Serial.println(struserstat);
#endif
          if (struserstat == "0") user_turn = false;
          if (struserstat == "1") user_turn = true;
          break;

        default:
#ifdef DEBUG
          Serial.println("case default reach !");
#endif
          ;
      }
    }
  }
  // Convert datas
  ledcol = strledcol.toFloat();
  ledrow = strledrow.toFloat();
  ledhue = strledhue.toFloat();
  ledlum = strledlum.toFloat();
  lednum = indexled(ledrow,
                    ledcol);  // Rows and columns are inverted in the hardware.

#ifdef DEBUG
  Serial.print("Valeurs décodées, colonne : ");
  Serial.print(ledcol);
  Serial.print(", ligne : ");
  Serial.print(ledrow);
  Serial.print(", hue : ");
  Serial.print(ledhue);
  Serial.print(", lum : ");
  Serial.println(ledlum);
  Serial.print("LED number concerned : ");
  Serial.print(lednum);
#endif

  cptledrow = 5;
  cptledlum = 0;
  addpiecemillis = millis();
  newpiece = true;  // Enable the add piece with fall down process
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void closeaddpiece_ws() {  // Stop the fall down process. Put the new piece at
  // is right place

  int cpt;
  newpiece = false;
  addpiecemillis = millis();

#ifdef DEBUG
  Serial.println("Close addpiece");
#endif

  for (cpt = ledrow + 1; cpt <= 5; cpt++) {
    leds->SetPixelColor(indexled(cpt, ledcol), HslColor(0, 0, 0));
#ifdef DEBUG
    Serial.print("row to put to black : ");
    Serial.println(cpt);
#endif
  }
  leds->SetPixelColor(indexled(ledrow, ledcol), HslColor(ledhue, 1, ledlum));
  leds->Show();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void boardsetting(String msg) {  // Decode the payload to set up the board
  float hue[3], lum[3];
  int code = 0;
  unsigned int cptposmsg, cptpossubmsg;
  int /*cptled,*/ cptinfo;
  char submsg[32];
  String strsubmsg;

  winloststatus = -1;
  canplayswinlost = true;

  for (int i = 0; i < 3; i++) {
    hue[i] = 0;
    lum[i] = 0;
  }

#ifdef DEBUG
  Serial.println("Board received");
#endif

  cptposmsg = 0;
  cptpossubmsg = 0;
  /*cptled = 0;*/
  cptinfo = 0;

  while (cptposmsg <= msg.length() && (cptinfo - 5) < 42) {
    if ((msg[cptposmsg] != '|' && msg[cptposmsg] != '\0') && (cptinfo < 4)) {
      submsg[cptpossubmsg] = msg[cptposmsg];
      cptpossubmsg++;
      /* #ifdef DEBUG
            Serial.print("cptinfo <=4, decoding one char before | or \0");
        #endif */
    } else {
      if (cptinfo >= 4) {
        submsg[cptpossubmsg] = msg[cptposmsg];
        cptpossubmsg++;
#ifdef DEBUG
        Serial.println("cptinfo > 4, decoding one char only");
#endif
      }

      submsg[cptpossubmsg] = '\0';
      cptpossubmsg = 0;
      strsubmsg = String(submsg);
      cptinfo++;

#ifdef DEBUG
      Serial.print("Led n° : ");
      Serial.println(cptinfo - 5);
      Serial.print("cptposmsg : ");
      Serial.println(cptposmsg);
      Serial.print("cptpossubmsg : ");
      Serial.println(cptpossubmsg);
      Serial.print("cptinfo : ");
      Serial.println(cptinfo);
      Serial.print("submsg : ");
      Serial.println(String(submsg));
      Serial.println();
#endif

      if (cptinfo == 1) {
        hue[1] = strsubmsg.toFloat();
      }

      if (cptinfo == 2) {
        lum[1] = strsubmsg.toFloat();
      }

      if (cptinfo == 3) {
        hue[2] = strsubmsg.toFloat();
      }

      if (cptinfo == 4) {
        lum[2] = strsubmsg.toFloat();
      }

#ifdef DEBUG
      if (cptinfo <= 4) {
        Serial.print("hue 0 : ");
        Serial.println(hue[0]);
        Serial.print("lum 0 : ");
        Serial.println(lum[0]);
        Serial.print("hue 1 : ");
        Serial.println(hue[1]);
        Serial.print("lum 1 : ");
        Serial.println(lum[1]);
        Serial.print("hue 2 : ");
        Serial.println(hue[2]);
        Serial.print("lum 2 : ");
        Serial.println(lum[2]);
      }
#endif

      if (cptinfo > 4) {
        code = strsubmsg.toInt();
        leds->SetPixelColor(indexled(cptinfo - 5),
                            HslColor(hue[code], 1, lum[code]));

#ifdef DEBUG
        Serial.print("led : ");
        Serial.println(cptinfo - 5);
        Serial.print("Player : ");
        Serial.println(code);
#endif
      }
    }
    cptposmsg++;
  }
  leds->Show();
}

void loop_user() {  // Blink the led when neded.

  float steplum;

  if (user_turn == false) {  // Not user's turn
    blinklum = devicecolor[1];
    blinkmillis = millis();
    lumstep = 20;
    dir = -1;
  } else {
    // Serial.print("-");
    if (millis() - blinkmillis >= 20) {
      // Serial.println(".");
      blinkmillis = millis();
      steplum = devicecolor[1] / 20;
      lumstep = lumstep + dir;
      if (lumstep >= 50) dir = -1;
      if (lumstep <= 0) dir = 1;

      blinklum = steplum * lumstep;
    }
  }

  leds->SetPixelColor(Nbr_LEDS - 1, HslColor(devicecolor[0], 1, blinklum));
  leds->Show();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gamefinished_ws(String msg) {  // Decode the payload when game is finished

  int code = 0;
  unsigned int cptposmsg, cptpossubmsg;
  int cptinfo;
  char submsg[32];
  String strsubmsg;

  user_turn = true;  // Put to blink user's led
  winlostmillis = millis();
  winlostonoffstat = 0;

#ifdef DEBUG
  Serial.println("Game finished received");
#endif

  cptposmsg = 0;
  cptpossubmsg = 0;
  cptinfo = 0;

  while (cptposmsg < msg.length()) {
    if ((msg[cptposmsg] != '|' && msg[cptposmsg] != '\0') && (cptinfo < 3)) {
      submsg[cptpossubmsg] = msg[cptposmsg];
      cptpossubmsg++;
      /* #ifdef DEBUG
            Serial.print("cptinfo < 3, decoding one char before | or \0");
        #endif */
    } else {
      if (cptinfo >= 3) {
        submsg[cptpossubmsg] = msg[cptposmsg];
        cptpossubmsg++;
#ifdef DEBUG
        Serial.print("cptinfo > ");
        Serial.print(cptinfo);
        Serial.println(", decoding one char only");
#endif
      }

      submsg[cptpossubmsg] = '\0';
      cptpossubmsg = 0;
      strsubmsg = String(submsg);
      cptinfo++;

      if (cptinfo == 1) {
        winloststatus = strsubmsg.toFloat();
#ifdef DEBUG
        Serial.print("winloststatus = ");
        Serial.println(winloststatus);
#endif
      }

      if (cptinfo == 2) {
        huewin = strsubmsg.toFloat();
#ifdef DEBUG
        Serial.print("huewin = ");
        Serial.println(huewin);
#endif
      }

      if (cptinfo == 3) {
        lumwin = strsubmsg.toFloat();
#ifdef DEBUG
        Serial.print("lumwin = ");
        Serial.println(lumwin);
#endif
      }

      if (cptinfo > 3) {
        code = strsubmsg.toInt();
        if (cptinfo % 2 == 0) {
          rowwin[int(floor(cptinfo / 2) - 2)] = code;
#ifdef DEBUG
          Serial.print("cptinfo = ");
          Serial.print(cptinfo);
          Serial.print(" - led n° = ");
          Serial.println(floor(cptinfo / 2) - 2);
          Serial.print("row = ");
          Serial.println(rowwin[int(floor(cptinfo / 2) - 2)]);
#endif
        } else {
          colwin[int(floor(cptinfo / 2) - 2)] = code;
#ifdef DEBUG
          Serial.print("cptinfo = ");
          Serial.print(cptinfo);
          Serial.print(" - led n° = ");
          Serial.println(floor(cptinfo / 2) - 2);
          Serial.print("col = ");
          Serial.println(colwin[int(floor(cptinfo / 2) - 2)]);
#endif
        }
      }
      nbrwinleds = int(floor(cptinfo / 2) - 2) + 1;
    }
    cptposmsg++;
  }
#ifdef DEBUG
  Serial.print("nbrwinleds = ");
  Serial.print(nbrwinleds);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop_ws() {
  int cpt;

  /////////////////////////////////////////////////////////////////////// Add
  ///piece loop with shading effect
  if ((newpiece == true) && (millis() - addpiecemillis >= 10)) {
    addpiecemillis = millis();

#ifdef DEBUG
    Serial.println(cptledlum, 6);
#endif

    leds->SetPixelColor(indexled(cptledrow, ledcol),
                        HslColor(ledhue, 1, cptledlum));
    if (cptledrow < 5) {
      leds->SetPixelColor(
        indexled(cptledrow + 1, ledcol),
        HslColor(ledhue, 1, floor((ledlum - cptledlum) * 100) / 100));
    }
    leds->Show();

    cptledlum = cptledlum + ledlum / 9;
    if (cptledlum > ledlum) {
      cptledlum = 0;
      cptledrow--;
    }

    if (cptledrow < ledrow) {
      newpiece = false;
    }
  }

  /////////////////////////////////////////////////////////////////////// Win /
  ///Lost loop

  // Wait until addpiece fall down process is ending after receiving "Game
  // finished" message and start playing the right sound and blink the winner's
  // leds

  if (newpiece == false && winloststatus != -1 &&
      (millis() - winlostmillis >= 250)) {
    winlostmillis = millis();

    if (winloststatus != -1 && canplayswinlost) {
      if (winloststatus == 0) {
        play_gameover();
        canplayswinlost = false;
      } else {
        play_win();
        canplayswinlost = false;
      }
    }

    for (cpt = 0; cpt < nbrwinleds; cpt++) {
      leds->SetPixelColor(indexled(rowwin[cpt], colwin[cpt]),
                          HslColor(huewin, 1, lumwin * winlostonoffstat));
    }
    leds->Show();
    if (winlostonoffstat == 0)
      winlostonoffstat = 1;
    else
      winlostonoffstat = 0;
  }
}
