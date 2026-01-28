#include <FastLED.h>
#include <SoftwareSerial.h>
#include <LowPower.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#define ANALOG_PIN A2
#define NUM_LEDS 20
#define LED_PIN 9
#define LED_TYPE SK6812
#define COLOR_ORDER GRB
#define IN_PIN 10

uint32_t now = millis();
uint32_t last = 0;
unsigned long lastReboot = 0;
const unsigned long rebootInterval = 3600000UL;
int delay_anim = 20;
int delay_police = 500;
int delay_cylon = 100;
int delay_twinkle = 150;
int delay_goyda = 140;
int delay_even = 250;
int delay_beat = 5;

char buff[10];
int cnt = 0;
char c = ' ';
char animation = ' ';

bool n_state = false;
bool p_state = false;
bool blinker_state = false;
bool e_state = false;

float dc = 0;
float env = 0;
float noiseFloor = 5;
float peak = 50;
uint8_t lasthit = 0;

SoftwareSerial ble(IN_PIN, -1);

int brightness_level[10] ={10,36,54,72,90,108,126,144,180,230};
int brr = 180;

int color_now = 0;
CRGB leds[NUM_LEDS];
CRGB colors[10] = {
  CRGB(145,253,255),
  CRGB::Purple,
  CRGB::ForestGreen,
  CRGB::Red,
  CRGB::Green,
  CRGB::Blue,
  CRGB(255,100,0),
  CRGB::Cyan,
  CRGB::Magenta,
  CRGB(255,165,0)
};

void setup() {
  cli();
  wdt_reset();
  WDTCSR = (1<<WDCE) | (1<<WDE);
  WDTCSR = (1<<WDE) | (1<<WDP3) | (1<<WDP1);
  sei();
  wdt_disable();
  lastReboot = millis();

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(brr);

  ble.begin(9600);


  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), wakeUp, RISING);

  pinMode(LED_BUILTIN, OUTPUT);
  analogReference(INTERNAL);

  calibr();
}

void loop() {
  wdt_reset();
  if (ble.available()) {
    c = ble.read();

    if (c == '#'){
      process_cmd(buff);
      memset(buff, '0', sizeof(buff));
      cnt = 0;
    }else if (c == 'E' or c == 'R' or c == 'O'){}
    else if (cnt < 9){
      buff[cnt] = c;
      cnt++;
    }
    else{
      memset(buff, '0', sizeof(buff));
      cnt = 0;
    }
  }
  now = millis();
  if (now - last >= delay_anim){
    

    if (animation == 'p' and now-last >= delay_police){
      last = millis();
      p_state = !p_state;
      police();
    }
    else if (animation == 'r'){
      last = millis();
      rainbow();
    }
    else if (animation == 'b'){
      last = millis();
      black();
    }
    else if (animation == 'f'){
      last = millis();
      fade();
    }
    else if (animation == 'c' and now-last >= delay_cylon){
      last=millis();
      cylon();
    }
    else if(animation == 'l' and now - last >= delay_police){
      last = millis();
      left();
    }
    else if(animation == 'a' and now - last >= delay_police){
      last = millis();
      right();
    }
    else if(animation == 'd' and now - last >= delay_police){
      last = millis();
      danger();
    }
    else if(animation == 't' and now - last >= delay_twinkle){
      last = millis();
      twinkle();
    }
    else if(animation == 'n'){
      last = millis();
      new_year();
    }
    else if (animation == 'y'){
      last = millis();
      new_year2();
    }
    else if (animation == 'g' and now-last > delay_goyda){
      last = millis();
      goyda();
    }
    else if (animation == 'e' and now-last > delay_even){
      last = millis();
      even();
    }
    else if (animation == 'v' and now - last > delay_cylon){
      last = millis();
      pixel();
    }
    else if (animation == 'z' and now - last > delay_beat){
      last = millis();
      beat();
    }
  }
  if (now - lastReboot > rebootInterval){
    forceReboot();
  }
}

void forceReboot(){
  cli();
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while(1);
}
void wakeUp(){
}

void police() {
  if (p_state){
    fill_solid(&leds[10], 10, CRGB::Blue);
    fill_solid(leds, 10, CRGB::Black);
    FastLED.show();}
  else{
    fill_solid(&leds[10], 10, CRGB::Black);
    fill_solid(leds, 10, CRGB::Red);
    FastLED.show();
  }
}
void rainbow(){
  static uint8_t hue = 0;
  fill_rainbow(leds, NUM_LEDS, hue, 255 / NUM_LEDS);
  FastLED.show();
  hue++;
}
void black(){
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}
void fade(){
  static uint8_t brightness = 0;
  static uint8_t direction = 1;
  brightness+=direction;
  if (brightness > brr){
    brightness = brr;
  }
  if (brightness == 0 || brightness == brr){
    direction = -direction;
  }
  fill_solid(leds, NUM_LEDS, colors[color_now]);
  FastLED.setBrightness(brightness);
  FastLED.show();
}
void cylon(){
  static uint8_t pos = 0;
  static int8_t direction = 1;

    // Fade all LEDs
  fadeToBlackBy(leds, NUM_LEDS, 50);

    // Set current position
  leds[pos] = colors[color_now];

    // Move position
  pos += direction;
  if (pos == 0 || pos == NUM_LEDS - 1) {
      direction = -direction;
  }
    FastLED.show();
}
void left(){
  if (blinker_state){
    fill_solid(&leds[5], 15, CRGB::Black);
    fill_solid(leds, 5, colors[6]);
    FastLED.show();
  }
  else{
    fill_solid(leds, 20, CRGB::Black);
    FastLED.show();
  }
  blinker_state = !blinker_state;
}
void right(){
  if (blinker_state){
    fill_solid(&leds[15], 5, colors[6]);
    fill_solid(leds, 15, CRGB::Black);
    FastLED.show();
  }
  else{
    fill_solid(leds, 20, CRGB::Black);
    FastLED.show();
  }
  blinker_state = !blinker_state;
}
void danger(){
  if (blinker_state){
    fill_solid(leds, 5, colors[6]);
    fill_solid(&leds[15], 5, colors[6]);
    FastLED.show();
  }
  else{
    fill_solid(leds, 20, CRGB::Black);
    FastLED.show();
  }
  blinker_state = !blinker_state;
}
void twinkle(){
  int index = random(NUM_LEDS);
  leds[index] = colors[color_now];
  fadeToBlackBy(leds, NUM_LEDS, 35);
  FastLED.show();
}
void new_year(){
  static uint8_t brightness = 0;
  static int direction = 1;
  brightness+=direction;
  if (brightness > brr){
    brightness = brr;
  }
  if (brightness == 0){
    direction = -direction;
    n_state = !n_state;
  }
  else if (brightness == brr){
    direction = -direction;
  }
  if (n_state){
    for (int i = 0; i<NUM_LEDS; i++){
      if (i%4==0){
        leds[i] = colors[9];
      }else if (i%4==1){
        leds[i] = colors[5];
      }else{
        leds[i] = CRGB::Black;
      }
    }
  }
  else{
    for (int i = 0; i<NUM_LEDS; i++){
      if (i%4==2){
        leds[i] = colors[3];
      }else if (i%4==3){
        leds[i] = colors[4];
      }else{
        leds[i] = CRGB::Black;
      }
    }
  }
  FastLED.setBrightness(brightness);
  FastLED.show();
}
void new_year2(){
  static uint8_t brightness = 0;
  static int direction = 2;
  if (brightness > brr){
    brightness = brr;
  }
  if (brightness == 0 or brightness == brr){
    direction = -direction;
  }
  for(int i = 0; i < NUM_LEDS; i++){
    if (i%4==0){
      leds[i] = colors[9];
    }else if (i%4==1){
      leds[i] = colors[3];
    }else if (i%4==2){
      leds[i]= colors[4];
    }else {
      leds[i] = colors[5];
    }
  }
  FastLED.setBrightness(brightness);
  brightness+=direction;
  FastLED.show();
}
void goyda(){
  static uint8_t pos = 0;
  static int8_t direction = 1;

    // Fade all LEDs
  fadeToBlackBy(leds, NUM_LEDS, 50);

    // Set current position
  leds[pos] = CRGB::Red;
  leds[pos+7] = CRGB::Blue;
  leds[pos+14] = CRGB::White;

    // Move position
  pos += direction;
  if (pos == 0 || pos == 7) {
      direction = -direction;
  }
    FastLED.show();
}
void even(){
  e_state = !e_state;
  for (int i = 0; i<NUM_LEDS; i++){
    if (e_state){
      if (i%2==0){
        leds[i] = colors[color_now];
      }
      else{
        leds[i] = CRGB::Black;
      }
    }
    else{
      if (i%2==0){
        leds[i] = CRGB::Black;
      }
      else{
        leds[i] =colors[color_now];
      }
    }
  }
  FastLED.show();
}
void pixel(){
  fill_solid(leds, NUM_LEDS, colors[color_now]);
  FastLED.setBrightness(brr);
  FastLED.show();
}
void beat(){
  int raw = analogRead(ANALOG_PIN);
  dc = dc * 0.999 + raw * 0.001;
  float sig = abs(raw - dc);
  if (sig > env){
    env = env * 0.2 + sig * 0.8;
  }
  else{
    env = env * 0.9 + sig * 0.1;
  }

  noiseFloor = noiseFloor * 0.995 + env * 0.005;
  float clean = env - noiseFloor;
  peak*=0.995;
  if (clean < 0) clean = 0;
  if (clean > peak) peak = clean;

  int level = (clean / peak) * 255;
  level = constrain(level, 0, 255);
  now = millis();
  if (level >  30 and lasthit - now > 30){
    lasthit = millis();
    fill_solid(leds, NUM_LEDS, colors[color_now]);
    FastLED.show();
    delay(7);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
}

void process_cmd(char* buff){
  if (buff[0] == 's'){
    wdt_disable();
    delay(1000);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    delay(1000);
    forceReboot();
  }
  else if (buff[0] == 'c' and buff[1] == 'l'){
    calibr();
  }
  //настройка яркости
  else if (buff[0] == 'b' && buff[1] == 'r'){
    char tmp = buff[2];
    int num = tmp - '0';
    brr = brightness_level[num];
    FastLED.setBrightness(brightness_level[num]);
    FastLED.show();
  }
  else if (buff[0] == 'r'){
    forceReboot();
  }
  else if (buff[0] == '*'){
    //fade с выбором цвета
    if (buff[1]== '1'){
      animation = 'f';
      color_now = buff[2]-'0';
    }
    //chase cylon
    else if (buff[1] == '2'){
      animation = 'c';
      color_now = buff[2]-'0';
    }
    //rainbow
    else if (buff[1] == '3'){
      if (buff[2] == '3'){
        animation = 'z';
        color_now = buff[3] - '0';
      }
      else{
        animation = 'r';
      }
    }
    //random
    else if (buff[1] == '4'){
      animation = 't';
      color_now = buff[2]-'0';
    }
    //new year
    else if (buff[1] == '5'){
      if (buff[2] == '5'){
        animation = 'e';
        color_now = buff[3]-'0';
      }
      else{
        animation = 'n';
      }
    }
    //n2
    else if (buff[1] == '6'){
      animation = 'y';
    }
    //left blinker and danger
    else if (buff[1] == '7'){
      if (buff[2] == '8'){
        animation = 'd';
      }
      else{
        animation = 'l';
      }
    }
    //right blinker
    else if (buff[1] == '8'){
      animation = 'a';
    }
    //police
    else if (buff[1] == '9'){
      if (buff[2] == '9'){
        animation = 'g';
      }
      else{
        animation = 'p';
      }
    }
    //black
    else if (buff[1] == '0'){
      animation = 'b';
    }
  }
}

void calibr(){
  wdt_reset();
  unsigned long start = millis();
  float envTemp = 0;
  while (millis() - start < 10000){
    int raw = analogRead(ANALOG_PIN);
    dc = dc * 0.990 + raw * 0.001;
    float sig = abs(raw - dc);
    envTemp = envTemp * 0.9 + sig * 0.1;
    if (envTemp < noiseFloor + 2){
      noiseFloor = noiseFloor * 0.98 + envTemp * 0.02;
    }
    if (envTemp > peak && envTemp > 200){
      peak = envTemp;
    }
    delay(5);
  }
  if (noiseFloor < 2) noiseFloor = 2;
  if (peak < noiseFloor + 10) peak = noiseFloor + 10;
}