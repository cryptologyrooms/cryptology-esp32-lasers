#include <TaskAction.h>

#include "laser-inputs.h"

static bool s_overrides[MAX_LASERS] = {true,true,true,true,true,true,true,true,true};

void laser_input_setup()
{
}

void laser_input_loop()
{
}

bool laser_input_check_and_clear()
{
  return false;
}

void laser_input_get_overrides(bool * overrides)
{
    if (overrides)
    {
        memcpy(overrides, s_overrides, sizeof(s_overrides));
    }
}
