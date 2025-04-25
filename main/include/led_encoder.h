#ifndef LED_ENCODER_H
#define LED_ENCODER_H

#include "driver/rmt_encoder.h"
#include "led_encoder.h"
#include "esp_check.h"
#include "driver/rmt_encoder.h"
#include <stdint.h>
#include "driver/rmt_tx.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


typedef struct {
    uint32_t resolution;
} led_strip_encoder_config_t;


esp_err_t rmt_custom_led_encoder(rmt_encoder_handle_t *led_encoder, led_strip_encoder_config_t *config);

void custom_transmit_led_values(rmt_channel_handle_t tx_channel, rmt_encoder_handle_t encoder, const void *payload, size_t payload_bytes, const rmt_transmit_config_t *config);


#endif 