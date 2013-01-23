#include "midi.hpp"

#include <fstream>
#include <iostream>

using namespace std;

#define midi_Check(statement) do { midi_Error error_ = (statement); if (error_ != midi_NoError) return error_; } while(false)

////////////////////////////////////////////////////////////////////////////////

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

static bool midi_isTrackEnd(const midi_Event& event)
{
	return event.command == midi_Event::Meta
		&& event.meta.command == midi_Event::MetaEvent::EndOfTrack;
}

static midi_Error midi_readEventTime(unsigned int& out, istream& in, unsigned int& byteCount)
{
	out = 0;
	unsigned char byte;
	unsigned int count = 0;
	do
	{
		byte = in.get();
		out <<= 7;
		out += byte & 0x7f;

		if (--byteCount == 0 || ++count >= 4)
		{
			return midi_InvalidEventTimeDelta;
		}
	} while (byte & 0x80);

	return midi_NoError;
}

static midi_Error midi_readNoteEndEvent(midi_Event::NoteEndEvent& event, istream& in, unsigned int& byteCount)
{
	event.noteNumber = in.get();
	event.velocity = in.get();
	byteCount -= 2;
	return midi_NoError;
}
	
static midi_Error midi_readNoteBeginEvent(midi_Event::NoteBeginEvent& event, istream& in, unsigned int& byteCount)
{
	event.noteNumber = in.get();
	event.velocity = in.get();
	byteCount -= 2;
	return midi_NoError;
}

static midi_Error midi_readVelocityChangeEvent(midi_Event::VelocityChangeEvent& event, istream& in, unsigned int& byteCount)
{
	event.noteNumber = in.get();
	event.velocity = in.get();
	byteCount -= 2;
	return midi_NoError;
}

static midi_Error midi_readControllerChangeEvent(midi_Event::ControllerChangeEvent& event, istream& in, unsigned int& byteCount)
{
	event.controllerNumber = in.get();
	event.velocity = in.get();
	byteCount -= 2;
	return midi_NoError;
}

static midi_Error midi_readProgramChangeEvent(midi_Event::ProgramChangeEvent& event, istream& in, unsigned int& byteCount)
{
	event.newProgramNumber = in.get();
	byteCount -= 1;
	return midi_NoError;
}

static midi_Error midi_readChannelPressureChangeEvent(midi_Event::ChannelPressureChangeEvent& event, istream& in, unsigned int& byteCount)
{
	event.channelNumber = in.get();
	byteCount -= 1;
	return midi_NoError;
}

static midi_Error midi_readPitchWheelChangeEvent(midi_Event::PitchWheelChangeEvent& event, istream& in, unsigned int& byteCount)
{
	event.bottom = in.get();
	event.top = in.get();
	byteCount -= 2;
	return midi_NoError;
}

static midi_Error midi_readMetaEvent(midi_Event::MetaEvent& event, istream& in, unsigned int& byteCount)
{
	event.command = static_cast<midi_Event::MetaEvent::MetaCommand>(in.get());
	unsigned int length = event.length = in.get();
	byteCount -= 2 + event.length;
	event.data = (char*)malloc(event.length);
	for (unsigned int i = 0; i < length; i++)
		event.data[i] = in.get();
	return midi_NoError;
}

unsigned char midi_readEventCommand(istream& in, unsigned char& previousCommandByte, unsigned int& byteCount)
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

static midi_Error midi_readEvent(midi_Event& event, istream& in, unsigned int& byteCount, unsigned char& previousCommandByte)
{
	midi_Check(midi_readEventTime(event.timeDelta, in, byteCount));

	unsigned char commandByte = midi_readEventCommand(in, previousCommandByte, byteCount);
	previousCommandByte = commandByte;
	event.command = static_cast<midi_Event::Command>(commandByte & 0xF0);
	event.channel = commandByte & 0x0F;
	switch(event.command)
	{
	case midi_Event::NoteEnd:
		midi_Check(midi_readNoteEndEvent(event.noteEnd, in, byteCount));
		break;
	case midi_Event::NoteBegin:
		midi_Check(midi_readNoteBeginEvent(event.noteBegin, in, byteCount));
		break;
	case midi_Event::VelocityChange:
		midi_Check(midi_readVelocityChangeEvent(event.velocityChange, in, byteCount));
		break;
	case midi_Event::ControllerChange:
		midi_Check(midi_readControllerChangeEvent(event.controllerChange, in, byteCount));
		break;
	case midi_Event::ProgramChange:
		midi_Check(midi_readProgramChangeEvent(event.programChange, in, byteCount));
		break;
	case midi_Event::ChannelPressureChange:
		midi_Check(midi_readChannelPressureChangeEvent(event.channelPressureChange, in, byteCount));
		break;
	case midi_Event::PitchWheelChange:
		midi_Check(midi_readPitchWheelChangeEvent(event.pitchWheelChange, in, byteCount));
		break;
	case midi_Event::Meta:
		midi_Check(midi_readMetaEvent(event.meta, in, byteCount));
		if (midi_isTrackEnd(event) != (byteCount == 0))
		{
			return midi_InvalidTrackLength;
		}
		break;
	default:
		return midi_InvalidEvent;
	}
	return midi_NoError;
}

static midi_Error midi_readTrack(midi_Track& track, istream& in)
{
	if (readUInt32be(in) != 'MTrk')
		return midi_InvalidTrackHeader;

	unsigned int byteCount = readUInt32be(in);
	unsigned char previousCommand = 0;
	while (byteCount)
	{
		track.events.push_back(midi_Event());
		midi_Check(midi_readEvent(track.events.back(), in, byteCount, previousCommand));
	}
	return midi_NoError;
}

midi_Error midi_readFile(midi_File& midi, istream& in)
{
	try 
	{
		in.exceptions(istream::failbit | istream::badbit);

		if (readUInt32be(in) != 'MThd')
			return midi_InvalidHeader;

		if (readUInt32be(in) != 0x0006)
			return midi_InvalidHeader;
	
		midi.header.format = readUInt16be(in);
		midi.tracks.resize(readUInt16be(in));
		midi.header.ticks = readUInt16be(in);
		for (midi_Track& track : midi.tracks)
		{
			midi_Check(midi_readTrack(track, in));
		}
	}
	catch (std::exception&)
	{
		return midi_ReadError;
	}
	return midi_NoError;
}