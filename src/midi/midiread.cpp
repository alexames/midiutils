#include "midi.hpp"

#include <fstream>
#include <iostream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

namespace midi
{

static unsigned int readUInt32be(istream& in)
{
	char data[4];
	in.read(data, 4);
	return (static_cast<unsigned char>(data[0]) << 24)
		| (static_cast<unsigned char>(data[1]) << 16)
		| (static_cast<unsigned char>(data[2]) << 8)
		| static_cast<unsigned char>(data[3]);
}

static unsigned short readUInt16be(istream& in)
{
	char data[2];
	in.read(static_cast<char*>(data), 2);
	return (static_cast<unsigned char>(data[0]) << 8)
		| static_cast<unsigned char>(data[1]);
}

////////////////////////////////////////////////////////////////////////////////

static bool isTrackEnd(const Event& event)
{
	return event.command == Event::Meta
		&& event.meta.command == Event::MetaEvent::EndOfTrack;
}

static void readEventTime(unsigned int& out, istream& in, unsigned int& byteCount)
{
	out = 0;
	unsigned char byte;
	do
	{
		byte = in.get();
		out <<= 7;
		out += byte & 0x7f;

		if (--byteCount == 0)
		{
			throw exception();
		}
	} while (byte & 0x80);
}

static void readNoteEndEvent(Event::NoteEndEvent& event, istream& in, unsigned int& byteCount)
{
	event.noteNumber = in.get();
	event.velocity = in.get();
	byteCount -= 2;
}
	
static void readNoteBeginEvent(Event::NoteBeginEvent& event, istream& in, unsigned int& byteCount)
{
	event.noteNumber = in.get();
	event.velocity = in.get();
	byteCount -= 2;
}

static void readVelocityChangeEvent(Event::VelocityChangeEvent& event, istream& in, unsigned int& byteCount)
{
	event.noteNumber = in.get();
	event.velocity = in.get();
	byteCount -= 2;
}

static void readControllerChangeEvent(Event::ControllerChangeEvent& event, istream& in, unsigned int& byteCount)
{
	event.controllerNumber = in.get();
	event.velocity = in.get();
	byteCount -= 2;
}

static void readProgramChangeEvent(Event::ProgramChangeEvent& event, istream& in, unsigned int& byteCount)
{
	event.newProgramNumber = in.get();
	byteCount -= 1;
}

static void readChannelPressureChangeEvent(Event::ChannelPressureChangeEvent& event, istream& in, unsigned int& byteCount)
{
	event.channelNumber = in.get();
	byteCount -= 1;
}

static void readPitchWheelChangeEvent(Event::PitchWheelChangeEvent& event, istream& in, unsigned int& byteCount)
{
	event.bottom = in.get();
	event.top = in.get();
	byteCount -= 2;
}

static void  readMetaEvent(Event::MetaEvent& event, istream& in, unsigned int& byteCount)
{
	event.command = static_cast<Event::MetaEvent::MetaCommand>(in.get());
	unsigned int length = event.length = in.get();
	byteCount -= 2 + event.length;
	event.data = (char*)malloc(event.length);
	for (unsigned int i = 0; i < length; i++)
		event.data[i] = in.get();
}

unsigned char readEventCommand(istream& in, unsigned char& previousCommandByte, unsigned int& byteCount)
{
	unsigned char commandByte = in.peek();
	if (commandByte & 0x80)
	{
		--byteCount;
		return in.get();
	}
	else
	{
		return previousCommandByte;
	}
}

static void readEvent(Event& event, istream& in, unsigned int& byteCount, unsigned char& previousCommandByte)
{
	readEventTime(event.timeDelta, in, byteCount);

	unsigned char commandByte = readEventCommand(in, previousCommandByte, byteCount);
	previousCommandByte = commandByte;
	event.command = static_cast<Event::Command>(commandByte & 0xF0);
	event.channel = commandByte & 0x0F;
	switch(event.command)
	{
	case Event::NoteEnd:
		readNoteEndEvent(event.noteEnd, in, byteCount);
		break;
	case Event::NoteBegin:
		readNoteBeginEvent(event.noteBegin, in, byteCount);
		break;
	case Event::VelocityChange:
		readVelocityChangeEvent(event.velocityChange, in, byteCount);
		break;
	case Event::ControllerChange:
		readControllerChangeEvent(event.controllerChange, in, byteCount);
		break;
	case Event::ProgramChange:
		readProgramChangeEvent(event.programChange, in, byteCount);
		break;
	case Event::ChannelPressureChange:
		readChannelPressureChangeEvent(event.channelPressureChange, in, byteCount);
		break;
	case Event::PitchWheelChange:
		readPitchWheelChangeEvent(event.pitchWheelChange, in, byteCount);
		break;
	case Event::Meta:
		readMetaEvent(event.meta, in, byteCount);
		if (isTrackEnd(event) != (byteCount == 0))
		{
			throw exception();
		}
		break;
	default:
		throw exception();
	}
}

static void readTrack(Track& track, istream& in)
{
	if (readUInt32be(in) != 'MTrk')
	{
		throw exception();
	}
	unsigned int byteCount = readUInt32be(in);
	unsigned char previousCommand = 0;
	while (byteCount)
	{
		track.events.push_back(Event());
		readEvent(track.events.back(), in, byteCount, previousCommand);
	}
}

void readFile(MidiFile& midi, istream& in)
{
	if (readUInt32be(in) != 'MThd' || readUInt32be(in) != 0x0006)
	{
		throw exception();
	}
	midi.format = static_cast<Format>(readUInt16be(in));
	midi.tracks.resize(readUInt16be(in));
	midi.ticks = readUInt16be(in);
	for (Track& track : midi.tracks)
	{
		readTrack(track, in);
	}
}

} // namespace midi