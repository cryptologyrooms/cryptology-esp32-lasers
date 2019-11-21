#include <Adafruit_NeoPixel.h>
#include <TaskAction.h>
#include <WiFi.h>

#include "wifi-control.h"

static const uint8_t RESET_PIN = 22;

static uint8_t s_debouncer = 0;
static bool s_pressed = false;

static Adafruit_NeoPixel s_status_pixel(1, 23, NEO_GRB + NEO_KHZ800);
static uint32_t s_pixel_colour = s_status_pixel.Color(0,0,0);

static void debounce_task_fn(TaskAction * pThisTask)
{
  (void)pThisTask;
  bool pressed = digitalRead(RESET_PIN) == LOW;
  if (pressed && (s_debouncer < 15))
  {
    s_debouncer++;
    s_pressed = s_debouncer == 15;
  }
  else if (!pressed && (s_debouncer > 0))
  {
    s_debouncer--;
  }
}
static TaskAction s_debounce_task(debounce_task_fn, 200, INFINITE_TICKS);

static void led_flash_task_fn(TaskAction * pThisTask)
{
  static bool on;
  s_status_pixel.setPixelColor(0, on ? s_pixel_colour : s_status_pixel.Color(0, 0, 0));
  s_status_pixel.show();
  s_status_pixel.show();
  on = !on;
}
static TaskAction s_led_flash_task(led_flash_task_fn, 500, INFINITE_TICKS);

void wifi_control_setup()
{
  pinMode(RESET_PIN, INPUT_PULLUP);
  s_status_pixel.begin();
  s_status_pixel.setPixelColor(0, s_status_pixel.Color(0, 0, 50));
  s_status_pixel.show();
  s_status_pixel.show();
}

void wifi_control_loop()
{
  s_debounce_task.tick();

  switch(WiFi.status())
  {
  case WL_IDLE_STATUS:
    s_led_flash_task.Enable(true);
    s_led_flash_task.SetInterval(200);
    s_pixel_colour = s_status_pixel.Color(0, 50, 0);
    break;
  case WL_NO_SSID_AVAIL:
    s_led_flash_task.Enable(true);
    s_led_flash_task.SetInterval(500);
    s_pixel_colour = s_status_pixel.Color(0, 0, 50);
    break;
  case WL_CONNECT_FAILED:
  case WL_CONNECTION_LOST:
    s_led_flash_task.Enable(true);
    s_led_flash_task.SetInterval(200);
    s_pixel_colour = s_status_pixel.Color(50, 0, 0);
    break;
    case WL_DISCONNECTED:
    s_led_flash_task.Enable(true);
    s_led_flash_task.SetInterval(500);
    s_pixel_colour = s_status_pixel.Color(50, 0, 0);
    break;
  case WL_CONNECTED:
    s_led_flash_task.Enable(false);
    s_status_pixel.setPixelColor(0, s_status_pixel.Color(0, 50, 0));
    s_status_pixel.show();
    break;
  default:
    break;
  }
  s_led_flash_task.tick();
}

bool wifi_control_check_and_clear()
{
  bool pressed = s_pressed;
  s_pressed = false;
  return pressed;
}

