#include <Adafruit_NeoPixel.h>
#include <arduino-timer.h>

//#define DEBUG

#define STAIR_RISERS 8
#define LED_PER_STAIR 5

#define LED_PIN 8
#define BOTTOM_SENSOR 4
#define TOP_SENSOR 5

#define STEP_DELAY 5 // in ms
#define FADE_DELAY 250  // in us

#define CYCLE_WAIT_TIME 250 // in ms

#define TIMEOUT 10000  //in ms, used for timeout

#define BRIGHTNESS 100 //0-100

Adafruit_NeoPixel strip((STAIR_RISERS * LED_PER_STAIR), LED_PIN, NEO_GRB + NEO_KHZ800);
Timer<1> timer;

#ifdef DEBUG
Timer<1> debugTimer;
#endif

bool topSensor = false;
bool bottomSensor = false;

enum From{
  top, bottom
};

struct State{
  bool triggered = false;
  From from = From::bottom;
  int currentRiser = 0;
};

State state;

void setup() {
  strip.begin();
  strip.clear();
  strip.setBrightness(BRIGHTNESS);
  strip.show();


  //Debug
  #ifdef DEBUG
  Serial.begin(19200);
  Serial.println("Startup!");
  debugTimer.every(500, debug);
  #endif

  pinMode(BOTTOM_SENSOR, INPUT);
  pinMode(TOP_SENSOR, INPUT);

}

void loop() {
  getInputs();
  handleInputs();
  handleTimer();

  #ifdef DEBUG
  debugTimer.tick();
  #endif
  
  timer.tick();
}

void handleInputs(){
  if(topSensor){
    if(!state.triggered){
      state.from = From::top;
      fadeIn();
    }else if(state.triggered && state.from == From::bottom){
      fadeOut();
    }
  }
  if(bottomSensor){
    if(!state.triggered){
      state.from = From::bottom;
      fadeIn();
    }else if(state.triggered && state.from == From::top){
      fadeOut();
    }
  }
}

bool fadeOut(){
  if (state.from != From::bottom) {
    state.currentRiser = 0;
    while (state.currentRiser <= STAIR_RISERS) {
      for (int j = 255; j > 0; j--) {
        for (int i = 0; i < LED_PER_STAIR; i++) {
          strip.setPixelColor((state.currentRiser * LED_PER_STAIR) + i, j, j, j);
        }
        strip.show();
        delayMicroseconds(FADE_DELAY);
      }
      state.currentRiser++;
      strip.show();
      delay(STEP_DELAY);
    }
  } else {
    state.currentRiser = STAIR_RISERS;
    while (state.currentRiser >= 0) {
      for (int j = 255; j > 0; j--) {
        for (int i = 0; i < LED_PER_STAIR; i++) {
          strip.setPixelColor((state.currentRiser * LED_PER_STAIR) + i, j, j, j);
        }
        strip.show();
        delayMicroseconds(FADE_DELAY);
      }
      state.currentRiser--;
      strip.show();
      delay(STEP_DELAY);
    }
  }
  state.triggered = false;
  return true;
}


bool fadeIn(){
  state.triggered = true;
  if (state.from == From::bottom) {
    state.currentRiser = 0;
    while (state.currentRiser <= STAIR_RISERS) {
      for (int j = 1; j < 255; j++) {
        for (int i = 0; i < LED_PER_STAIR; i++) {
          strip.setPixelColor((state.currentRiser * LED_PER_STAIR) + i, j, j, j);
        }
        strip.show();
        delayMicroseconds(FADE_DELAY);
      }
      state.currentRiser++;
      strip.show();
      delay(STEP_DELAY);
    }
  } else {
    state.currentRiser = STAIR_RISERS;
    while (state.currentRiser >= 0) {
      for (int j = 1; j < 255; j++) {
        for (int i = 0; i < LED_PER_STAIR; i++) {
          strip.setPixelColor((state.currentRiser * LED_PER_STAIR) + i, j, j, j);
        }
        strip.show();
        delayMicroseconds(FADE_DELAY);
      }
      state.currentRiser--;
      strip.show();
      delay(STEP_DELAY);
    }
  }
  return true;
}

bool timeout(){
  fadeOut();
  return true;
}

void handleTimer(){
  if(state.triggered && timer.empty()){
    timer.in(TIMEOUT, timeout);
  }else{
    if(!timer.empty() && !state.triggered){
      timer.cancel();
    }
  }
}

void getInputs() {
  topSensor = digitalRead(TOP_SENSOR) == HIGH;
  bottomSensor = digitalRead(BOTTOM_SENSOR) == HIGH;
}

#ifdef DEBUG
void debug(){

  Serial.println("Inputs:");
  Serial.print("topSensor: ");
  Serial.print(topSensor);
  Serial.print("\nbottomSensor: ");
  Serial.print(bottomSensor);

  
  Serial.println("\n\nState:");
  
  Serial.print("triggered: ");
  Serial.print(state.triggered);
  Serial.print("\n");

  Serial.print("currentRiser: ");
  Serial.print(state.currentRiser);
  Serial.print("\n");

  Serial.print("from: ");
  switch (state.from){
    case From::top:
      Serial.print("top\n");
      break;
    case From::bottom:
      Serial.print("bottom\n");
      break;
    default:
      Serial.print("unknown\n");
      break;
  }

  if(timer.empty()){
    Serial.println("timer: empty");
  }else{
    Serial.println("timer: !empty");
  }
  Serial.println();
}
#endif
