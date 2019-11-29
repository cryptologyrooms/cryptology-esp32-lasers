#include <TaskAction.h>
#include <esp_system.h>
#include <EEPROM.h>

#include "app-wifi.h"
#include "laser-inputs.h"
#include "laser-control.h"
#include "wifi-control.h"
#include "serial-handler.h"
#include "server.h"
#include "io.h"
#include "application.h"

bool s_tripped = false;

void application_loop()
{
    laser_control_run();
    io_set_locked_display(s_tripped);
    io_set_maglock(s_tripped);
    
    laser_input_global_enable(
        laser_control_get_laser_onoff_state()
    );
}

void application_set_laser_tripped()
{
    laser_control_set_lasers(false);
    s_tripped = true;
}

void application_handle_security_switch_press()
{
    s_tripped = false;
}
