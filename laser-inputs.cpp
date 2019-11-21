#include <TaskAction.h>
#include <EEPROM.h>

#include "laser-inputs.h"
#include "application.h"

static const int DEBOUNCE_COUNT = 5;

struct laser_input
{
    bool clear;
    bool just_cleared;
    bool just_tripped;
    int debounce;
};
typedef struct laser_input LASER_INPUT;

static const uint8_t LASER_INPUT_PINS[MAX_LASERS] = {};
static LASER_INPUT s_laser_inputs[MAX_LASERS] = {0};

/* Overrides are stored in flash, accessed by using EEPROM abstraction */
static bool s_overrides[MAX_LASERS];

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
        laser_input.just_cleared = !laser_input.clear;
        laser_input.clear = true;
    }
    else if (laser_input.debounce == 0)
    {
        laser_input.just_tripped = laser_input.clear;
        laser_input.clear = false;
    }
}

static void debounce_task_fn(TaskAction* this_task)
{
    (void)this_task;
    bool at_least_one_laser_tripped = false;

    for (uint8_t i=0; i<MAX_LASERS; i++)
    {
        if (!s_overrides[i])
        {
            debounce_input(s_laser_inputs[i], digitalRead(LASER_INPUT_PINS[i])==LOW);
        }
        else
        {
            s_laser_inputs[i].clear = true;
            s_laser_inputs[i].just_cleared = false;
            s_laser_inputs[i].just_tripped = false;
        }

        at_least_one_laser_tripped |= s_laser_inputs[i].clear = false;

        if (s_laser_inputs[i].just_cleared)
        {
            Serial.print("Input "); Serial.print(i); Serial.println(" cleared");
        }
        else if (s_laser_inputs[i].just_tripped)
        {
            Serial.print("Input "); Serial.print(i); Serial.println(" tripped");
        }

    }

    if (at_least_one_laser_tripped)
    {
        application_set_laser_input_state(true);
    }
}
static TaskAction s_debounce_task(debounce_task_fn, 10, NULL);

void laser_input_setup()
{
    for(uint8_t i=0; i<MAX_LASERS; i++)
    {
        pinMode(LASER_INPUT_PINS[i], INPUT);
        s_overrides[i] = EEPROM.read(i);
    }
}

void laser_input_loop()
{
    if (!application_in_laser_off_time())
    {
        s_debounce_task.tick();
    }
}

void laser_input_get_overrides(bool * overrides)
{
    if (overrides)
    {
        memcpy(overrides, s_overrides, sizeof(s_overrides));
    }
}

void laser_input_toggle_override(uint8_t i)
{
    if (i < MAX_LASERS)
    {
        s_overrides[i] = !s_overrides[i];
        EEPROM.write(i, s_overrides[i]);
        EEPROM.commit();
    }
}
