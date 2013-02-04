#include "midiutils.hpp"
#include "luamidiutils.hpp"

#include <string>
#include <iostream>

using namespace std;
using namespace midi;

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        try
        {
            const char* filename = argv[1];
            LuaEventProducer producer(filename);
            MidiStream stream(producer);
            stream.play();
            string line;
            while(getline(cin, line))
            {
                producer.pushMessage(line);
            }
        }
        catch(exception& ex)
        {
            cout << ex.what() << endl;
        }
    }
    while (true);
    return 0;
}