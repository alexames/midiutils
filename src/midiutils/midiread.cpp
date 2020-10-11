#include <fstream>
#include <iostream>

#include "midiutils.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

namespace midi {

static uint32_t readUInt32be(istream& in) {
  char data[4];
  in.read(data, 4);
  return (static_cast<uint8_t>(data[0]) << 24) |
         (static_cast<uint8_t>(data[1]) << 16) |
         (static_cast<uint8_t>(data[2]) << 8) | static_cast<uint8_t>(data[3]);
}

static unsigned short readUInt16be(istream& in) {
  char data[2];
  in.read(static_cast<char*>(data), 2);
  return (static_cast<uint8_t>(data[0]) << 8) | static_cast<uint8_t>(data[1]);
}

////////////////////////////////////////////////////////////////////////////////

static bool isTrackEnd(const Event& event) {
  return event.command == Event::Command::Meta &&
         event.meta.command == Event::MetaCommand::EndOfTrack;
}

static void readEventTime(uint32_t& out, istream& in, uint32_t& byteCount) {
  out = 0;
  uint8_t byte;
  do {
    byte = in.get();
    out <<= 7;
    out += byte & 0x7f;

    if (--byteCount == 0) {
      throw exception();
    }
  } while (byte & 0x80);
}

static void readNoteEndEvent(Event::NoteEndEvent& event, istream& in,
                             uint32_t& byteCount) {
  event.noteNumber = in.get();
  event.velocity = in.get();
  byteCount -= 2;
}

static void readNoteBeginEvent(Event::NoteBeginEvent& event, istream& in,
                               uint32_t& byteCount) {
  event.noteNumber = in.get();
  event.velocity = in.get();
  byteCount -= 2;
}

static void readVelocityChangeEvent(Event::VelocityChangeEvent& event,
                                    istream& in, uint32_t& byteCount) {
  event.noteNumber = in.get();
  event.velocity = in.get();
  byteCount -= 2;
}

static void readControllerChangeEvent(Event::ControllerChangeEvent& event,
                                      istream& in, uint32_t& byteCount) {
  event.controllerNumber = in.get();
  event.velocity = in.get();
  byteCount -= 2;
}

static void readProgramChangeEvent(Event::ProgramChangeEvent& event,
                                   istream& in, uint32_t& byteCount) {
  event.newProgramNumber = in.get();
  byteCount -= 1;
}

static void readChannelPressureChangeEvent(
    Event::ChannelPressureChangeEvent& event, istream& in,
    uint32_t& byteCount) {
  event.channelNumber = in.get();
  byteCount -= 1;
}

static void readPitchWheelChangeEvent(Event::PitchWheelChangeEvent& event,
                                      istream& in, uint32_t& byteCount) {
  event.bottom = in.get();
  event.top = in.get();
  byteCount -= 2;
}

static void readMetaEvent(Event::MetaEvent& event, istream& in,
                          uint32_t& byteCount) {
  event.command = static_cast<Event::MetaCommand>(in.get());
  uint32_t length = event.length = in.get();
  byteCount -= 2 + event.length;
  event.data = (uint8_t*)malloc(event.length);
  for (uint32_t i = 0; i < length; i++) {
    event.data[i] = in.get();
  }
}

uint8_t readEventCommand(istream& in, uint8_t& previousCommandByte,
                         uint32_t& byteCount) {
  uint8_t commandByte = in.peek();
  if (commandByte & 0x80) {
    --byteCount;
    return in.get();
  } else {
    return previousCommandByte;
  }
}

static bool readEvent(Event& event, istream& in, uint32_t& byteCount,
                      uint8_t& previousCommandByte, bool strict) {
  readEventTime(event.timeDelta, in, byteCount);

  uint8_t commandByte = readEventCommand(in, previousCommandByte, byteCount);
  previousCommandByte = commandByte;
  event.command = static_cast<Event::Command>(commandByte & 0xF0);
  event.channel = commandByte & 0x0F;
  switch (event.command) {
    case Event::Command::NoteEnd:
      readNoteEndEvent(event.noteEnd, in, byteCount);
      break;
    case Event::Command::NoteBegin:
      readNoteBeginEvent(event.noteBegin, in, byteCount);
      break;
    case Event::Command::VelocityChange:
      readVelocityChangeEvent(event.velocityChange, in, byteCount);
      break;
    case Event::Command::ControllerChange:
      readControllerChangeEvent(event.controllerChange, in, byteCount);
      break;
    case Event::Command::ProgramChange:
      readProgramChangeEvent(event.programChange, in, byteCount);
      break;
    case Event::Command::ChannelPressureChange:
      readChannelPressureChangeEvent(event.channelPressureChange, in,
                                     byteCount);
      break;
    case Event::Command::PitchWheelChange:
      readPitchWheelChangeEvent(event.pitchWheelChange, in, byteCount);
      break;
    case Event::Command::Meta:
      readMetaEvent(event.meta, in, byteCount);
      if (isTrackEnd(event)) {
        if (strict && (byteCount != 0)) {
          throw exception("Invalid track length");
        } else {
          return false;
        }
      }
      break;
    default:
      throw exception("Invalid midi event");
  }
  return true;
}

static void readTrack(Track& track, istream& in, bool strict) {
  if (readUInt32be(in) != 'MTrk') {
    throw exception("Invalid midi track header");
  }
  uint32_t byteCount = readUInt32be(in);
  uint8_t previousCommand = 0;
  if (byteCount || !strict) {
    do {
      track.events.emplace_back();
    } while (
        readEvent(track.events.back(), in, byteCount, previousCommand, strict));
  }
}

void readFile(MidiFile& midi, istream& in, bool strict) {
  if (readUInt32be(in) != 'MThd' || readUInt32be(in) != 0x0006) {
    throw exception("Invalid midi header");
  }
  midi.format = static_cast<Format>(readUInt16be(in));
  midi.tracks.resize(readUInt16be(in));
  midi.ticks = readUInt16be(in);
  for (Track& track : midi.tracks) {
    readTrack(track, in, strict);
  }
}

}  // namespace midi