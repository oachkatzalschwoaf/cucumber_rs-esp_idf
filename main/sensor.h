#ifndef __SENSOR_H__
#define __SENSOR_H__

typedef struct {
    float bme280_temp;
    float bme280_hum;    // only available with BME280 - missing in BMP280
    float bme280_press;

    float hts221_hum;
    float hts221_temp;

    float mpu6050_acce_x;
    float mpu6050_acce_y;
    float mpu6050_acce_z;

    float mpu6050_gyro_x;
    float mpu6050_gyro_y;
    float mpu6050_gyro_z;

} sensor_data_t;

void sensorGetAllData(sensor_data_t* data);

void bme280_print_data(void);
void hts221_print_data();
void mpu6050_print_data(void);

void sensor_init(void);
void sensor_deinit(void);

#endif