#include "led_encoder.h"
#include "esp_check.h"
#include "driver/rmt_encoder.h"
#include <stdint.h>
#include "driver/rmt_tx.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// static const char *TAG = "led_encoder";

typedef struct
{
    rmt_encoder_t *copy_encoder;
    rmt_encoder_t *byte_encoder;
    rmt_encoder_t base;
    rmt_symbol_word_t reset_code;
    int state;
} rmt_led_strip_encoder_t;

static size_t encode_data(rmt_encoder_t *encoder, rmt_channel_handle_t tx_channel, const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state)
{

    rmt_led_strip_encoder_t *led_strip = __containerof(encoder, rmt_led_strip_encoder_t, base);
    rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    size_t encoded_symbols = 0;
    switch (led_strip->state)
    {
    case 0:
        // led_strip->byte_encoder->encode(led_strip->byte_encoder, rmt_channel_handle_t tx_channel, const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state)
        encoded_symbols += led_strip->byte_encoder->encode(led_strip->byte_encoder, tx_channel, primary_data, data_size, &session_state);
        if (session_state & RMT_ENCODING_COMPLETE)
        {
            led_strip->state = 1;
        }
        if (session_state & RMT_ENCODING_MEM_FULL)
        {
            state |= RMT_ENCODING_MEM_FULL;
            goto out;
        }

    case 1:
        encoded_symbols += led_strip->copy_encoder->encode(led_strip->copy_encoder, tx_channel, &led_strip->reset_code, sizeof(led_strip->reset_code), &session_state);
        if (session_state & RMT_ENCODING_COMPLETE)
        {
            led_strip->state = RMT_ENCODING_RESET;
            state |= RMT_ENCODING_COMPLETE;
        }
        if (session_state & RMT_ENCODING_MEM_FULL)
        {
            state |= RMT_ENCODING_MEM_FULL;
            goto out;
        }
    }

out:
    *ret_state = state;
    return encoded_symbols;
}

static esp_err_t reset_encoder(rmt_encoder_t *encoder)
{
    rmt_led_strip_encoder_t *led_encoder = __containerof(encoder, rmt_led_strip_encoder_t, base);
    ESP_ERROR_CHECK(rmt_encoder_reset(led_encoder->byte_encoder));
    ESP_ERROR_CHECK(rmt_encoder_reset(led_encoder->copy_encoder));
    led_encoder->state = RMT_ENCODING_RESET;
    return ESP_OK;
}
static esp_err_t del_encoder(rmt_encoder_t *encoder)
{
    rmt_led_strip_encoder_t *led_encoder = __containerof(encoder, rmt_led_strip_encoder_t, base);
    ESP_ERROR_CHECK(rmt_del_encoder(led_encoder->copy_encoder));
    ESP_ERROR_CHECK(rmt_del_encoder(led_encoder->byte_encoder));
    free(encoder);
    return ESP_OK;
}

esp_err_t rmt_custom_led_encoder(rmt_encoder_handle_t *led_encoder, led_strip_encoder_config_t *config)
{
    esp_err_t state = ESP_OK;
    rmt_led_strip_encoder_t *led_strip = rmt_alloc_encoder_mem(sizeof(rmt_led_strip_encoder_t));
    // ESP_GOTO_ON_FALSE(led_encoder && config, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    // rmt_encoder_handle_t copy_encoder;
    // rmt_encoder_handle_t byte_encoder;
    rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = {
            .level0 = 1,
            .duration0 = 0.3 * config->resolution / 1000000, // T0H=0.3us
            .level1 = 0,
            .duration1 = 0.9 * config->resolution / 1000000, // T0L=0.9us
        },
        .bit1 = {
            .level0 = 1,
            .duration0 = 0.9 * config->resolution / 1000000, // T1H=0.9us
            .level1 = 0,
            .duration1 = 0.3 * config->resolution / 1000000, // T1L=0.3us
        },
        .flags.msb_first = 1 // WS2812 transfer bit order: G7...G0R7...R0B7...B0
    };
    rmt_copy_encoder_config_t copy_config = {};

    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&bytes_encoder_config, &led_strip->byte_encoder));
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&copy_config, &led_strip->copy_encoder));

    uint32_t reset_ticks = config->resolution / 1000000 * 50;
    led_strip->reset_code = (rmt_symbol_word_t){
        .level0 = 0,
        .duration0 = reset_ticks,
        .level1 = 0,
        .duration1 = 0, // Don't split it — one clean LOW
    };

    led_strip->base.encode = encode_data;
    led_strip->base.del = del_encoder;
    led_strip->base.reset = reset_encoder;

    *led_encoder = &led_strip->base;
    return state;
}


void custom_transmit_led_values(rmt_channel_handle_t tx_channel, rmt_encoder_handle_t encoder, const void *payload, size_t payload_bytes, const rmt_transmit_config_t *config)
{
    ESP_ERROR_CHECK(rmt_transmit(tx_channel, encoder, payload, payload_bytes, config));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_channel, portMAX_DELAY));
}
