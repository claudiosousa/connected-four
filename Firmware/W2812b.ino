#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* leds;  // Initialise a pointer for the led's datas.
HslColor black = HslColor(0, 0, 0);

const byte STATUS_ANIMATION_STEPS = 20;
const byte STATUS_ANIMATION_STEP_INTERVAL = 50;
const byte DROP_PIECE_ANIMATION_STEP_INTERVAL = 20;
const byte DROP_PIECE_ANIMATION_STEPS = 5;
const byte FINISH_ANIMATION_INTERVAL = 250;

typedef struct {
  float hue;
  float lum;
  bool blink;
  int anim_step;
  int anim_dir;
  unsigned long anim_last;
} status_anim_t;

status_anim_t status_anim;

typedef struct {
  int ledcount;
  int winleds[7];
  bool anim_visible;
  HslColor color;
  unsigned long anim_last;
  bool animate;
  int sound_to_play;
} finish_anim_t;

finish_anim_t finish_anim;

typedef struct {
  int col;
  int row;
  int anim_row;
  byte anim_step;
  unsigned long anim_last;
  float hue;
  float lum;
  bool animate;
} addpiece_anim_t;

addpiece_anim_t addpiece_anim;

#ifdef WS_79_LEDS
const int LED_COUNT = 79;
#else
const int LED_COUNT = 43;
#endif

int indexled(int col, int row) {
  int index = row * 7 + col;
#ifdef WS_79_LEDS
  index = index  * 2 - row;
#endif
  return index;
}

int indexled(int ledindex) {
#ifdef WS_79_LEDS
  retun ledindex * 2 - ledindex / 7;
#else
  return ledindex;
#endif
}

void setup_leds() {
  status_anim.blink = false;
  finish_anim.animate = false;
  addpiece_anim.animate = false;

  leds = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(LED_COUNT, 2);
  leds->Begin();

  for (int i = 0; i < LED_COUNT; i++)
    leds->SetPixelColor(i, black);

  leds->Show();
}

void loop_leds() {
  animate_status_led();
  animate_drop_piece();
  animate_game_finish();
}

void set_device_status(float hue, float lum, bool blink) {
  status_anim.hue = hue;
  status_anim.lum = lum;
  status_anim.anim_step = STATUS_ANIMATION_STEPS;
  status_anim.anim_dir = -1;
  status_anim.anim_last = 0;
  set_device_status_blink(blink);
}

void set_device_status_blink(bool blink) {
  status_anim.blink = blink;
  update_status_led(status_anim.lum);
}

void drop_piece(int ledrow, int ledcol, float hue, float lum) {
  stop_dropping_piece();

  play_submitmove();

  addpiece_anim.col = ledcol;
  addpiece_anim.row = ledrow;
  addpiece_anim.anim_step = 0;
  addpiece_anim.anim_row = 5;
  addpiece_anim.anim_last = 0;
  addpiece_anim.hue = hue;
  addpiece_anim.lum = lum;
  addpiece_anim.animate = true;
}

void stop_dropping_piece() {
  if (!addpiece_anim.animate)
    return;
  addpiece_anim.animate = false;

#ifdef DEBUG
  Serial.println("stop_dropping_piece");
#endif

  for (int i = addpiece_anim.row + 1; i <= 5; i++)
    leds->SetPixelColor(indexled(addpiece_anim.col, i), black);

  leds->SetPixelColor(indexled(addpiece_anim.col, addpiece_anim.row), HslColor(addpiece_anim.hue, 1, addpiece_anim.lum));
  leds->Show();
}


void set_board_pieces(char *  msg) {
#ifdef DEBUG
  Serial.println("set_board_pieces");
#endif

  stop_dropping_piece();
  finish_anim.animate = false;

  split_to_array(msg);

  HslColor colors[3] = {black,
                        HslColor(String(splitted[0]).toFloat(), 1., String(splitted[1]).toFloat()),
                        HslColor(String(splitted[2]).toFloat(), 1., String(splitted[3]).toFloat())
                       };
  int pos = 0;
  char c;
  while (c = *(splitted[4] + pos)) {
    Serial.println(String("set pixel: ") + pos + " :" + (int)(c - '0'));
    leds->SetPixelColor(pos, colors[(int)(c - '0')]);
    pos++;
  }

  leds->Show();

#ifdef DEBUG
  Serial.println("set_board_pieces_finished");
#endif
}

void set_game_finished_pieces(float hue, float  lum, char * pieces, bool won) {
  finish_anim.anim_visible = false;
  finish_anim.anim_last = 0;
  finish_anim.color.H = hue;
  finish_anim.color.L = lum;
  finish_anim.color.S = 1;
  finish_anim.animate = true;
  finish_anim.sound_to_play = won ? 1 : 2;

  finish_anim.ledcount = 0;
  char c;
  while (c = *(pieces + finish_anim.ledcount * 2)) {
    finish_anim.winleds[finish_anim.ledcount] = indexled(*(pieces + finish_anim.ledcount * 2 + 1) - '0', c - '0');
    finish_anim.ledcount++;
  }
}

void update_status_led(float lum) {
  leds->SetPixelColor(LED_COUNT - 1, HslColor(status_anim.hue, 1, lum));
  leds->Show();
}

void animate_status_led() {
  if (!status_anim.blink)
    return;

  if (millis() - status_anim.anim_last < STATUS_ANIMATION_STEP_INTERVAL)
    return;

  status_anim.anim_last = millis();

  status_anim.anim_step += status_anim.anim_dir;

  if (status_anim.anim_step == STATUS_ANIMATION_STEPS || status_anim.anim_step == 0)
    status_anim.anim_dir = -status_anim.anim_dir;

  update_status_led(status_anim.lum / STATUS_ANIMATION_STEPS * status_anim.anim_step);
}

void animate_drop_piece() {
  if (!addpiece_anim.animate)
    return;

  if (millis() - addpiece_anim.anim_last < DROP_PIECE_ANIMATION_STEP_INTERVAL)
    return;

  addpiece_anim.anim_last = millis();

  addpiece_anim.anim_step++;

  leds->SetPixelColor(indexled(addpiece_anim.col, addpiece_anim.anim_row), HslColor(addpiece_anim.hue, 1, addpiece_anim.lum / DROP_PIECE_ANIMATION_STEPS * addpiece_anim.anim_step));
  if (addpiece_anim.anim_row < 5)
    leds->SetPixelColor(indexled(addpiece_anim.col, addpiece_anim.anim_row + 1), HslColor(addpiece_anim.hue, 1, addpiece_anim.lum / DROP_PIECE_ANIMATION_STEPS * (DROP_PIECE_ANIMATION_STEPS - addpiece_anim.anim_step)));

  leds->Show();

  if (addpiece_anim.anim_step == DROP_PIECE_ANIMATION_STEPS) {
    addpiece_anim.anim_step = 0;
    addpiece_anim.anim_row--;
    if (addpiece_anim.anim_row < addpiece_anim.row)
      addpiece_anim.animate = false;
  }
}

void animate_game_finish() {

  if (addpiece_anim.animate) //animate winning piece drop first
    return;

  if (!finish_anim.animate)
    return;

  if (millis() - finish_anim.anim_last < FINISH_ANIMATION_INTERVAL)
    return;

  finish_anim.anim_last = millis();

  if (finish_anim.sound_to_play) {
    if (finish_anim.sound_to_play == 1)
      play_win();
    else
      play_gameover();

    finish_anim.sound_to_play = 0;
  }

  for (int i = 0; i < finish_anim.ledcount; i++)
    leds->SetPixelColor(finish_anim.winleds[i], finish_anim.anim_visible ? finish_anim.color : black);

  leds->Show();
  finish_anim.anim_visible = !finish_anim.anim_visible;
}

