#ifndef _SERVER_H_
#define _SERVER_H_

typedef enum _eEvent
{
	eEvent_LaserTrip,
	eEvent_Button
} eEvent;

void server_start();
void server_loop();

void server_push_event(eEvent event, uint8_t param);

#endif
