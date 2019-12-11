#include <TaskAction.h>
#include <EEPROM.h>

#include "laser-inputs.h"
#include "application.h"

static const int DEBOUNCE_COUNT = 5;

struct laser_input
{
    bool tripped;
    bool just_cleared;
    bool just_tripped;
    int debounce;
};
typedef struct laser_input LASER_INPUT;

static const uint8_t LASER_INPUT_PINS[MAX_LASERS] = {34, 35, 32, 33, 25, 26, 27, 14, 12};
static LASER_INPUT s_laser_inputs[MAX_LASERS] = {0};

/* Laser enabled sensors are stored in flash, accessed by using EEPROM abstraction */
static bool s_sensor_enabled[MAX_LASERS];

static void debounce_input(LASER_INPUT& laser_input, bool state)
{
    laser_input.just_cleared = false;
    laser_input.just_tripped = false;

    if (state)
    {
        laser_input.debounce = min(laser_input.debounce+1, DEBOUNCE_COUNT);
    }
    else
    {
        laser_input.debounce = max(laser_input.debounce-1, 0);    
    }

    if (laser_input.debounce == DEBOUNCE_COUNT)
    {
        laser_input.just_tripped = !laser_input.tripped;
        laser_input.tripped = true;
    }
    else if (laser_input.debounce == 0)
    {
        laser_input.just_cleared = laser_input.tripped;
        laser_input.tripped = false;
    }
}

static void debounce_task_fn(TaskAction* this_task)
{
    (void)this_task;
    bool at_least_one_laser_tripped = false;

    for (uint8_t i=0; i<MAX_LASERS; i++)
    {
        if (s_sensor_enabled[i])
        {
            debounce_input(s_laser_inputs[i], digitalRead(LASER_INPUT_PINS[i])==LOW);
        }
        else
        {
            s_laser_inputs[i].tripped = false;
            s_laser_inputs[i].just_cleared = false;
            s_laser_inputs[i].just_tripped = false;
        }

        at_least_one_laser_tripped |= (s_laser_inputs[i].tripped == true);

        if (s_laser_inputs[i].just_cleared)
        {
            Serial.print("Laser "); Serial.print(i+1); Serial.println(" cleared");
        }
        if (s_laser_inputs[i].just_tripped)
        {
            Serial.print("Laser "); Serial.print(i+1); Serial.println(" tripped");
        }
    }

    if (at_least_one_laser_tripped)
    {
        Serial.println("Laser tripped!");
        application_set_laser_tripped();
    }
}
static TaskAction s_debounce_task(debounce_task_fn, 10, INFINITE_TICKS);

static void debug_task_fn(TaskAction* this_task)
{
    (void)this_task;
    Serial.print("Lasers: ");
    for (uint8_t i=0; i<MAX_LASERS; i++)
    {
        Serial.print(s_laser_inputs[i].tripped ? "1," : "0,");
    }
    Serial.println("");
}
static TaskAction s_debug_task(debug_task_fn, 750, INFINITE_TICKS);

void laser_input_setup()
{
    Serial.print("Enabled sensors: ");
    for(uint8_t i=0; i<MAX_LASERS; i++)
    {
        pinMode(LASER_INPUT_PINS[i], INPUT_PULLUP);
        s_sensor_enabled[i] = EEPROM.read(i);
        Serial.print(s_sensor_enabled[i] ? "1," : "0,");
        s_laser_inputs[i].tripped = false;
        s_laser_inputs[i].just_tripped = false;
        s_laser_inputs[i].just_cleared = false;
           
    }
    Serial.println("");
    s_debounce_task.Enable(true);
}

void laser_input_reset_debounce()
{
    for(uint8_t i=0; i<MAX_LASERS; i++)
    {
        s_laser_inputs[i].debounce = 0;
        s_laser_inputs[i].tripped = false;
        s_laser_inputs[i].just_tripped = false;
        s_laser_inputs[i].just_cleared = false;
    }
}

void laser_input_loop()
{
    s_debounce_task.tick();        
    s_debug_task.tick();
}

void laser_input_get_enabled_sensors(bool * enabled_sensors)
{
    if (enabled_sensors)
    {
        memcpy(enabled_sensors, s_sensor_enabled, sizeof(s_sensor_enabled));
    }
}

void laser_input_set_sensor_enable(uint8_t i, bool en)
{
    if (i < MAX_LASERS)
    {
        s_sensor_enabled[i] = en;
        EEPROM.write(i, s_sensor_enabled[i]);
        EEPROM.commit();
    }
}

void laser_input_toggle_sensor_enable(uint8_t i)
{
    if (i < MAX_LASERS)
    {
        laser_input_set_sensor_enable(i, !s_sensor_enabled[i]);
    }
}

static bool task_enabled = true;
void laser_input_global_enable(bool enable)
{ 
    if (enable != task_enabled)
    {
        if (enable)
        {
            Serial.println("Enabling sensors");
        }
        else
        {
            Serial.println("Disabling sensors");            
        }
        
        task_enabled = enable;
        s_debounce_task.Enable(enable);
        laser_input_reset_debounce();
    }
}
