#include <FastLED.h>
#include <SoftwareSerial.h>
#include <LowPower.h>

#define NUM_LEDS 20
#define LED_PIN 9
#define LED_TYPE SK6812
#define COLOR_ORDER GRB
#define IN_PIN 10

uint32_t now = millis();
uint32_t last = 0;
int delay_anim = 20;
int delay_police = 500;
int delay_cylon = 100;
int delay_twinkle = 150;

char buff[10];
int cnt = 0;
char c = ' ';
char animation = ' ';

bool n_state = false;
bool p_state = false;
bool blinker_state = false;

SoftwareSerial ble(IN_PIN, -1);

int brightness_level[10] ={18,36,54,72,90,108,126,144,162,180};
int brr = 180;

int color_now = 0;
CRGB leds[NUM_LEDS];
CRGB colors[10] = {
  CRGB::Yellow,
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
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(100);

  ble.begin(9600);


  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), wakeUp, RISING);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (ble.available()) {
    c = ble.read();

    if (c == '#'){

      process_cmd(buff);
      memset(buff, '0', sizeof(buff));
      cnt = 0;
    }
    else{
      buff[cnt] = c;
      cnt++;
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
  }
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
    fill_solid(&leds[10], 10, CRGB::Black);
    fill_solid(leds, 10, colors[6]);
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
    fill_solid(&leds[10], 10, colors[6]);
    fill_solid(leds, 10, CRGB::Black);
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
    fill_solid(leds, 20, colors[6]);
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
  leds[index] = colors[random(10)];
  fadeToBlackBy(leds, NUM_LEDS, 35);
  FastLED.show();
}
void new_year(){
  static uint8_t brightness = 0;
  static int direction = 1;
  brightness+=direction;
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
        leds[i] = colors[0];
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
  static int brightness = 1;
  static int direction = 1;
  if (brightness == 0 or brightness == brr){
    direction = -direction;
  }
  for(int i = 0; i < NUM_LEDS; i++){
    if (i%4==0){
      leds[i] = colors[0];
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

void process_cmd(char* buff){
  if (buff[0] == 's'){
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);


  digitalWrite(LED_BUILTIN, HIGH);
  }
  //настройка яркости
  else if (buff[0] == 'b' && buff[1] == 'r'){
    char tmp = buff[2];
    int num = tmp - '0';
    brr = brightness_level[num];
    FastLED.setBrightness(brightness_level[num]);
    FastLED.show();
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
      animation = 'r';
    }
    //random
    else if (buff[1] == '4'){
      animation = 't';
    }
    //new year
    else if (buff[1] == '5'){
      animation = 'n';
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
      animation = 'p';
    }
    //black
    else if (buff[1] == '0'){
      animation = 'b';
    }
  }
}