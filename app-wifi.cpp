#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>

#include "server.h"

static WiFiManager s_wifiManager;

/*
!!!
This application requires the development version of WifiManager!!!
"""
*/

static IPAddress local_ip(192, 168, 1, 21);
static IPAddress gateway(192, 168, 1, 1);
static IPAddress subnet(255, 255, 255, 0);
static IPAddress dns(192, 168, 1, 10);


static void onAccessPointCallback(WiFiManager * pManager)
{
  (void)pManager;
  Serial.print("Started access point ");
  Serial.print(s_wifiManager.getConfigPortalSSID());
  Serial.print(" at ");
  Serial.println(WiFi.softAPIP());
}

void app_wifi_setup()
{
  s_wifiManager.setAPCallback(onAccessPointCallback);
  s_wifiManager.setSTAStaticIPConfig(local_ip, gateway, subnet, dns);

  s_wifiManager.autoConnect("Cryptology-Laser-AP");

  Serial.print("Connected to ");
  Serial.print(WiFi.SSID());
  Serial.print(" as ");
  Serial.println(WiFi.localIP());

  server_start();

  server_test_connection();
}

void app_wifi_wipe_credentials()
{
  WiFi.disconnect(true);
  WiFi.begin("0","0");
}

