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
#include "led_encoder.h"
#include <stdbool.h>
#include "gameplay.h"

#define RMT_LED_STRIP_RESOLUTION_HZ 10000000
#define GPIO_LED GPIO_NUM_4
#define GPIO_BTN GPIO_NUM_13
#define LEDS 8

static uint8_t led_strip_pixels[LEDS * 3];
static char *TAG = "BUTTON";
static bool forward = true;
static led_bulb led_bulbs[LEDS];

void IRAM_ATTR my_button_isr_handler(void *arg)
{
   forward = !forward;
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

   led_iter_t iter = {.starting_value = 0};

   while (1)
   {

      bool current_val = forward;
      iter.increment = forward ? 1 : -1;

      for (int j = 0; j < LEDS; j += 1)
      {
         uint8_t i = iter.starting_value + (j * iter.increment);
         i = i % LEDS;
         led_bulbs[i].update(43, 138, 226, &led_bulbs[i]);

         custom_transmit_led_values(100, led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &transmit_config);
         memset(led_strip_pixels, 0, sizeof(led_strip_pixels));
         custom_transmit_led_values(100, led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &transmit_config);

         if (forward != current_val)
         {
            iter.starting_value = i;
            ESP_LOGI(TAG, "direction changed");
            break;
         }
      }
   }
}
