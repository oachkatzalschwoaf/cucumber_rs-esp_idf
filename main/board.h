#ifndef __BOARD_H__
#define __BOARD_H__

#include "esp_err.h"
#include "i2c_bus.h"

esp_err_t board_init(void);
esp_err_t board_deinit(void);

#endif