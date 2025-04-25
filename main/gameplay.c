#include "gameplay.h"

void update_values(const uint8_t *grb, struct led_bulb *self)
{
    *(self->GRB[0]) = grb[0];
    *(self->GRB[1]) = grb[1];
    *(self->GRB[2]) = grb[2];
}

const color_grb_t named_colors[] = {
    {"white", {255, 255, 255}},
    {"silver", {192, 192, 192}},
    {"gray", {128, 128, 128}},
    {"black", {0, 0, 0}},
    {"maroon", {0, 128, 0}},
    {"red", {0, 255, 0}},
    {"orange", {165, 255, 0}},
    {"yellow", {255, 255, 0}},
    {"olive", {128, 128, 0}},
    {"lime", {255, 0, 0}},
    {"green", {128, 0, 0}},
    {"aqua", {255, 0, 255}},
    {"blue", {0, 0, 255}},
    {"navy", {0, 0, 128}},
    {"teal", {128, 0, 128}},
    {"fuchsia", {0, 255, 255}},
    {"purple", {0, 128, 128}},
};

const size_t NUM_COLORS = sizeof(named_colors) / sizeof(named_colors[0]);

const uint8_t *get_color_grb(char *name)
{
    for (int i = 0; i < NUM_COLORS; i++)
    {
        if (strcmp(name, named_colors[i].name) == 0)
        {
            return named_colors[i].grb;
        }
    }
    return NULL;
}

static void display_win_loss(uint8_t count, led_configuration *config, uint8_t *payload, uint8_t for_win)
{
    char color[16];
    switch (for_win)
    {
    case 2:
        strcpy(color, "purple");
        break;
    case 1:
        strcpy(color, "green");
        break;
    case 0:
        strcpy(color, "red");
        break;
    default:
        strcpy(color, "blue"); // helps identify error in logic
    }
    for (int i = 0; i < count; i++)
    {

        memset(payload, 0, config->payload_bytes);
        custom_transmit_led_values(config->tx_channel, config->encoder, payload, config->payload_bytes, config->config);
        for (int j = 0; j < config->led_bulb_count; j += 1)
        {
            const uint8_t *grb = get_color_grb(color);
            config->led_bulbs[j].update(grb, &config->led_bulbs[j]);
        }
        custom_transmit_led_values(config->tx_channel, config->encoder, payload, config->payload_bytes, config->config);
        vTaskDelay(pdMS_TO_TICKS(200));

        memset(payload, 0, config->payload_bytes);
        custom_transmit_led_values(config->tx_channel, config->encoder, payload, config->payload_bytes, config->config);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void transmit_led_values(led_configuration *config, uint8_t *payload)
{
    custom_transmit_led_values(config->tx_channel, config->encoder, payload, config->payload_bytes, config->config);
    memset(payload, 0, config->payload_bytes);
}

void display_win(uint8_t count, led_configuration *config, uint8_t *payload)
{
    display_win_loss(count, config, payload, 1);
}
void display_loss(uint8_t count, led_configuration *config, uint8_t *payload)
{

    display_win_loss(count, config, payload, 0);
}
void display_super_sayan(uint8_t count, led_configuration *config, uint8_t *payload)
{

    display_win_loss(count, config, payload, 2);
}