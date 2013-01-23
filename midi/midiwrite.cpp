#include "midi.hpp"

#include <fstream>
#include <iostream>

using namespace std;

#define midi_Check(statement) do { midi_Error error_ = (statement); if (error_ != midi_NoError) return error_; } while(false)

////////////////////////////////////////////////////////////////////////////////

static void writeUInt32be(ostream& out, unsigned int val)
{
	char data[] = {
		static_cast<char>(val >> 24),
		static_cast<char>(val >> 16),
		static_cast<char>(val >> 8),
		static_cast<char>(val)
	};
	out.write(data, sizeof(data));
}

static void writeUInt16be(ostream& out, unsigned short val)
{
	char data[] = {
		static_cast<char>(val >> 8),
		static_cast<char>(val)
	};
	out.write(data, sizeof(data));
}

////////////////////////////////////////////////////////////////////////////////

static void midi_writeNoteEndEvent(midi_Event::NoteEndEvent event, ostream& out)
{
	out.put(event.noteNumber);
	out.put(event.velocity);
}

static void midi_writeNoteBeginEvent(midi_Event::NoteBeginEvent event, ostream& out)
{
	out.put(event.noteNumber);
	out.put(event.velocity);
}

static void midi_writeVelocityChangeEvent(midi_Event::VelocityChangeEvent event, ostream& out)
{
	out.put(event.noteNumber);
	out.put(event.velocity);
}

static void midi_writeControllerChangeEvent(midi_Event::ControllerChangeEvent event, ostream& out)
{
	out.put(event.controllerNumber);
	out.put(event.velocity);
}

static void midi_writeProgramChangeEvent(midi_Event::ProgramChangeEvent event, ostream& out)
{
	out.put(event.newProgramNumber);
}

static void midi_writeChannelPressureChangeEvent(midi_Event::ChannelPressureChangeEvent event, ostream& out)
{
	out.put(event.channelNumber);
}

static void midi_writePitchWheelChangeEvent(midi_Event::PitchWheelChangeEvent event, ostream& out)
{
	out.put(event.bottom);
	out.put(event.top);
}

static void midi_writeMetaEvent(midi_Event::MetaEvent event, ostream& out)
{
	out.put(event.command);
	out.put(event.length);
	for (int i = 0; i < event.length; i++)
		out.put(event.data[i]);
}

static void midi_writeEventTime(unsigned int timeDelta, ostream& out)
{
	if (timeDelta > (0x7F * 0x7F * 0x7F))
	{
		out.put((timeDelta >> 21) | 0x80);
	}
	if (timeDelta > (0x7F * 0x7F))
	{
		out.put((timeDelta >> 14) | 0x80);
	}
	if (timeDelta > (0x7F))
	{
		out.put((timeDelta >> 7) | 0x80);
	}
	out.put(timeDelta & 0x7F);
}

static midi_Error midi_writeEvent(midi_Event& event, ostream& out, unsigned char& previousCommandByte)
{
	midi_writeEventTime(event.timeDelta, out);
	unsigned char commandByte = event.command | event.channel;
	if (commandByte != previousCommandByte || event.command == midi_Event::Meta)
	{
		out.put(commandByte);
		previousCommandByte = commandByte;
	}
	switch(event.command)
	{
	case midi_Event::NoteEnd:
		midi_writeNoteEndEvent(event.noteEnd, out);
		break;
	case midi_Event::NoteBegin:
		midi_writeNoteBeginEvent(event.noteBegin, out);
		break;
	case midi_Event::VelocityChange:
		midi_writeVelocityChangeEvent(event.velocityChange, out);
		break;
	case midi_Event::ControllerChange:
		midi_writeControllerChangeEvent(event.controllerChange, out);
		break;
	case midi_Event::ProgramChange:
		midi_writeProgramChangeEvent(event.programChange, out);
		break;
	case midi_Event::ChannelPressureChange:
		midi_writeChannelPressureChangeEvent(event.channelPressureChange, out);
		break;
	case midi_Event::PitchWheelChange:
		midi_writePitchWheelChangeEvent(event.pitchWheelChange, out);
		break;
	case midi_Event::Meta:
		midi_writeMetaEvent(event.meta, out);
		break;
	default:
		return midi_InvalidEvent;
	}
	return midi_NoError;
}

static unsigned int midi_getTrackLength(midi_Track& track)
{
	unsigned int length = 0;

	unsigned char previousCommandByte = 0;
	for (midi_Event& event : track.events)
	{
		// Time delta
		if (event.timeDelta > (0x7f * 0x7f * 0x7f))
			length += 4;
		if (event.timeDelta > (0x7f * 0x7f))
			length += 3;
		if (event.timeDelta > (0x7f))
			length += 2;
		else
			length += 1;

		// Command 
		unsigned char commandByte = event.command | event.channel;
		if (commandByte != previousCommandByte || event.command == midi_Event::Meta)
		{
			length += 1;
			previousCommandByte = commandByte;
		}

		// Data bytes
		switch(event.command)
		{
		// One data byte
		case midi_Event::ProgramChange:
		case midi_Event::ChannelPressureChange:
			length += 1;
			break;
		// Two data bytes
		case midi_Event::NoteEnd:
		case midi_Event::NoteBegin:
		case midi_Event::VelocityChange:
		case midi_Event::ControllerChange:
		case midi_Event::PitchWheelChange:
			length += 2;
			break;
		// Variable data bytes
		case midi_Event::Meta:
			length += 2 + event.meta.length;
			break;
		}
	}

	return length;
}

static midi_Error midi_writeTrack(midi_Track& track, std::ostream& out)
{
	writeUInt32be(out, 'MTrk');
	writeUInt32be(out, midi_getTrackLength(track));
	unsigned char previousCommandByte = 0;
	for (midi_Event& event : track.events)
	{
		midi_Check(midi_writeEvent(event, out, previousCommandByte));
	}
	return midi_NoError;
}

midi_Error midi_writeFile(midi_File& midi, std::ostream& out)
{
	try 
	{
		out.exceptions(ostream::failbit | ostream::badbit);

		writeUInt32be(out, 'MThd');
		writeUInt32be(out,  0x0006);
		writeUInt16be(out, midi.header.format);
		writeUInt16be(out, midi.tracks.size());
		writeUInt16be(out, midi.header.ticks);
		for (midi_Track& track : midi.tracks)
		{
			midi_Check(midi_writeTrack(track, out));
		}
	}
	catch (std::exception&)
	{
		return midi_WriteError;
	}
	return midi_NoError;
}
