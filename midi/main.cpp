#include "midi.hpp"

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	string FILENAME = "..\\example9.mid";
	ifstream file(FILENAME, ios::binary);
	midi_File midi;
	midi_Error error = midi_readFile(midi, file);
	if (error == midi_NoError)
	{
		ofstream output("..\\output.mid", ios::binary);
		error = midi_writeFile(midi, output);
	}
	
	if (error != midi_NoError)
	{
		fprintf(stderr, "%s\n", midi_errorString(error));
	}
	return 0;
}