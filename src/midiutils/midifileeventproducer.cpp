#include <limits>

#include "midiutils.hpp"

using namespace std;

namespace midi {

MidiFileEventProducer::MidiFileEventProducer(MidiFile& midi)
    : m_midi(&midi),
      m_lastEventTime(0),
      m_indicies(midi.tracks.size()),
      m_absoluteTimes(midi.tracks.size()) {}

MidiFileEventProducer::~MidiFileEventProducer() {}

static const Event* getNextEventInTrack(const Track& track, uint32_t& index,
                                        uint32_t& absoluteTime) {
  while (index < track.events.size()) {
    const Event* event = &track.events[index++];
    absoluteTime += event->timeDelta;

    if (event->command != Event::Command::Meta) {
      return event;
    }
  }
  return nullptr;
}

const optional<Event> MidiFileEventProducer::getNextEvent() {
  const Event* bestEvent = nullptr;
  uint32_t bestTime = numeric_limits<uint32_t>::max();
  for (uint32_t i = 0; i < m_midi->tracks.size(); i++) {
    const Track& track = m_midi->tracks[i];
    const Event* event =
        getNextEventInTrack(track, m_indicies[i], m_absoluteTimes[i]);
    if (event && m_absoluteTimes[i] <= bestTime) {
      bestEvent = event;
      bestTime = m_absoluteTimes[i];
    }
  }
  return bestEvent ? optional<Event>{*bestEvent} : std::nullopt;
}

uint32_t MidiFileEventProducer::getTicksPerBeat() { return m_midi->ticks; }

}  // namespace midi
