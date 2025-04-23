#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stdint.h>
#include <string.h>

typedef struct
{
   uint8_t starting_value;
   int8_t increment;
} led_iter_t;

typedef struct led_bulb
{
   uint8_t *GRB[3];
   void (*update)(uint8_t green, uint8_t red, uint8_t blue, struct led_bulb *self);
} led_bulb;



typedef struct {
    const char *name;
    uint8_t grb[3];  // Green, Red, Blue
} color_grb_t;

extern const color_grb_t named_colors[];
extern const int NUM_COLORS;

void update_values(uint8_t green, uint8_t red, uint8_t blue, led_bulb *self);

const uint8_t *get_color_grb(char *name, const color_grb_t *colors, size_t arr_size);




#endif