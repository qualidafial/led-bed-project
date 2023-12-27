#include <FastLED.h>

//// Holidays
CRGB newYearsRaw[] = {CRGB::Orange, CRGB::Purple, CRGB::White};
CRGBSet newYears(newYearsRaw, sizeof(newYearsRaw) / sizeof(CRGB));

CRGB valentinesDayRaw[] = {CRGB::DeepPink, CRGB::White, CRGB::Red};
CRGBSet valentinesDay(valentinesDayRaw, sizeof(valentinesDayRaw) / sizeof(CRGB));

CRGB saintPatricksDayRaw[] = {CRGB::DarkGreen, CRGB::Gold};
CRGBSet saintPatricksDay(saintPatricksDayRaw, sizeof(saintPatricksDayRaw) / sizeof(CRGB));

CRGB usFlagRaw[] = {CRGB::Red, CRGB::White, CRGB::Blue};
CRGBSet usFlag(usFlagRaw, sizeof(usFlagRaw) / sizeof(CRGB));

CRGB halloweenRaw[] = {CRGB::Orange, CRGB::Green, CRGB::Purple};
CRGBSet halloween(halloweenRaw, sizeof(halloweenRaw) / sizeof(CRGB));

CRGB thanksgivingRaw[] = {CRGB::OrangeRed, CRGB::Orange, CRGB::SaddleBrown};
CRGBSet thanksgiving(thanksgivingRaw, sizeof(thanksgivingRaw) / sizeof(CRGB));

CRGB christmasRaw[] = {CRGB::Red, CRGB::Green};
CRGBSet christmas(christmasRaw, sizeof(christmasRaw) / sizeof(CRGB));

//// Pride

CRGB prideFlagClassicRaw[] = {0xe50000, 0xff8d00, 0xffee00, 0x028121, 0x004cff, 0x760088};
CRGBSet prideFlagClassic(prideFlagClassicRaw, sizeof(prideFlagClassicRaw) / sizeof(CRGB));

CRGB bisexualFlagRaw[] = {0xd60270, 0xd60270, 0x9b4f96, 0x0038a8, 0x0038a8};
CRGBSet bisexualFlag(bisexualFlagRaw, sizeof(bisexualFlagRaw) / sizeof(CRGB));

CRGB asexualFlagRaw[] = {0x000000, 0xa4a4a4, 0xffffff, 0x810081};
CRGBSet asexualFlag(asexualFlagRaw, sizeof(asexualFlagRaw) / sizeof(CRGB));

CRGB nonbinaryFlagRaw[] = {0xfcf431, 0xfcfcfc, 0x9d59d2, 0x282828};
CRGBSet nonbinaryFlag(nonbinaryFlagRaw, sizeof(nonbinaryFlagRaw) / sizeof(CRGB));

CRGB transgenderFlagRaw[] = {0x5bcffb, 0xf5abb9, 0xffffff, 0xf5abb9, 0x5bcffb};
CRGBSet transgenderFlag(transgenderFlagRaw, sizeof(transgenderFlagRaw) / sizeof(CRGB));

CRGB genderqueerFlagRaw[] = {0xb57fdd, 0xb57fdd, 0xffffff, 0xffffff, 0x49821e, 0x49821e};
CRGBSet genderqueerFlag(genderqueerFlagRaw, sizeof(genderqueerFlagRaw) / sizeof(CRGB));
