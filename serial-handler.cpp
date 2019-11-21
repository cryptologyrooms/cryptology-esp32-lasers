#include <Arduino.h>

static void handle_complete_buffer(String& buffer)
{
    (void)buffer;
}

void serial_loop()
{
    static String s_in_buffer;

    char c;
    while(Serial.available())
    {
        c = Serial.read();
        if (c != '\n')
        {
            s_in_buffer += c;
        }
        else
        {
            handle_complete_buffer(s_in_buffer);
            s_in_buffer = "";
        }
    }
}
