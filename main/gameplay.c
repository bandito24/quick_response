#include "gameplay.h"


void update_values(uint8_t green, uint8_t red, uint8_t blue, led_bulb *self)
{
   *(self->GRB[0]) = green;
   *(self->GRB[1]) = red;
   *(self->GRB[2]) = blue;
}

const color_grb_t named_colors[] = {
    { "white",   {255, 255, 255} },
    { "silver",  {192, 192, 192} },
    { "gray",    {128, 128, 128} },
    { "black",   {  0,   0,   0} },
    { "maroon",  {  0, 128,   0} },
    { "red",     {  0, 255,   0} },
    { "orange",  {165, 255,   0} },
    { "yellow",  {255, 255,   0} },
    { "olive",   {128, 128,   0} },
    { "lime",    {255,   0,   0} },
    { "green",   {128,   0,   0} },
    { "aqua",    {255,   0, 255} },
    { "blue",    {  0,   0, 255} },
    { "navy",    {  0,   0, 128} },
    { "teal",    {128,   0, 128} },
    { "fuchsia", {  0, 255, 255} },
    { "purple",  {  0, 128, 128} },
};

const uint8_t *get_color_grb(char *name, const color_grb_t *colors, size_t arr_size){

}