#include "gameplay.h"


void update_values(uint8_t green, uint8_t red, uint8_t blue, led_bulb *self)
{
   *(self->GRB[0]) = green;
   *(self->GRB[1]) = red;
   *(self->GRB[2]) = blue;
}



