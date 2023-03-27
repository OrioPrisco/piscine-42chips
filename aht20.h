#ifndef AHT20_H
#define AHT20_H

#define TEMP_SENSOR_SLA 0x38 //rtfm
#define TEMP_SENSOR_BUSY_MASK 0x80 // bit 7
#define TEMP_SENSOR_CALIBRATE 0xbe
#define TEMP_SENSOR_CALIBRATION_MASK 0x18
#define TEMP_SENSOR_MEASURE 0xAC3300

unsigned char temp_sensor_get_status();
void temp_sensor_init();
void temp_sensor_trigger_measurment();
void temp_sensor_get_measurment(unsigned char data[7]);
//does not call trigger measurement, only collects it
//0 means not ready, 1 means ready and read
_Bool temp_sensor_collect_measurment_if_ready(unsigned char data[7]);

float temp_sensor_data_to_celcius(unsigned char data[7]);
float temp_sensor_data_to_farenheit(unsigned char data[7]);
float temp_sensor_data_to_humidity(unsigned char data[7]);
#endif
