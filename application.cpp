#include <TaskAction.h>
#include <esp_system.h>
#include <EEPROM.h>

#include "app-wifi.h"
#include "laser-inputs.h"
#include "wifi-control.h"
#include "serial-handler.h"
#include "server.h"
#include "io.h"
#include "application.h"

static const unsigned long LASER_OFF_TIME = 500UL;
static unsigned long s_laser_trip_time = 0UL;

bool s_tripped = false;

void application_loop()
{
	if (!application_in_laser_off_time())
    {
        io_lasers_enable(true);
    }

    io_set_locked_display(s_tripped);
}

void application_set_laser_input_state(bool tripped)
{
	s_tripped = tripped;
    io_lasers_enable(!tripped);
    if (tripped)
    {
        s_laser_trip_time = millis();
    }
}

bool application_in_laser_off_time()
{
    return (millis() - s_laser_trip_time) > LASER_OFF_TIME;
}

void application_handle_security_switch_press()
{
    s_tripped = false;

}
