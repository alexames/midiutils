#include "midi.hpp"

#define NOMINMAX // Who's idea was it to define a 'max' macro all lowercase?
#include "windows.h"
#include "mmsyscom.h"

#include <vector>
#include <limits>

using namespace std;

namespace midi
{

struct MidiEventBuffer;

struct MidiStreamImpl
{
	HMIDISTRM midiStreamHandle;
	HANDLE eventHandle;
	MIDIHDR midiHeader;
	MidiEventBuffer* midiEventBuffer;
};

////////////////////////////////////////////////////////////////////////////////

static void writeUInt32le(char*& out, unsigned int val)
{
	*out++ = static_cast<char>(val >> 0);
	*out++ = static_cast<char>(val >> 8);
	*out++ = static_cast<char>(val >> 16);
	*out++ = static_cast<char>(val >> 24);
}

static void writeNoteEndEventToBuffer(const Event::NoteEndEvent& event, char*& out, unsigned int& bytesFilled)
{
	*out++ = event.noteNumber;
	*out++ = event.velocity;
	*out++ = 0;
	bytesFilled+=3;
}

static void writeNoteBeginEventToBuffer(const Event::NoteBeginEvent& event, char*& out, unsigned int& bytesFilled)
{
	*out++ = event.noteNumber;
	*out++ = event.velocity;
	*out++ = 0;
	bytesFilled+=3;
}

static void writeVelocityChangeEventToBuffer(const Event::VelocityChangeEvent& event, char*& out, unsigned int& bytesFilled)
{
	*out++ = event.noteNumber;
	*out++ = event.velocity;
	*out++ = 0;
	bytesFilled+=3;
}

static void writeControllerChangeEventToBuffer(const Event::ControllerChangeEvent& event, char*& out, unsigned int& bytesFilled)
{
	*out++ = event.controllerNumber;
	*out++ = event.velocity;
	*out++ = 0;
	bytesFilled+=3;
}

static void writeProgramChangeEventToBuffer(const Event::ProgramChangeEvent& event, char*& out, unsigned int& bytesFilled)
{
	*out++ = event.newProgramNumber;
	*out++ = 0;
	*out++ = 0;
	bytesFilled+=3;
}

static void writeChannelPressureChangeEventToBuffer(const Event::ChannelPressureChangeEvent& event, char*& out, unsigned int& bytesFilled)
{
	*out++ = event.channelNumber;
	*out++ = 0;
	*out++ = 0;
	bytesFilled+=3;
}

static void writePitchWheelChangeEventToBuffer(const Event::PitchWheelChangeEvent& event, char*& out, unsigned int& bytesFilled)
{
	*out++ = 0;
	*out++ = 0;
	*out++ = 0;
	bytesFilled+=3;
}

static void writeMidiEventToBuffer(char*& out, const Event* event, unsigned int timeDelta, unsigned int& bytesFilled)
{
	writeUInt32le(out, timeDelta);
	writeUInt32le(out, 0x00000000);
	*out++ = event->command | event->channel;
	bytesFilled+=9;
	switch (event->command)
	{
	case Event::NoteEnd:
		writeNoteEndEventToBuffer(event->noteEnd, out, bytesFilled);
		break;
	case Event::NoteBegin:
		writeNoteBeginEventToBuffer(event->noteBegin, out, bytesFilled);
		break;
	case Event::VelocityChange:
		writeVelocityChangeEventToBuffer(event->velocityChange, out, bytesFilled);
		break;
	case Event::ControllerChange:
		writeControllerChangeEventToBuffer(event->controllerChange, out, bytesFilled);
		break;
	case Event::ProgramChange:
		writeProgramChangeEventToBuffer(event->programChange, out, bytesFilled);
		break;
	case Event::ChannelPressureChange:
		writeChannelPressureChangeEventToBuffer(event->channelPressureChange, out, bytesFilled);
		break;
	case Event::PitchWheelChange:
		writePitchWheelChangeEventToBuffer(event->pitchWheelChange, out, bytesFilled);
		break;
	default:
		throw exception();
	}
}

////////////////////////////////////////////////////////////////////////////////

class MidiEventBuffer
{
public:
	MidiEventBuffer(EventProducer& producer);
	bool fillBuffer();
	char* getReadyBuffer();
	int getBytesFilled();

private:	
	EventProducer* m_producer;
	char m_eventDoubleBuffer[2][4096];
	unsigned int m_bytesFilled[2];
	int m_readyBufferIndex;
	unsigned int m_lastEventTime;
};

MidiEventBuffer::MidiEventBuffer(EventProducer& producer)
	: m_producer(&producer)
	, m_eventDoubleBuffer()
	, m_bytesFilled()
	, m_readyBufferIndex(0)
	, m_lastEventTime(0)
{
}

bool MidiEventBuffer::fillBuffer()
{
	m_readyBufferIndex = 1 - m_readyBufferIndex;
	m_bytesFilled[m_readyBufferIndex] = 0;
	char* buffer = m_eventDoubleBuffer[m_readyBufferIndex];
	bool songFinished = true;
	const Event* event;
	unsigned int eventAbsoluteTime;
	while ((m_bytesFilled[m_readyBufferIndex] < 4000) && (event = m_producer->getNextEvent(eventAbsoluteTime)))
	{
		songFinished = false;
		writeMidiEventToBuffer(buffer, event, eventAbsoluteTime - m_lastEventTime, m_bytesFilled[m_readyBufferIndex]);
		m_lastEventTime = eventAbsoluteTime;
	}
	return !songFinished;
}

char* MidiEventBuffer::getReadyBuffer()
{
	return m_eventDoubleBuffer[m_readyBufferIndex];
}

int MidiEventBuffer::getBytesFilled()
{
	return m_bytesFilled[m_readyBufferIndex];
}

////////////////////////////////////////////////////////////////////////////////

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

unsigned int MidiFileEventProducer::getInitialTempo()
{
	return m_midi->ticks;
}

////////////////////////////////////////////////////////////////////////////////

static void CALLBACK midiStreamEventCallback(HMIDIOUT handle, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	MidiStreamImpl* stream = (MidiStreamImpl*)dwInstance;	
	switch (uMsg)
	{
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

static DWORD WINAPI prepareBufferThreadCallback(LPVOID lpParam) 
{
	MidiStreamImpl* stream = (MidiStreamImpl*)lpParam;

	if (!stream->midiEventBuffer->fillBuffer())
		return 0;

	while (true)
	{
		stream->midiHeader.lpData = (LPSTR)stream->midiEventBuffer->getReadyBuffer();
		stream->midiHeader.dwBufferLength = stream->midiHeader.dwBytesRecorded = stream->midiEventBuffer->getBytesFilled();
		stream->midiHeader.dwFlags = 0;
		midiOutPrepareHeader((HMIDIOUT)stream->midiStreamHandle, &stream->midiHeader, sizeof(MIDIHDR));
		midiStreamOut(stream->midiStreamHandle, &stream->midiHeader, sizeof(MIDIHDR));
		
		if (!stream->midiEventBuffer->fillBuffer())
			break;

		WaitForSingleObject(stream->eventHandle, INFINITE);
	}

	return 0;
}

static MidiStreamImpl* createStreamImpl(EventProducer& producer)
{
	MidiStreamImpl* stream = new MidiStreamImpl();
	
	stream->midiEventBuffer = new MidiEventBuffer(producer);

	stream->eventHandle = CreateEvent(0, FALSE, FALSE, 0);
	
	unsigned int deviceHandle = 0;
	midiStreamOpen(&stream->midiStreamHandle, (LPUINT)&deviceHandle, 1, (DWORD)midiStreamEventCallback, (DWORD)stream, CALLBACK_FUNCTION);

	MIDIPROPTIMEDIV prop;
	prop.cbStruct = sizeof(MIDIPROPTIMEDIV);
	prop.dwTimeDiv = producer.getInitialTempo();
	midiStreamProperty(stream->midiStreamHandle, (LPBYTE)&prop, MIDIPROP_SET|MIDIPROP_TIMEDIV);
	
	return stream;
}

////////////////////////////////////////////////////////////////////////////////
//
// Public API
//

MidiStream::MidiStream(EventProducer& producer)
	: impl(createStreamImpl(producer))
{
}

MidiStream::~MidiStream()
{
	midiOutUnprepareHeader((HMIDIOUT)impl->midiStreamHandle, &impl->midiHeader, sizeof(MIDIHDR));
	midiStreamClose(impl->midiStreamHandle);
	CloseHandle(impl->eventHandle);
	delete impl;
}

void MidiStream::play()
{
	DWORD dwThreadId;
	HANDLE thread = CreateThread(NULL, 0, prepareBufferThreadCallback, impl, 0, &dwThreadId);
	midiStreamRestart(impl->midiStreamHandle);
}

unsigned int MidiStream::getPosition() const
{
	MMTIME time;
	time.wType = TIME_MIDI; 
	if (midiStreamPosition(impl->midiStreamHandle, &time, sizeof(MMTIME)) || time.wType != TIME_MIDI)
	{
		throw exception("error requesting stream position");
	}
	else
	{
		return time.u.midi.songptrpos;
	}
}

} // namespace midi