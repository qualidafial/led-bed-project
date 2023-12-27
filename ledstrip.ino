#include <Arduino.h>
#include <FastLED.h>

#include "colors.h"
#include "assets.h"

#define LED_DATA_PIN 2
#define MAX_MILLIWATTS (5 * 1000) // V * mA

#define RF_RECEIVER_VI_PIN 13
#define RF_RECEIVER_D0_PIN 12
#define RF_RECEIVER_D1_PIN 11
#define RF_RECEIVER_D2_PIN 10
#define RF_RECEIVER_D3_PIN 9
#define RF_RECEIVER_VT_PIN 8

#define BRIGHTNESS_PIN RF_RECEIVER_D3_PIN
#define SOLID_COLOR_PIN RF_RECEIVER_D2_PIN
#define ANIMATION_PIN RF_RECEIVER_D1_PIN
#define PRIDE_FLAG_PIN RF_RECEIVER_D0_PIN

const int headerLeftHighStart = 0;
const int headerRightHigh = headerLeftHighStart + 37;
const int headerRightLow = headerRightHigh + 44;
const int footerRight = headerRightLow + 60;
const int footerLeft = footerRight + 37;
const int headerLeftLow = footerLeft + 60;
const int headerLeftHighEnd = headerLeftLow + 44;
const int NUM_LEDS = headerLeftHighEnd;

CRGB rawleds[NUM_LEDS];

CRGBSet leds(rawleds, NUM_LEDS);

// LEDs are arranged clockwise around a bed, with the Arduino tucked behind the
// left side of the headboard.

// The header starts at the left, and flows to the right. LEDs are pointing up.
CRGBSet header = leds(headerLeftHighStart, headerRightHigh - 1);

// The header right hand post has pixels flowing from top to bottom. LEDs are
// pointing right.
CRGBSet headerRightHandPost = leds(headerRightHigh, headerRightLow - 1);

// The right side has pixels flowing from right to left. LED order is reversed
// so that patterns flow from left to right. LEDs are pointing down.
CRGBSet rightSide = leds(footerRight - 1, headerRightLow);

// The footer has pixels flowing from right to left. LED order is reversed so
// that patterns flow from left to right. LEDs are pointing down.
CRGBSet footer = leds(footerLeft - 1, footerRight);

// The left side has pixels flowing from right to left. LED order is reversed
// so that patterns flow from left to right. LEDs are pointing down.
CRGBSet leftSide = leds(headerLeftLow - 1, footerLeft);

// The header left hand post has pixels flowing from bottom to top. LED order
// is reversed so that patterns flow from top to bottom. LEDs are pointing
// left.
CRGBSet headerLeftHandPost = leds(headerLeftHighEnd - 1, headerLeftLow);

// Whether all LEDs are off. Initially true, until a button is pressed. Changes
// to false when 4 buttons are pressed on the remote. Changes back to true when
// brightness button is held for at least 1 second.
bool off = true;

// Whether the brightness pin was high the last time we checked. True when
// button 1 is held on the remote.
bool brightnessButton;

// The last time the brightness button went from low to high.
uint32_t brightnessSince;

// Whether the solid color pin was high the last time we checked. True when
// button 2 is held on the remote.
bool solidButton;

// Whether the animation pin was high the last time we checked. True when
// button 3 is held on the remote.
bool animationButton;

// Whether the pride flag pin was high the last time we checked. True when
// button 4 is held on the remote.
bool prideFlagButton;

// Program

enum Brightness
{
  BRIGHTNESS_FULL,
  BRIGHTNESS_3,
  BRIGHTNESS_2,
  BRIGHTNESS_1,
  BRIGHTNESS_NIGHT_LIGHT,

  BRIGHTNESS_COUNT
};

Brightness brightness = BRIGHTNESS_FULL;

enum Mode
{
  MODE_SOLID_COLOR,
  MODE_ANIMATION,
  MODE_PRIDE_FLAG,
};

Mode mode = MODE_SOLID_COLOR;

int solidColorIndex = 0;

enum Animation
{
  ANIMATION_RAINBOW_CHASE,
  ANIMATION_NEW_YEARS_DAY,
  ANIMATION_VALENTINES_DAY,
  ANIMATION_SAINT_PATRICKS_DAY,
  ANIMATION_INDEPENDENCE_DAY,
  ANIMATION_HALLOWEEN,
  ANIMATION_THANKSGIVING,
  ANIMATION_CHRISTMAS,

  ANIMATION_COUNT
};

Animation animation = ANIMATION_RAINBOW_CHASE;

// The current time as of the beginning of each call to loop()
unsigned long tickTime;

// The frame number of the currently selected animation. Each animation can
// have a different number of frames befores it loops.
unsigned int frame = 0;

// The number of milliseconds between each frame of the animation. Each
// animation can have its own frame delay.
unsigned int frameDelay = 1;

// When the next frame should be drawn. Always set to tickTime + frameDelay
// after drawing the current frame.
unsigned long nextFrame;

enum PrideFlag
{
  PRIDE_FLAG_CLASSIC,
  PRIDE_FLAG_ASEXUAL,
  PRIDE_FLAG_GENDERQUEER,
  PRIDE_FLAG_NONBINARY,
  PRIDE_FLAG_BISEXUAL,
  PRIDE_FLAG_TRANSGENDER,

  PRIDE_FLAG_COLLAGE,

  PRIDE_FLAG_COUNT
};

PrideFlag prideFlag = PRIDE_FLAG_CLASSIC;

void setup()
{
  Serial.begin(115200);

  // Set up FastLED for the LED strip
  pinMode(LED_DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInMilliWatts(MAX_MILLIWATTS);
  FastLED.setCorrection(UncorrectedColor);
  FastLED.setTemperature(WarmFluorescent);

  // Set up pins for the RF receiver for the remote.
  pinMode(RF_RECEIVER_VI_PIN, OUTPUT);
  pinMode(RF_RECEIVER_D0_PIN, INPUT);
  pinMode(RF_RECEIVER_D1_PIN, INPUT);
  pinMode(RF_RECEIVER_D2_PIN, INPUT);
  pinMode(RF_RECEIVER_D3_PIN, INPUT);
  pinMode(RF_RECEIVER_VT_PIN, INPUT);
  digitalWrite(RF_RECEIVER_VI_PIN, HIGH);

  test();

  turnOff();
}

void test()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::White;
    FastLED.show();
    leds[i] = CRGB::Black;
  }
}

void loop()
{
  tickTime = millis();

  if (buttonPressed(BRIGHTNESS_PIN, &brightnessButton))
  {
    brightnessSince = tickTime;
    changeBrightness();
  }
  else if (brightnessButton && (tickTime - brightnessSince > 1000))
  {
    // Hold brightness for 1 second: off
    turnOff();
  }

  if (buttonPressed(SOLID_COLOR_PIN, &solidButton))
  {
    changeSolidColor();
  }

  if (buttonPressed(ANIMATION_PIN, &animationButton))
  {
    changeAnimation();
  }

  if (buttonPressed(PRIDE_FLAG_PIN, &prideFlagButton))
  {
    changePrideFlag();
  }

  tick();

  FastLED.show();
}

bool buttonPressed(uint8_t pin, bool *state)
{
  bool previous = *state;
  bool current = digitalRead(pin);
  *state = current;
  return current && !previous;
}

void changeBrightness()
{
  if (off)
  {
    off = false;
    return;
  }
  setBrightness(Brightness((brightness + 1) % BRIGHTNESS_COUNT));
}

void setBrightness(Brightness b)
{
  brightness = b;

  switch (brightness)
  {
  case BRIGHTNESS_FULL:
    Serial.println("Brightness: 100%");
    // FastLED.setBrightness(0xff);
    FastLED.setMaxPowerInMilliWatts(MAX_MILLIWATTS);
    break;
  case BRIGHTNESS_3:
    Serial.println("Brightness: 75%");
    // FastLED.setBrightness(0xc0);
    FastLED.setMaxPowerInMilliWatts(3 * MAX_MILLIWATTS / 4);
    break;
  case BRIGHTNESS_2:
    Serial.println("Brightness: 50%");
    // FastLED.setBrightness(0x80);
    FastLED.setMaxPowerInMilliWatts(MAX_MILLIWATTS / 2);
    break;
  case BRIGHTNESS_1:
    Serial.println("Brightness: 25%");
    // FastLED.setBrightness(0x40);
    FastLED.setMaxPowerInMilliWatts(MAX_MILLIWATTS / 4);
    break;
  case BRIGHTNESS_NIGHT_LIGHT:
    Serial.println("Brightness: night light");
    // FastLED.setBrightness(0x10);
    FastLED.setMaxPowerInMilliWatts(MAX_MILLIWATTS / 16);
    break;
  }
}

void turnOff()
{
  Serial.println("Turn off.");
  off = true;
}

void changeSolidColor()
{
  if (off)
  {
    off = false;
    if (mode == MODE_SOLID_COLOR)
    {
      return;
    }
  }

  if (mode == MODE_SOLID_COLOR)
  {
    solidColorIndex = (solidColorIndex + 1) % NUM_SOLID_COLORS;
  }
  else
  {
    Serial.println("Mode: solid color");
    mode = MODE_SOLID_COLOR;
  }
  Serial.print("Solid color: ");
  Serial.println(solidColorIndex);
}

void changeAnimation()
{
  if (off)
  {
    off = false;
    if (mode == MODE_ANIMATION)
    {
      return;
    }
  }

  if (mode == MODE_ANIMATION)
  {
    animation = Animation((animation + 1) % ANIMATION_COUNT);
  }
  else
  {
    Serial.println("Mode: animation");
    mode = MODE_ANIMATION;
    frame = 0;
  }
  nextFrame = tickTime;
  Serial.print("Animation: ");
  Serial.println(animation);
}

void changePrideFlag()
{
  if (off)
  {
    off = false;
    if (mode == MODE_PRIDE_FLAG)
    {
      return;
    }
  }
  else if (mode == MODE_PRIDE_FLAG)
  {
    prideFlag = PrideFlag((prideFlag + 1) % PRIDE_FLAG_COUNT);
  }
  else
  {
    Serial.println("Mode: pride flag");
    mode = MODE_PRIDE_FLAG;
  }
  Serial.print("Pride flag: ");
  Serial.println(prideFlag);
}

void tick()
{
  if (off)
  {
    leds = CRGB::Black;
    FastLED.show();
    return;
  }

  switch (mode)
  {
  case MODE_SOLID_COLOR:
    solid();
    break;
  case MODE_ANIMATION:
    animate();
    break;
  case MODE_PRIDE_FLAG:
    drawPrideFlag();
    break;
  }

  FastLED.show();
}

void solid()
{
  leds = solidColors[solidColorIndex];
}

void animate()
{
  if (tickTime < nextFrame)
  {
    return;
  }

  switch (animation)
  {
  case ANIMATION_RAINBOW_CHASE:
    rainbow();
    break;
  case ANIMATION_NEW_YEARS_DAY:
    colorChase(newYears, 20);
    frameDelay = 100;
    break;
  case ANIMATION_VALENTINES_DAY:
    colorChase(valentinesDay, 20);
    frameDelay = 100;
    break;
  case ANIMATION_SAINT_PATRICKS_DAY:
    colorChase(saintPatricksDay, 20);
    frameDelay = 100;
    break;
  case ANIMATION_INDEPENDENCE_DAY:
    colorChase(usFlag, 20);
    frameDelay = 100;
    break;
  case ANIMATION_HALLOWEEN:
    colorChase(halloween, 20);
    frameDelay = 100;
    break;
  case ANIMATION_THANKSGIVING:
    colorChase(thanksgiving, 20);
    frameDelay = 100;
    break;
  case ANIMATION_CHRISTMAS:
    colorChase(christmas, 20);
    frameDelay = 100;
    break;
  default:
    turnOff();
    break;
  }

  nextFrame = tickTime + frameDelay;
}

#define RAINBOW_HUE_CHANGE_PER_FRAME 1
#define RAINBOW_FRAME_DELAY 1

void rainbow()
{
  frame %= 256;
  fill_rainbow_circular(leds, NUM_LEDS, frame);
  frame = (frame + RAINBOW_HUE_CHANGE_PER_FRAME) % 256;
  frameDelay = RAINBOW_FRAME_DELAY;
}

void colorChase(CRGBSet src, int scale)
{
  const int pattern_width = src.size() * scale;
  if (pattern_width <= 0)
  {
    return;
  }

  leds = CRGB::Black;
  fillCycle(leds, src, scale);

  frame = (frame + 1) % (pattern_width);
}

void fillCycle(CRGBSet dst, CRGBSet src, int scale)
{
  const int pattern_width = src.size() * scale;
  if (pattern_width <= 0)
  {
    return;
  }

  frame %= pattern_width;

  for (int color_index = 0; color_index < src.size(); color_index++)
  {
    CRGB color = src[color_index];

    for (int group_start = color_index * scale - frame; group_start < dst.size(); group_start += pattern_width)
    {
      if (group_start + scale <= 0)
      {
        continue;
      }

      int segment_start = max(group_start, 0);
      int segment_end = min(group_start + scale, dst.size()) - 1;
      for (int i = segment_start; i <= segment_end; i++)
      {
        dst[i] = color;
      }
    }
  }
}

void drawPrideFlag()
{
  leds = CRGB::Black;

  switch (prideFlag)
  {
  case PRIDE_FLAG_ASEXUAL:
    fillAutoScaleEverySide(asexualFlag);
    break;
  case PRIDE_FLAG_BISEXUAL:
    fillAutoScaleEverySide(bisexualFlag);
    break;
  case PRIDE_FLAG_CLASSIC:
    fillAutoScaleEverySide(prideFlagClassic);
    break;
  case PRIDE_FLAG_GENDERQUEER:
    fillAutoScaleEverySide(genderqueerFlag);
    break;
  case PRIDE_FLAG_NONBINARY:
    fillAutoScaleEverySide(nonbinaryFlag);
    break;
  case PRIDE_FLAG_TRANSGENDER:
    fillAutoScaleEverySide(transgenderFlag);
    break;
  case PRIDE_FLAG_COLLAGE:
    fillAutoScale(rightSide, genderqueerFlag);
    fillAutoScale(footer, asexualFlag);
    fillAutoScale(leftSide, nonbinaryFlag);
    fillAutoScale(headerLeftHandPost, transgenderFlag);
    fillAutoScale(header, prideFlagClassic);
    fillAutoScale(headerRightHandPost, bisexualFlag);
    break;
  }
}

void fillAutoScaleEverySide(CRGBSet src)
{
  fillAutoScale(rightSide, src);
  fillAutoScale(footer, src);
  fillAutoScale(leftSide, src);
  fillAutoScale(headerLeftHandPost, src);
  fillAutoScale(header, src);
  fillAutoScale(headerRightHandPost, src);
}

void fillAutoScale(CRGBSet dst, CRGBSet src)
{
  if (src.size() == 0)
  {
    Serial.println("fillAutoScale: source pixel set is empty");
    return;
  }

  if (dst.size() == 0)
  {
    Serial.println("fillAutoScale: destination pixel set is empty");
    return;
  }

  for (int i = 0; i < src.size(); i++)
  {
    CRGB color = src[i];
    int dstFirst = i * dst.size() / src.size();
    int dstLast = (i + 1) * dst.size() / src.size() - 1;

    // FastLED has a bug where sub-ranges on a reverse range do not honor the
    // direction of the parent range. Essentially, subranges on reverse ranges
    // do nothing.

    // dst(dstFirst, dstLast) = color;

    // Instead we have to loop through the indexes one at a time.
    for (int i = dstFirst; i <= dstLast; i++)
    {
      dst[i] = color;
    }
  }
}
