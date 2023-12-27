#include <FastLED.h>

CRGB solidColors[] = {
    CRGB::Red,
    CRGB::Yellow,
    CRGB::Green,
    CRGB::Brown,
    0xff2400, // scarlet
    CRGB::Black,
    0xcc7722, // ochre
    0xffe5b4, // peach
    0xe0115f, // ruby
    CRGB::Olive,
    CRGB::Violet,
    0xe5aa70, // fawn
    0xc8a2c8, // lilac
    CRGB::Gold,
    CRGB::Chocolate,
    0xe0b0ff, // mauve
    0xfffdd0, // cream
    CRGB::Crimson,
    CRGB::Silver,
    0xff007f, // rose
    0xb2beb5, // ash
    0xfff700, // lemon
    0x80461b, // russet
    CRGB::Gray,
    CRGB::Purple,
    CRGB::White,
    CRGB::Pink,
    CRGB::Orange,
    CRGB::Blue,
};

const int NUM_SOLID_COLORS = sizeof(solidColors) / sizeof(CRGB);
