#ifndef _LASER_INPUTS_H_
#define _LASER_INPUTS_H_

#define MAX_LASERS 9

void laser_input_setup();
void laser_input_loop();
void laser_input_get_enabled_sensors(bool * enabled_sensors);
void laser_input_set_sensor_enable(uint8_t i, bool en);
void laser_input_toggle_sensor_enable(uint8_t i);
void laser_input_global_enable(bool enable);

#endif
