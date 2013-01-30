#include "midiutils.hpp"

#include <fstream>
#include <iostream>

using namespace std;

namespace midi
{

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

static void writeNoteEndEvent(Event::NoteEndEvent event, ostream& out)
{
    out.put(event.noteNumber);
    out.put(event.velocity);
}

static void writeNoteBeginEvent(Event::NoteBeginEvent event, ostream& out)
{
    out.put(event.noteNumber);
    out.put(event.velocity);
}

static void writeVelocityChangeEvent(Event::VelocityChangeEvent event, ostream& out)
{
    out.put(event.noteNumber);
    out.put(event.velocity);
}

static void writeControllerChangeEvent(Event::ControllerChangeEvent event, ostream& out)
{
    out.put(event.controllerNumber);
    out.put(event.velocity);
}

static void writeProgramChangeEvent(Event::ProgramChangeEvent event, ostream& out)
{
    out.put(event.newProgramNumber);
}

static void writeChannelPressureChangeEvent(Event::ChannelPressureChangeEvent event, ostream& out)
{
    out.put(event.channelNumber);
}

static void writePitchWheelChangeEvent(Event::PitchWheelChangeEvent event, ostream& out)
{
    out.put(event.bottom);
    out.put(event.top);
}

static void writeMetaEvent(const Event::MetaEvent event, ostream& out)
{
    out.put(event.command);
    out.put(event.length);
    for (int i = 0; i < event.length; i++)
        out.put(event.data[i]);
}

static void writeEventTime(unsigned int timeDelta, ostream& out)
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

static void writeEvent(const Event& event, ostream& out, unsigned char& previousCommandByte)
{
    writeEventTime(event.timeDelta, out);
    unsigned char commandByte = event.command | event.channel;
    if (commandByte != previousCommandByte || event.command == Event::Meta)
    {
        out.put(commandByte);
        previousCommandByte = commandByte;
    }
    switch(event.command)
    {
    case Event::NoteEnd:
        writeNoteEndEvent(event.noteEnd, out);
        break;
    case Event::NoteBegin:
        writeNoteBeginEvent(event.noteBegin, out);
        break;
    case Event::VelocityChange:
        writeVelocityChangeEvent(event.velocityChange, out);
        break;
    case Event::ControllerChange:
        writeControllerChangeEvent(event.controllerChange, out);
        break;
    case Event::ProgramChange:
        writeProgramChangeEvent(event.programChange, out);
        break;
    case Event::ChannelPressureChange:
        writeChannelPressureChangeEvent(event.channelPressureChange, out);
        break;
    case Event::PitchWheelChange:
        writePitchWheelChangeEvent(event.pitchWheelChange, out);
        break;
    case Event::Meta:
        writeMetaEvent(event.meta, out);
        break;
    default:
        throw std::exception();
    }
}

static unsigned int getTrackLength(const Track& track)
{
    unsigned int length = 0;

    unsigned char previousCommandByte = 0;
    for (const Event& event : track.events)
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
        if (commandByte != previousCommandByte || event.command == Event::Meta)
        {
            length += 1;
            previousCommandByte = commandByte;
        }

        // Data bytes
        switch(event.command)
        {
        // One data byte
        case Event::ProgramChange:
        case Event::ChannelPressureChange:
            length += 1;
            break;
        // Two data bytes
        case Event::NoteEnd:
        case Event::NoteBegin:
        case Event::VelocityChange:
        case Event::ControllerChange:
        case Event::PitchWheelChange:
            length += 2;
            break;
        // Variable data bytes
        case Event::Meta:
            length += 2 + event.meta.length;
            break;
        }
    }

    return length;
}

static void writeTrack(const Track& track, std::ostream& out)
{
    writeUInt32be(out, 'MTrk');
    writeUInt32be(out, getTrackLength(track));
    unsigned char previousCommandByte = 0;
    for (const Event& event : track.events)
    {
        writeEvent(event, out, previousCommandByte);
    }
}

void writeFile(const MidiFile& midi, std::ostream& out)
{
    writeUInt32be(out, 'MThd');
    writeUInt32be(out,  0x0006);
    writeUInt16be(out, midi.format);
    writeUInt16be(out, midi.tracks.size());
    writeUInt16be(out, midi.ticks);
    for (const Track& track : midi.tracks)
    {
        writeTrack(track, out);
    }
}

} // namespace midi