#include <TaskAction.h>

#include "laser-control.h"
#include "io.h"

typedef enum _eLaserState
{
    eLaserState_Off,
    eLaserState_Powering,
    eLaserState_On
} eLaserState;

static const unsigned long LASER_OFF_TIME = 800UL;
static const unsigned long LASER_POWERUP_TIME = 500UL;
static unsigned long s_laser_timeout = 0;

static eLaserState s_laser_state = eLaserState_Off;

static void laser_control_task_fn(TaskAction* this_task)
{
	bool end_of_timeout = false;
	if (s_laser_timeout)
	{
		s_laser_timeout -= 10;
		end_of_timeout = (s_laser_timeout == 0); 
	}

	switch(s_laser_state)
	{
	case eLaserState_Off:
		if (end_of_timeout)
		{
			Serial.println("Laser off timeout expired");
			Serial.println("Starting powerup timeout");
			io_lasers_enable(true);
			s_laser_state = eLaserState_Powering;
			s_laser_timeout = LASER_POWERUP_TIME;
		}
		break;
	case eLaserState_Powering:
		if (end_of_timeout)
		{
			Serial.println("Laser powerup timeout expired");
			s_laser_state = eLaserState_On;
			s_laser_timeout = 0;
		}
		break;
	case eLaserState_On:
		break;
	}
}
static TaskAction s_laser_control_task(laser_control_task_fn, 10, INFINITE_TICKS);

void laser_control_run()
{
	s_laser_control_task.tick();    
}

void laser_control_set_lasers(bool on)
{
	io_lasers_enable(on);

	if (on)
	{
		switch(s_laser_state)
		{
		case eLaserState_Off:
			Serial.println("Starting powerup timeout");
			s_laser_state = eLaserState_Powering;
			s_laser_timeout = LASER_POWERUP_TIME;
			break;
		case eLaserState_Powering:
			break;
		case eLaserState_On:
			break;
		}
	}
	else
	{
		switch(s_laser_state)
		{
		case eLaserState_Off:
			break;
		case eLaserState_Powering:
			Serial.println("Starting off timeout");
			s_laser_state = eLaserState_Off;
			s_laser_timeout = LASER_OFF_TIME;
			break;
		case eLaserState_On:
			Serial.println("Starting off timeout");
			s_laser_state = eLaserState_Off;
			s_laser_timeout = LASER_OFF_TIME;
			break;
		}
	}
}

bool laser_control_get_laser_onoff_state()
{
	return s_laser_state == eLaserState_On;
}
