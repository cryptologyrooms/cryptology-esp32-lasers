#include <Esp.h>
#include <TaskAction.h>
#include <esp_system.h>

#include "app-wifi.h"
#include "laser-inputs.h"
#include "wifi-control.h"
#include "serial-handler.h"
#include "server.h"


static hw_timer_t *timer = NULL;

static void led_task_fn(TaskAction* pTask)
{
    (void)pTask;
    static bool s_led = false;
    digitalWrite(5, s_led = !s_led);
}
static TaskAction s_led_task(led_task_fn, 500, INFINITE_TICKS);

void IRAM_ATTR resetModule() {
    esp_restart();
}

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    esp_log_level_set("*", ESP_LOG_VERBOSE);

    pinMode(5, OUTPUT);

    wifi_control_setup();
    app_wifi_setup();
    laser_input_setup();

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &resetModule, true);
    timerAlarmWrite(timer, 20000 * 1000, false);
    timerAlarmEnable(timer);
}

void loop()
{
    laser_input_loop();
    wifi_control_loop();
    serial_loop();
    server_loop();

    if (wifi_control_check_and_clear())
    {
        Serial.println("Clearing WiFi credentials");
        app_wifi_wipe_credentials();
        delay(3000);
        Serial.println("Restarting...");
        ESP.restart();
        delay(1000);
    }

    s_led_task.tick();

    timerWrite(timer, 0);
}
