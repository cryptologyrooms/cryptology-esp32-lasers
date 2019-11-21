#ifndef _LASER_INPUTS_H_
#define _LASER_INPUTS_H_

#define MAX_LASERS 9

void laser_input_setup();
void laser_input_loop();
void laser_input_get_overrides(bool * overrides);
void laser_input_toggle_override(uint8_t i);

#endif
