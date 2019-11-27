#ifndef _APPLICATION_H_
#define _APPLICATION_H_

void application_loop();

void application_set_laser_tripped();
bool application_in_laser_off_time();

void application_handle_security_switch_press();

#endif
