#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/rmt_tx.h"
#include "esp_check.h"
#include "esp_log.h"
#include <string.h>
#include "driver/rmt_tx.h"
#include "driver/rmt_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "led_encoder.h"
#include <stdbool.h>
#include "gameplay.h"
#include "esp_system.h"

#define RMT_LED_STRIP_RESOLUTION_HZ 10000000
#define GPIO_LED GPIO_NUM_4
#define GPIO_BTN GPIO_NUM_13
#define LEDS 8

static uint8_t led_strip_pixels[LEDS * 3];
volatile bool button_click = false;
const uint16_t starting_speed = 700;
static led_bulb led_bulbs[LEDS];
static int score = 0;

void IRAM_ATTR my_button_isr_handler(void *arg)
{
    button_click = true;
}

void app_main(void)
{

    for (int i = 0; i < LEDS; i++)
    {
        led_bulb next_bulb = {
            .GRB = {
                &led_strip_pixels[i * 3 + 0],
                &led_strip_pixels[i * 3 + 1],
                &led_strip_pixels[i * 3 + 2]},
            .update = update_values};
        led_bulbs[i] = next_bulb;
    }

    gpio_config_t btn_config = {
        .pin_bit_mask = (1ULL << GPIO_BTN),
        .mode = GPIO_MODE_DEF_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE};
    ESP_ERROR_CHECK(gpio_config(&btn_config));

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(GPIO_BTN, my_button_isr_handler, NULL));

    rmt_channel_handle_t led_chan;
    rmt_tx_channel_config_t channel_config = {
        .gpio_num = GPIO_LED,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
        .mem_block_symbols = 64,
        .trans_queue_depth = 4,
        .flags.invert_out = false, // do not invert output signal
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&channel_config, &led_chan));

    rmt_encoder_handle_t led_encoder;
    led_strip_encoder_config_t encoder_config = {
        .resolution = RMT_LED_STRIP_RESOLUTION_HZ};

    ESP_ERROR_CHECK(rmt_custom_led_encoder(&led_encoder, &encoder_config));

    ESP_ERROR_CHECK(rmt_enable(led_chan));

    rmt_transmit_config_t transmit_config = {
        .loop_count = 0};

    ESP_ERROR_CHECK(rmt_encoder_reset(led_encoder));

    led_configuration enc_values = {
        .tx_channel = led_chan,
        .encoder = led_encoder,
        .payload_bytes = sizeof(led_strip_pixels),
        .config = &transmit_config,
        .led_bulbs = led_bulbs,
        .led_bulb_count = LEDS};

    bool super_sayan = false;

    const uint8_t *color_start = get_color_grb("green");
    const uint8_t *color_end = get_color_grb("red");
    const uint8_t *color_match = get_color_grb("blue");

    uint16_t speed = starting_speed;
    while (1)
    {
        if (speed > 100)
        {
            speed = starting_speed - (score * 100);
        }
        else
        {
            speed = 50;
            super_sayan = true;
        }

        uint8_t start = rand() % 8;
        int8_t increment = start % 2 == 0 ? 1 : -1;
        uint8_t finish = ((rand() % 5) + start + 2) % 8; // end index is next to it with a buffer of one

        // starts as same color then moves
        enc_values.led_bulbs[start].update(color_start, &enc_values.led_bulbs[start]);
        enc_values.led_bulbs[finish].update(color_start, &enc_values.led_bulbs[finish]);
        transmit_led_values(&enc_values, led_strip_pixels);
        vTaskDelay(pdMS_TO_TICKS(700));

        transmit_led_values(&enc_values, led_strip_pixels);

        bool leds_match = false;

        for (int i = 1; i < LEDS; i++)
        {

            uint8_t index = ((start + (i * increment)) % LEDS + LEDS) % LEDS;
            if (index == finish)
            {
                leds_match = true;
            }
            else
            {
                leds_match = false;
            }

            enc_values.led_bulbs[index].update(color_start, &enc_values.led_bulbs[index]);

            const uint8_t *result_color = leds_match ? color_match : color_end;

            enc_values.led_bulbs[finish].update(result_color, &enc_values.led_bulbs[finish]);
            transmit_led_values(&enc_values, led_strip_pixels);
            vTaskDelay(pdMS_TO_TICKS(speed));

            if (button_click)
            {
                button_click = false;
                if (leds_match)
                {
                    score += 1;
                    if (super_sayan)
                    {
                        display_super_sayan(score, &enc_values, led_strip_pixels);
                    }
                    else
                    {
                        display_win(score, &enc_values, led_strip_pixels);
                    }
                    ESP_LOGI("status", "you won!!! score is %d", score);
                    goto start_over;
                }
                else
                {
                    break;
                }

                button_click = false;
            }
        }

        // vTaskDelay(pdMS_TO_TICKS(1000));

        speed = starting_speed;
        score = 0;
        super_sayan = false;
        ESP_LOGI("status", "you lost :(");
        display_loss(2, &enc_values, led_strip_pixels);

    start_over:
        continue;
    }
}
