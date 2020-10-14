#include "midiutils.hpp"

#define NOMINMAX  // Who's idea was it to define a 'max' macro all lowercase?
#include <algorithm>
#include <vector>

#include "spdlog/spdlog.h"

// clang-format off
#include "windows.h"
#include "mmsyscom.h"
// clang-format on

using namespace std;

namespace midi {

class MidiEventBuffer;

struct MidiStreamImpl {
  HMIDISTRM midiStreamHandle;
  HANDLE eventHandle;
  MIDIHDR midiHeader;
  MidiEventBuffer* midiEventBuffer;
};

////////////////////////////////////////////////////////////////////////////////

static void writeUInt32le(uint8_t*& out, unsigned int val) {
  *out++ = static_cast<char>(val >> 0);
  *out++ = static_cast<char>(val >> 8);
  *out++ = static_cast<char>(val >> 16);
  *out++ = static_cast<char>(val >> 24);
}

static void writeNoteEndEventToBuffer(const Event::NoteEndEvent& event,
                                      uint8_t*& out, std::size_t& bytesFilled) {
  *out++ = event.noteNumber;
  *out++ = event.velocity;
  *out++ = 0;
  bytesFilled += 3;
}

static void writeNoteBeginEventToBuffer(const Event::NoteBeginEvent& event,
                                        uint8_t*& out,
                                        std::size_t& bytesFilled) {
  *out++ = event.noteNumber;
  *out++ = event.velocity;
  *out++ = 0;
  bytesFilled += 3;
}

static void writeVelocityChangeEventToBuffer(
    const Event::VelocityChangeEvent& event, uint8_t*& out,
    std::size_t& bytesFilled) {
  *out++ = event.noteNumber;
  *out++ = event.velocity;
  *out++ = 0;
  bytesFilled += 3;
}

static void writeControllerChangeEventToBuffer(
    const Event::ControllerChangeEvent& event, uint8_t*& out,
    std::size_t& bytesFilled) {
  *out++ = event.controllerNumber;
  *out++ = event.velocity;
  *out++ = 0;
  bytesFilled += 3;
}

static void writeProgramChangeEventToBuffer(
    const Event::ProgramChangeEvent& event, uint8_t*& out,
    std::size_t& bytesFilled) {
  *out++ = event.newProgramNumber;
  *out++ = 0;
  *out++ = 0;
  bytesFilled += 3;
}

static void writeChannelPressureChangeEventToBuffer(
    const Event::ChannelPressureChangeEvent& event, uint8_t*& out,
    std::size_t& bytesFilled) {
  *out++ = event.channelNumber;
  *out++ = 0;
  *out++ = 0;
  bytesFilled += 3;
}

static void writePitchWheelChangeEventToBuffer(
    const Event::PitchWheelChangeEvent& event, uint8_t*& out,
    std::size_t& bytesFilled) {
  *out++ = 0;
  *out++ = 0;
  *out++ = 0;
  bytesFilled += 3;
}

static void writeMidiEventToBuffer(uint8_t*& out, const Event& event,
                                   std::size_t& bytesFilled) {
  writeUInt32le(out, event.timeDelta);
  writeUInt32le(out, 0x00000000);
  *out++ = static_cast<uint8_t>(event.command) | event.channel;
  bytesFilled += 9;
  switch (event.command) {
    case Event::Command::NoteEnd:
      writeNoteEndEventToBuffer(event.noteEnd, out, bytesFilled);
      break;
    case Event::Command::NoteBegin:
      writeNoteBeginEventToBuffer(event.noteBegin, out, bytesFilled);
      break;
    case Event::Command::VelocityChange:
      writeVelocityChangeEventToBuffer(event.velocityChange, out, bytesFilled);
      break;
    case Event::Command::ControllerChange:
      writeControllerChangeEventToBuffer(event.controllerChange, out,
                                         bytesFilled);
      break;
    case Event::Command::ProgramChange:
      writeProgramChangeEventToBuffer(event.programChange, out, bytesFilled);
      break;
    case Event::Command::ChannelPressureChange:
      writeChannelPressureChangeEventToBuffer(event.channelPressureChange, out,
                                              bytesFilled);
      break;
    case Event::Command::PitchWheelChange:
      writePitchWheelChangeEventToBuffer(event.pitchWheelChange, out,
                                         bytesFilled);
      break;
    default:
      spdlog::error("Invalid command: {}", event.command);
      throw exception();
  }
}

////////////////////////////////////////////////////////////////////////////////

class MidiEventBuffer {
 public:
  MidiEventBuffer(EventProducer& producer);
  bool fillBuffer();
  uint8_t* getReadyBuffer();
  int getBytesFilled();

 private:
  EventProducer* m_producer;
  uint8_t m_eventDoubleBuffer[2][4096];
  std::size_t m_bytesFilled[2];
  int m_readyBufferIndex;

  std::vector<uint32_t> m_channelLastEventTimes;
};

MidiEventBuffer::MidiEventBuffer(EventProducer& producer)
    : m_producer(&producer),
      m_eventDoubleBuffer(),
      m_bytesFilled(),
      m_readyBufferIndex(0),
      m_channelLastEventTimes{} {}

bool MidiEventBuffer::fillBuffer() {
  m_readyBufferIndex = 1 - m_readyBufferIndex;
  m_bytesFilled[m_readyBufferIndex] = 0;
  auto* buffer = m_eventDoubleBuffer[m_readyBufferIndex];
  optional<Event> event;
  m_producer->preBufferFill();
  while ((m_bytesFilled[m_readyBufferIndex] < 4000) &&
         (event = m_producer->getNextEvent())) {
    // Ensure that we are tracking all channels' absolute times.
    if (event->channel >= m_channelLastEventTimes.size()) {
      m_channelLastEventTimes.resize(event->channel + 1, 0);
    }
    // Get the absolute time of the current event
    auto eventAbsoluteTime =
        m_channelLastEventTimes[event->channel] + event->timeDelta;
    // Get the absolute time of the most recent event.
    auto lastEventAbsoluteTime = *std::max_element(
        m_channelLastEventTimes.begin(), m_channelLastEventTimes.end());
    // Update the absolute time in the cache.
    m_channelLastEventTimes[event->channel] += event->timeDelta;
    // Somehow got an event that has already passed. Do nothing on continue.
    if (eventAbsoluteTime < lastEventAbsoluteTime) {
      continue;
    }
    // When playing a midi event, delta time represents the time since the most
    // recent event on any channel, not the most recent event on the event's
    // channel.
    event->timeDelta = eventAbsoluteTime - lastEventAbsoluteTime;
    writeMidiEventToBuffer(buffer, *event, m_bytesFilled[m_readyBufferIndex]);
  }
  return true;
}

uint8_t* MidiEventBuffer::getReadyBuffer() {
  return m_eventDoubleBuffer[m_readyBufferIndex];
}

int MidiEventBuffer::getBytesFilled() {
  return m_bytesFilled[m_readyBufferIndex];
}

////////////////////////////////////////////////////////////////////////////////

static void CALLBACK midiStreamEventCallback(HMIDIOUT handle, UINT uMsg,
                                             DWORD_PTR dwInstance,
                                             DWORD dwParam1, DWORD dwParam2) {
  MidiStreamImpl* stream = reinterpret_cast<MidiStreamImpl*>(dwInstance);
  switch (uMsg) {
    case MOM_POSITIONCB:
      break;

    case MOM_DONE:
      SetEvent(stream->eventHandle);
      break;

    case MOM_OPEN:
      break;

    case MOM_CLOSE:
      break;
  }
}

static DWORD WINAPI prepareBufferThreadCallback(LPVOID lpParam) {
  MidiStreamImpl* stream = (MidiStreamImpl*)lpParam;

  if (!stream->midiEventBuffer->fillBuffer()) return 0;

  while (true) {
    stream->midiHeader.lpData =
        (LPSTR)stream->midiEventBuffer->getReadyBuffer();
    stream->midiHeader.dwBufferLength = stream->midiHeader.dwBytesRecorded =
        stream->midiEventBuffer->getBytesFilled();
    stream->midiHeader.dwFlags = 0;
    midiOutPrepareHeader((HMIDIOUT)stream->midiStreamHandle,
                         &stream->midiHeader, sizeof(MIDIHDR));
    midiStreamOut(stream->midiStreamHandle, &stream->midiHeader,
                  sizeof(MIDIHDR));

    if (!stream->midiEventBuffer->fillBuffer()) break;

    WaitForSingleObject(stream->eventHandle, INFINITE);
  }

  return 0;
}

static MidiStreamImpl* createStreamImpl(EventProducer& producer) {
  MidiStreamImpl* stream = new MidiStreamImpl();

  stream->midiEventBuffer = new MidiEventBuffer(producer);

  stream->eventHandle = CreateEvent(0, FALSE, FALSE, 0);

  UINT deviceHandle = 0;
  midiStreamOpen(&stream->midiStreamHandle, &deviceHandle, 1,
                 (DWORD_PTR)midiStreamEventCallback, (DWORD_PTR)stream,
                 CALLBACK_FUNCTION);

  MIDIPROPTIMEDIV prop;
  prop.cbStruct = sizeof(MIDIPROPTIMEDIV);
  prop.dwTimeDiv = producer.getTicksPerBeat();
  midiStreamProperty(stream->midiStreamHandle, (LPBYTE)&prop,
                     MIDIPROP_SET | MIDIPROP_TIMEDIV);

  return stream;
}

////////////////////////////////////////////////////////////////////////////////
//
// Public API
//

MidiStream::MidiStream(EventProducer& producer)
    : impl(createStreamImpl(producer)) {}

MidiStream::~MidiStream() {
  midiOutUnprepareHeader((HMIDIOUT)impl->midiStreamHandle, &impl->midiHeader,
                         sizeof(MIDIHDR));
  midiStreamClose(impl->midiStreamHandle);
  CloseHandle(impl->eventHandle);
  delete impl;
}

void MidiStream::play() {
  DWORD dwThreadId;
  HANDLE thread =
      CreateThread(NULL, 0, prepareBufferThreadCallback, impl, 0, &dwThreadId);
  midiStreamRestart(impl->midiStreamHandle);
}

unsigned int MidiStream::getMilliseconds() const {
  MMTIME time;
  time.wType = TIME_MIDI;
  if (midiStreamPosition(impl->midiStreamHandle, &time, sizeof(MMTIME)) ||
      time.wType != TIME_MIDI) {
    throw exception("error requesting stream position");
  } else {
    return time.u.midi.songptrpos;
  }
}

unsigned int MidiStream::getTicks() const {
  MMTIME time;
  time.wType = TIME_TICKS;
  if (midiStreamPosition(impl->midiStreamHandle, &time, sizeof(MMTIME)) ||
      time.wType != TIME_TICKS) {
    throw exception("error requesting stream position");
  } else {
    return time.u.midi.songptrpos;
  }
}

}  // namespace midi