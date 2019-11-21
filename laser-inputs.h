#ifndef _LASER_INPUTS_H_
#define _LASER_INPUTS_H_

#define MAX_LASERS 9

void laser_input_setup();
void laser_input_loop();
bool laser_input_check_and_clear();
void laser_input_get_overrides(bool * overrides);

#endif
