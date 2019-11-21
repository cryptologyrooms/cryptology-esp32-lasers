#include <Arduino.h>

#include <fixed-length-accumulator.h>

static void handle_complete_buffer(FixedLengthAccumulator& buffer)
{
    (void)buffer;
}

void serial_loop()
{
    static char s_incoming[128];
    static FixedLengthAccumulator s_in_buffer(s_incoming, 128);

    char c;
    while(Serial.available())
    {
        c = Serial.read();
        if (c != '\n')
        {
            s_in_buffer.writeChar(c);
        }
        else
        {
            handle_complete_buffer(s_in_buffer);
            s_in_buffer.reset();
        }
    }
}
