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

static const unsigned long LASER_OFF_TIME = 5000UL;
static unsigned long s_laser_trip_time = 0;

bool s_tripped = false;
bool s_lasers_off = false;

void application_loop()
{
    if (s_lasers_off && !application_in_laser_off_time())
    {
        Serial.println("Re-enable lasers");
        s_lasers_off = false;
        io_lasers_enable(true);
        laser_input_global_enable(true);
    }
    
    io_set_locked_display(s_tripped);
    io_set_maglock(s_tripped);
}

void application_set_laser_tripped()
{
    s_tripped = true;
    io_lasers_enable(false);
    laser_input_global_enable(false);
    s_lasers_off = true;
    s_laser_trip_time = millis();
}

bool application_in_laser_off_time()
{
    return (millis() - s_laser_trip_time) < LASER_OFF_TIME;
}

void application_handle_security_switch_press()
{
    s_tripped = false;
}
