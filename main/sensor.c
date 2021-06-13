#include "sensor.h"

#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "bme280.h"
#include "hts221.h"
#include "mpu6050.h"


#define I2C_MASTER_SCL_IO           40          /*!< gpio number for I2C master clock IO21*/
#define I2C_MASTER_SDA_IO           41          /*!< gpio number for I2C master data  IO15*/
#define I2C_MASTER_NUM              I2C_NUM_1   /*!< I2C port number for master bme280 */
#define I2C_MASTER_TX_BUF_DISABLE   0           /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0           /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ          100000      /*!< I2C master clock frequency */

static i2c_bus_handle_t i2c_bus = NULL;

static bme280_handle_t bme280 = NULL;
static hts221_handle_t hts221 = NULL;
static mpu6050_handle_t mpu6050 = NULL;

void sensor_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_bus = i2c_bus_create(I2C_MASTER_NUM, &conf);

    // BME280
    bme280 = bme280_create(i2c_bus, BME280_I2C_ADDRESS_DEFAULT);
    ESP_LOGI("BME280:", "bme280_default_init:%d", bme280_default_init(bme280));
    bme280_print_data();

    uint8_t hts221_deviceid;
    hts221 = hts221_create(i2c_bus, HTS221_I2C_ADDRESS);
    hts221_get_deviceid(hts221, &hts221_deviceid);
    ESP_LOGI("HTS221", "hts221 device ID is: 0x%02x", hts221_deviceid);
    hts221_config_t hts221_config;
    hts221_get_config(hts221, &hts221_config);
    /*
    ESP_LOGI("HTS221", "avg_h is: %02x", hts221_config.avg_h);
    ESP_LOGI("HTS221", "avg_t is: %02x", hts221_config.avg_t);
    ESP_LOGI("HTS221", "odr is: %02x", hts221_config.odr);
    ESP_LOGI("HTS221", "bdu_status is: %02x", hts221_config.bdu_status);
    ESP_LOGI("HTS221", "heater_status is: %02x", hts221_config.heater_status);
    ESP_LOGI("HTS221", "irq_level is: %02x", hts221_config.irq_level);
    ESP_LOGI("HTS221", "irq_output_type is: %02x", hts221_config.irq_output_type);
    ESP_LOGI("HTS221", "irq_enable is: %02x", hts221_config.irq_enable);
    */

    hts221_config.avg_h = HTS221_AVGH_32;
    hts221_config.avg_t = HTS221_AVGT_16;
    hts221_config.odr = HTS221_ODR_1HZ;
    hts221_config.bdu_status = HTS221_DISABLE;
    hts221_config.heater_status = HTS221_DISABLE;
    hts221_set_config(hts221, &hts221_config);
    hts221_set_activate(hts221);


    // mpu6050
    mpu6050 = mpu6050_create(i2c_bus, MPU6050_I2C_ADDRESS);


    uint8_t mpu6050_deviceid;
    mpu6050_get_deviceid(mpu6050, &mpu6050_deviceid);
    ESP_LOGI("MPU6050","mpu6050 device ID is: 0x%02x", mpu6050_deviceid);
    mpu6050_wake_up(mpu6050);
    mpu6050_set_acce_fs(mpu6050, ACCE_FS_4G);
    mpu6050_set_gyro_fs(mpu6050, GYRO_FS_500DPS);
}


void sensor_deinit() {
    bme280_delete(&bme280);
    hts221_delete(&hts221);
    mpu6050_delete(&mpu6050);

    i2c_bus_delete(&i2c_bus);
}



void hts221_print_data(void)
{
    static const char* HTS221_TAG = "HTS221";
    int16_t temperature;
    int16_t humidity;

    hts221_get_humidity(hts221, &humidity);
    ESP_LOGI(HTS221_TAG, "humidity value is: %2.2f", (float)humidity / 10);
    hts221_get_temperature(hts221, &temperature);
    ESP_LOGI(HTS221_TAG, "temperature value is: %2.2f", (float)temperature / 10);
}

void bme280_print_data(void)
{
    float temperature = 0.0;
    // float humidity = 0.0;
    float pressure = 0.0;
    if (ESP_OK == bme280_read_temperature(bme280, &temperature)) {
        ESP_LOGI("BME280", "temperature:%f ", temperature);
    } else {
        ESP_LOGE("BME280", "Failed to read temperature");
    }
    /*
    // there is no humidity with bmp
    if (ESP_OK == bme280_read_humidity(bme280, &humidity))
    ESP_LOGI("BME280", "humidity:%f ", humidity);
    */
    if (ESP_OK == bme280_read_pressure(bme280, &pressure)) {
        ESP_LOGI("BME280", "pressure:%f", pressure);
    } else {
        ESP_LOGE("BME280", "Failed to read pressure");
    }
}

void mpu6050_print_data(void)
{
    mpu6050_acce_value_t acce;
    mpu6050_gyro_value_t gyro;

    mpu6050_get_acce(mpu6050, &acce);
    ESP_LOGI("MPU6050","acce_x:%.2f, acce_y:%.2f, acce_z:%.2f", acce.acce_x, acce.acce_y, acce.acce_z);
    mpu6050_get_gyro(mpu6050, &gyro);
    ESP_LOGI("MPU6050","gyro_x:%.2f, gyro_y:%.2f, gyro_z:%.2f", gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);

}






void sensorGetAllData(sensor_data_t* data) {
    int16_t tmp16;

    bme280_read_temperature(bme280, &(data->bme280_temp));
    bme280_read_pressure(bme280, &(data->bme280_press));

    hts221_get_humidity(hts221,  &tmp16);
    data->hts221_hum = ((float)tmp16 / 10);
    hts221_get_temperature(hts221,  &tmp16);
    data->hts221_temp = ((float)tmp16 / 10);


    mpu6050_acce_value_t acce;
    mpu6050_gyro_value_t gyro;

    mpu6050_get_acce(mpu6050, &acce);
    mpu6050_get_gyro(mpu6050, &gyro);

    data->mpu6050_acce_x = acce.acce_x;
    data->mpu6050_acce_y = acce.acce_y;
    data->mpu6050_acce_z = acce.acce_z;

    data->mpu6050_gyro_x = gyro.gyro_x;
    data->mpu6050_gyro_y = gyro.gyro_y;
    data->mpu6050_gyro_z = gyro.gyro_z;
}