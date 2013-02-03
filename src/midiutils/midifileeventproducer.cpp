#include "midiutils.hpp"

#include <limits>

using namespace std;

namespace midi
{

MidiFileEventProducer::MidiFileEventProducer(MidiFile& midi)
	: m_midi(&midi)
	, m_lastEventTime(0)
	, m_indicies(midi.tracks.size())
	, m_absoluteTimes(midi.tracks.size())
{
}

static const Event* getNextEventInTrack(const Track& track, unsigned int& index, unsigned int& absoluteTime)
{
	while (index < track.events.size())
	{
		const Event* event = &track.events[index++];
		absoluteTime += event->timeDelta;
			
		if (event->command != Event::Meta)
		{
			return event;
		}
	}
	return nullptr;
}

const Event* MidiFileEventProducer::getNextEvent(unsigned int& absoluteTime)
{
	const Event* bestEvent = nullptr;
	unsigned int bestTime = numeric_limits<unsigned int>::max();
	for (unsigned int i = 0; i < m_midi->tracks.size(); i++)
	{
		const Track& track = m_midi->tracks[i];
		const Event* event = getNextEventInTrack(track, m_indicies[i], m_absoluteTimes[i]);
		if (event && m_absoluteTimes[i] <= bestTime)
		{
			bestEvent = event;
			bestTime = m_absoluteTimes[i];
		}
	}
	absoluteTime = bestTime;
	return bestEvent;
}

unsigned int MidiFileEventProducer::getTicksPerBeat()
{
	return m_midi->ticks;
}

} // namespace midi
