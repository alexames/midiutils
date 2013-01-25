#include "midi.hpp"

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	const string filename = "..\\example9.mid";
	midi::MidiFile midi;
	
	ifstream file(filename, ios::binary);
	midi::readFile(midi, file);
	
	//ofstream output("..\\output.mid", ios::binary);
	//midi::writeFile(midi, output);

	midi::MidiStream stream(midi);
	stream.play();
	
	while (true) ;

	return 0;
}