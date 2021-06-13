#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "board.h"
#include "sensor.h"
#include "led_strip.h"

#include "esp_err.h"
#include "esp_log.h"

#define TAG "MAIN"

// Cucumber board has 1 ws2812B in IO18
#define LED_STRIP_LENGTH 1U
#define LED_STRIP_RMT_INTR_NUM 19U
#define LED_STRIP_GPIO_NUM 18U

static struct led_color_t led_strip_buf_1[LED_STRIP_LENGTH];
static struct led_color_t led_strip_buf_2[LED_STRIP_LENGTH];

static struct led_strip_t led_strip = {
    .rgb_led_type = RGB_LED_TYPE_WS2812,
    .rmt_channel = RMT_CHANNEL_1,
    .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
    .gpio = LED_STRIP_GPIO_NUM,
    .led_strip_buf_1 = led_strip_buf_1,
    .led_strip_buf_2 = led_strip_buf_2,
    .led_strip_length = LED_STRIP_LENGTH
};

void ledInit() {

    led_strip.access_semaphore = xSemaphoreCreateBinary();

    bool led_init_ok = led_strip_init(&led_strip);

    if (led_init_ok) {
        ESP_LOGI(TAG, "%s OK", __func__);
    } else {
        ESP_LOGE(TAG, "%s NOK", __func__);
    }

    led_strip_set_pixel_rgb(&led_strip, 0, 0x99, 0x00, 0x00);
    led_strip_show(&led_strip);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Startup...");
    ledInit();
    sensor_init();

    int i = 0;
    int round = 0;
    sensor_data_t data;
    while(1) {
        printf("\r\n");
        ESP_LOGI(TAG, "****** %4d *****", round++);
        sensorGetAllData(&data);

        ESP_LOGI("SENSOR", "BME280 pressure: %4.1f    temp: %2.1f ", data.bme280_press, data.bme280_temp);
        ESP_LOGI("SENSOR", "HTS221 humidity: %4.1f    temp: %2.1f ", data.hts221_hum, data.hts221_temp);

        ESP_LOGI("MPU6050","acce_x:%.2f, acce_y:%.2f, acce_z:%.2f", data.mpu6050_acce_x, data.mpu6050_acce_y, data.mpu6050_acce_z);
        ESP_LOGI("MPU6050","gyro_x:%.2f, gyro_y:%.2f, gyro_z:%.2f", data.mpu6050_gyro_x, data.mpu6050_gyro_y, data.mpu6050_gyro_z);

        i++;

        if (i == 1) {
            led_strip_set_pixel_rgb(&led_strip, 0, 0x99, 0x00, 0x00);
        } else if(i == 2) {
            led_strip_set_pixel_rgb(&led_strip, 0, 0, 0x99, 0x00);

        } else if (i == 3) {
            led_strip_set_pixel_rgb(&led_strip, 0, 0, 0,  0x99);
            i = 0;
        }
        led_strip_show(&led_strip);

        /*  bme280_print_data();
            hts221_print_data();
            mpu6050_print_data();
        */

        vTaskDelay(1000/portTICK_RATE_MS);
    }
}
