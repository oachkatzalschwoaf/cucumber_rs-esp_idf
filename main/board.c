#include "board.h"

#include "esp_err.h"
#include "i2c_bus.h"

#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "Board";


#define BOARD_IO_I2C0_SCL 3
#define BOARD_IO_I2C0_SDA 1

/*Definations of Peripheral*/
#define BOARD_I2C0_MODE I2C_MODE_MASTER
#define BOARD_I2C0_SPEED (100000)
#define BOARD_I2C0_SCL_PULLUP_EN    true
#define BOARD_I2C0_SDA_PULLUP_EN    true

static bool s_board_is_init = false;

#define BOARD_CHECK(a, str, ret) if(!(a)) { \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str); \
        return (ret); \
    }

static i2c_bus_handle_t s_i2c0_bus_handle = NULL;

static esp_err_t board_i2c_bus_init(void)
{
    i2c_config_t board_i2c0_conf = {
        .mode = BOARD_I2C0_MODE,
        .sda_io_num = BOARD_IO_I2C0_SDA,
        .sda_pullup_en = BOARD_I2C0_SDA_PULLUP_EN,
        .scl_io_num = BOARD_IO_I2C0_SCL,
        .scl_pullup_en = BOARD_I2C0_SCL_PULLUP_EN,
        .master.clk_speed = BOARD_I2C0_SPEED,
    };
    i2c_bus_handle_t handle0 = i2c_bus_create(I2C_NUM_0, &board_i2c0_conf);
    BOARD_CHECK(handle0 != NULL, "i2c_bus0 creat failed", ESP_FAIL);
    s_i2c0_bus_handle = handle0;

    return ESP_OK;
}

static esp_err_t board_i2c_bus_deinit(void)
{
    if (s_i2c0_bus_handle != NULL) {
        i2c_bus_delete(&s_i2c0_bus_handle);
        if (s_i2c0_bus_handle != NULL) {
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

esp_err_t board_init(void)
{
    if(s_board_is_init) {
        return ESP_OK;
    }
    int ret;

    ret = board_i2c_bus_init();
    BOARD_CHECK(ret == ESP_OK, "i2c init failed", ret);

    s_board_is_init = true;
    ESP_LOGI(TAG,"Board Init Done ...");
    return ESP_OK;
}

esp_err_t board_deinit(void)
{
    if(!s_board_is_init) {
        return ESP_OK;
    }
    int ret;

    ret = board_i2c_bus_deinit();
    BOARD_CHECK(ret == ESP_OK, "i2c de-init failed", ret);

    s_board_is_init = false;
    ESP_LOGI(TAG,"Board Deinit Done ...");
    return ESP_OK;
}

bool board_is_init(void)
{
    return s_board_is_init;
}

i2c_bus_handle_t board_get_handle(int id)
{
    i2c_bus_handle_t handle = s_i2c0_bus_handle;
    return handle;
}

