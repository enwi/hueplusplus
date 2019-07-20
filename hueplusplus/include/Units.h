#ifndef _UNITS_H
#define _UNITS_H

struct Kelvin {
  int value;
};

struct Mired {
  int value;
};

struct Brightness {
  int value;
};

struct HueSaturation {
  int hue;
  int saturation;
};

struct XY {
  float x;
  float y;
};

struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

#endif
