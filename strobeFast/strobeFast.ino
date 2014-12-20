#include <Arduino.h>
#include <TimerOne.h>

#define DPIN_RLED_SW            2
#define DPIN_GLED               5
#define DPIN_PWR                8
#define DPIN_DRV_MODE           9
#define DPIN_DRV_EN             10
// Modes
#define MODE_OFF                0
#define MODE_FADE               1
#define MODE_ON                 2

byte mode;
boolean btnDown;
unsigned long lastTime, btnTime, lastTime_blink;
unsigned long bright = 30000;
int fadeDir;
int flash;
byte bblink;


void setup() {
  pinMode(DPIN_PWR,      INPUT);
  digitalWrite(DPIN_PWR, LOW);

  // Initialize GPIO
  pinMode(DPIN_RLED_SW,  INPUT);
  pinMode(DPIN_GLED,     OUTPUT);
  pinMode(DPIN_DRV_MODE, OUTPUT);
  pinMode(DPIN_DRV_EN,   OUTPUT);
  digitalWrite(DPIN_DRV_MODE, HIGH);
  digitalWrite(DPIN_DRV_EN,   LOW);
  digitalWrite(DPIN_GLED, HIGH);

  mode = MODE_OFF;
  btnDown = digitalRead(DPIN_RLED_SW);
  btnTime = millis();
  
  Timer1.initialize(100000);
  Timer1.attachInterrupt(blinkLed);
  Timer1.stop();
}

void loop()
{
  unsigned long time = millis();
  unsigned long m_time = micros();
  boolean newBtnDown = digitalRead(DPIN_RLED_SW);

  switch (mode)
  {
  case MODE_OFF:
    if (btnDown && newBtnDown && time-btnTime>50)
    {
      mode = MODE_FADE;
      bright = 0;
      fadeDir = 100;
      pinMode(DPIN_PWR, OUTPUT);
      digitalWrite(DPIN_PWR, HIGH);
      Timer1.start();
    }
    break;

  case MODE_FADE:
    Timer1.stop();
    if (time-lastTime > 5) {
      lastTime = time;
      if (fadeDir>0 && bright==300000) fadeDir = -100;
      if (fadeDir<0 && bright==30000) fadeDir =  100;
      bright += fadeDir;
//      Timer1.setPeriod(bright);
      flash = bright / 30;
    }
    if (!btnDown && !newBtnDown && time-btnTime>50)
      mode = MODE_ON;
      Timer1.start();
    break;

  case MODE_ON:
    if (btnDown && newBtnDown && time-btnTime>200)
      mode = MODE_FADE;
    if (btnDown && !newBtnDown) {
      mode = MODE_OFF;
      digitalWrite(DPIN_DRV_EN, LOW);
      pinMode(DPIN_PWR, OUTPUT);
      digitalWrite(DPIN_PWR, LOW);
      Timer1.stop();
    }
    break;
  }

  if (newBtnDown != btnDown)
  {
    btnTime = time;
    btnDown = newBtnDown;
    delay(50);
  }
}

void blinkLed() {
    digitalWrite(DPIN_DRV_EN, HIGH);
    delayMicroseconds(3000);
    digitalWrite(DPIN_DRV_EN, LOW);
}
