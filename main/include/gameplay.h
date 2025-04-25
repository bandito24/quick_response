#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stdint.h>
#include <string.h>
#include "led_encoder.h"


typedef struct led_bulb
{
    uint8_t *GRB[3];
    void (*update)(const uint8_t *grb, struct led_bulb *self);
} led_bulb;

typedef struct
{
    const char *name;
    uint8_t grb[3]; // Green, Red, Blue
} color_grb_t;

typedef struct
{
    rmt_channel_handle_t tx_channel;
    rmt_encoder_handle_t encoder;
    const void *payload;
    size_t payload_bytes;
    const rmt_transmit_config_t *config;
    led_bulb *led_bulbs;
    const size_t led_bulb_count;
} led_configuration;



extern const color_grb_t named_colors[];
extern const size_t NUM_COLORS;

void update_values(const uint8_t *grb, struct led_bulb *self);

void display_win(uint8_t count, led_configuration *config, uint8_t *payload);
void display_loss(uint8_t count, led_configuration *config, uint8_t *payload);
void display_super_sayan(uint8_t count, led_configuration *config, uint8_t *payload);
void transmit_led_values(led_configuration *config, uint8_t *payload);

const uint8_t *get_color_grb(char *name);

#endif