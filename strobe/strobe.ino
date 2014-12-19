#include <Arduino.h>
#include <TimerOne.h>
/*
  Hexbright demo firmware: Fades
 Andrew Magill  9/2012
 
 1. Hold button down.  Light fades up and down.
 2. Let go of button.  Light holds current brightness.
 3. Press button.  Light turns off.
 */

// Pin assignments
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
int bright, fadeDir; 
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
  
  Timer1.initialize(1000000);
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
      fadeDir = 1;
      pinMode(DPIN_PWR,      OUTPUT);
      digitalWrite(DPIN_PWR, HIGH);
    }
    break;

  case MODE_FADE:
    if (time-lastTime > 1) {
      lastTime = time;
      if (fadeDir>0 && bright==25500) fadeDir = -10;
      if (fadeDir<0 && bright==0  ) fadeDir =  10;
      bright += fadeDir;

      if (m_time-lastTime_blink < bright) break;
      lastTime_blink = m_time;
      bblink = !bblink;
      blinkLed();
      break;
    }
    if (!btnDown && !newBtnDown && time-btnTime>50)
      mode = MODE_ON;
    break;

  case MODE_ON:
    if (btnDown && newBtnDown && time-btnTime>200)
      mode = MODE_FADE;
    if (btnDown && !newBtnDown) {
      mode = MODE_OFF;
      digitalWrite(DPIN_DRV_EN, LOW);
      pinMode(DPIN_PWR,      OUTPUT);
      digitalWrite(DPIN_PWR, LOW);
    }    
    if (m_time-lastTime_blink < bright) break;
    
    lastTime_blink = m_time;
    bblink = !bblink;
    blinkLed();
    break;
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
    delayMicroseconds(100);
    digitalWrite(DPIN_DRV_EN, LOW);
    
}
