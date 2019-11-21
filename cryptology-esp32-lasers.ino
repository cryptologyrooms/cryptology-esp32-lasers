#include <Esp.h>
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

static hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule() {
    esp_restart();
}

void setup()
{
    EEPROM.begin(32);
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    esp_log_level_set("*", ESP_LOG_VERBOSE);

    wifi_control_setup();
    app_wifi_setup();
    laser_input_setup();
    io_setup();

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
    application_loop();

    if (wifi_control_check_and_clear())
    {
        Serial.println("Clearing WiFi credentials");
        app_wifi_wipe_credentials();
        delay(3000);
        Serial.println("Restarting...");
        ESP.restart();
        delay(1000);
    }

    timerWrite(timer, 0);
}
