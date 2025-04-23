#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stdint.h>

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


void update_values(uint8_t green, uint8_t red, uint8_t blue, led_bulb *self);


#endif