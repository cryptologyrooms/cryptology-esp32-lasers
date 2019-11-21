#include <TaskAction.h>
#include <Arduino.h>

#include "io.h"
#include "application.h"

#define LASER_CONTROL_PIN -1
#define SECURITY_ENABLE_PIN -1
#define LOCK_DISPLAY_CONTROL_PIN -1

static const int DEBOUNCE_COUNT = 5;

struct io_input
{
    bool pressed;
    bool just_pressed;
    bool just_released;
    int debounce;
};
typedef struct io_input IO_INPUT;

static IO_INPUT s_security_enable_input;

static void debounce_input(IO_INPUT& io_input, bool state)
{
    io_input.just_pressed = false;
    io_input.just_released = false;

    if (state)
    {
        io_input.debounce = min(io_input.debounce+1, DEBOUNCE_COUNT);
    }
    else
    {
        io_input.debounce = max(io_input.debounce-1, 0);    
    }

    if (io_input.debounce == DEBOUNCE_COUNT)
    {
        io_input.just_pressed = !io_input.pressed;
        io_input.pressed = true;
    }
    else if (io_input.debounce == 0)
    {
        io_input.just_released = io_input.pressed;
        io_input.pressed = false;
    }
}

static void debounce_task_fn(TaskAction* this_task)
{
    (void)this_task;
    
    debounce_input(s_security_enable_input, digitalRead(SECURITY_ENABLE_PIN)==LOW);

    if (s_security_enable_input.just_pressed)
    {
        Serial.println("Security switch pressed");
        application_handle_security_switch_press();
    }
}
static TaskAction s_debounce_task(debounce_task_fn, 10, NULL);

void io_setup(void)
{
    pinMode(LASER_CONTROL_PIN, OUTPUT);
    pinMode(SECURITY_ENABLE_PIN, INPUT_PULLUP);
}

void io_loop()
{
    s_debounce_task.tick(); 
}

void io_lasers_enable(bool enable)
{
    digitalWrite(LASER_CONTROL_PIN, enable ? HIGH : LOW);
}

void io_set_locked_display(bool enable)
{
    digitalWrite(LOCK_DISPLAY_CONTROL_PIN, enable ? HIGH : LOW);    
}