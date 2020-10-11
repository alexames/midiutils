#include <iostream>
#include <string>

#include "luamidiutils.hpp"
#include "midiutils.hpp"

using namespace std;
using namespace midi;

int main(int argc, char* argv[]) {
  const char* filename;
  if (argc >= 2) {
    filename = argv[1];
  } else {
    filename = "init.lua";
  }
  try {
    LuaEventProducer producer(filename);
    MidiStream stream(producer);
    stream.play();
    string line;
    while (getline(cin, line)) {
      producer.pushMessage(line);
    }
  } catch (exception& ex) {
    cout << ex.what() << endl;
  }

  return 0;
}