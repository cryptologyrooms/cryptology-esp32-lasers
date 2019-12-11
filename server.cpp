#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "laser-inputs.h"
#include "server.h"

static WebServer s_server(80);
static char const * const HTML_TEMPLATES[] = {\
"<html>\n"
"    <head>\n"
, /*********/

"          <meta http-equiv='refresh' content='1;url=/index''>\n"
, /*********/
"        <title>Daylight Robbery - Laser Control</title>\n"
"        <style>\n"
"           html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n"
"           button { border: none; color: white; padding: 16px 40px;"
" text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}\n"
"           button.red {background-color: #EE2222;}\n"
"           button.green {background-color: #22EE22;}\n"
"        </style>\n"
"    </head>\n"
"    <body>\n"
"        <h1>Daylight Robbery - Laser Control</h1>\n"
, /*********/
"    </body>\n"
"</html>"
};

static const char * ON_BUTTON_TEMPLATE = "<p><a href=\"/index?n=%d\"><button class=\"button green\">Laser %d</button></a></p>";
static const char * OFF_BUTTON_TEMPLATE = "<p><a href=\"/index?n=%d\"><button class=\"button red\">Laser %d</button></a></p>";

static char s_buffer[2048];

static const char * expected_json_keys[] = { 
    "sensor0", "sensor1", "sensor2", "sensor3",
    "sensor4", "sensor5", "sensor6", "sensor7",
    "sensor8"
};

static void add_laser_status_html(String& s, uint8_t laser_index, bool sensor_is_on)
{
    char buffer[96];
    if (sensor_is_on)
    {
        sprintf(buffer, ON_BUTTON_TEMPLATE, laser_index, laser_index+1);
    }
    else
    {
        sprintf(buffer, OFF_BUTTON_TEMPLATE, laser_index, laser_index+1);
    }

    s += buffer;
}

static void print_args(WebServer& server)
{
    String dbg = "/:";
    dbg += server.args();
    dbg += " args: ";
    for (uint8_t i = 0; i < server.args(); i++) {
        dbg += " " + server.argName(i) + ": " + server.arg(i) + ", ";
    }
    Serial.println(dbg);
}

static void print_data(WebServer& server)
{
    Serial.print("Got POST data:");
    Serial.println(server.arg("plain"));
}

void render_page(bool refresh)
{
    bool sensor_is_enabled[MAX_LASERS];
    String laser_status_html = "";

    laser_input_get_enabled_sensors(sensor_is_enabled);

    for (uint8_t i=0; i<MAX_LASERS; i++)
    {
        add_laser_status_html(laser_status_html, i, sensor_is_enabled[i]);
    }

    sprintf(s_buffer, "%s%s%s%s%s",
        HTML_TEMPLATES[0],
        refresh ? HTML_TEMPLATES[1] : "",
        HTML_TEMPLATES[2],
        laser_status_html.c_str(),
        HTML_TEMPLATES[3]
    );
}

void returnOK() {
  s_server.send(200, "text/plain", "");
}

void handle_index()
{
    print_args(s_server);

    if (s_server.args() > 0)
    {
        int laser_n = int(s_server.arg(0)[0]) - '0';
        if ((laser_n >= 0) && (laser_n < MAX_LASERS))
        {
            Serial.print("Toggling laser "); Serial.print(laser_n); Serial.println("");
            laser_input_toggle_sensor_enable(laser_n);
        }
        else
        {
            Serial.print("Unrecognized argument "); Serial.print(s_server.arg(0)); Serial.println("");
        }
    }

    render_page(s_server.args() > 0);
    s_server.sendHeader("Cache-Control", "no-cache");
    s_server.send(200, "text/html", s_buffer);
}

void handle_setsensors()
{
    print_data(s_server);
    StaticJsonDocument<96> doc;
    DeserializationError err = deserializeJson(doc, s_server.arg("plain"));
    
    if (err)
    {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.c_str());
        return;
    }

    for(int i=0; i<8; i++)
    {
        String value = doc[expected_json_keys[i]];
        if (value == "on")
        {
            Serial.print("Activating sensor "); Serial.println(i);
            laser_input_set_sensor_enable(i, true);
        }
        else if (value == "off")
        {
            Serial.print("Deactivating sensor "); Serial.println(i);
            laser_input_set_sensor_enable(i, false);
        }
    }
}

void server_start()
{
    s_server.on("/index", HTTP_GET, handle_index);
    s_server.on("/setsensors", HTTP_POST, handle_setsensors);
    s_server.begin();
}

void server_loop()
{
    s_server.handleClient();
}

void server_push_event(eEvent event, uint8_t param)
{
    static char event_encoded[] = "option=LX";
    switch(event)
    {
    case eEvent_LaserTrip:
        {
            event_encoded[8] = '1' + param;
        }
        break;
    case eEvent_Button:
        event_encoded[8] = 'S';
        break;
    }

    {
        Serial.print("Event: "); Serial.println(event_encoded);
        HTTPClient http;
        http.begin("http://cryptology.net/writeEventLaser.php");
        //http.addHeader("Content-Type", "application/json");
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        int resp = http.POST(event_encoded);
        Serial.print("Response: "); Serial.println(resp);
    }
}

void server_test_connection()
{
    HTTPClient http;
    http.begin("http://cryptology.net/test.php");
    int resp = http.GET();
    String payload = http.getString();
    Serial.print("Response: "); Serial.println(resp);
    Serial.print("Payload: "); Serial.println(payload);
}