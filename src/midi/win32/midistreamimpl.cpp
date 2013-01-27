#include "midi.hpp"
#include "midistreamimpl.hpp"

#define NOMINMAX // Who's idea was it to define a 'max' macro all lowercase?
#include "windows.h"

#include <vector>
#include <limits>

using namespace std;

namespace midi
{

static void writeUInt32le(char*& out, unsigned int val)
{
	*out++ = static_cast<char>(val >> 0);
	*out++ = static_cast<char>(val >> 8);
	*out++ = static_cast<char>(val >> 16);
	*out++ = static_cast<char>(val >> 24);
}

struct MidiEventBuffer
{
public:
	MidiEventBuffer();
	bool fillBuffer();
	char* getReadyBuffer();
	int getBytesFilled();

private:
	virtual bool fillBufferInternal(char* buffer, unsigned int& bytesFilled) = 0;

	char eventDoubleBuffer[2][4096];
	unsigned int bytesFilled[2];
	int readyBufferIndex;
	
};

struct MidiFileEventBuffer : public MidiEventBuffer
{
public:
	MidiFileEventBuffer(const MidiFile& midi);

private:

	virtual bool fillBufferInternal(char* buffer, unsigned int& bytesFilled);

	const MidiFile* m_midi;
	unsigned int m_lastEventTime;
	std::vector<unsigned int> m_indicies;
	std::vector<unsigned int> m_absoluteTimes;
};

struct MidiStreamImpl
{
	HMIDISTRM midiStreamHandle;
	HANDLE eventHandle;
	MIDIHDR midiHeader;
	MidiEventBuffer* midiEventBuffer;
};

MidiEventBuffer::MidiEventBuffer()
	: eventDoubleBuffer()
	, bytesFilled()
	, readyBufferIndex(0)
{
}

bool MidiEventBuffer::fillBuffer()
{
	bytesFilled[readyBufferIndex] = 0;
	readyBufferIndex = 1 - readyBufferIndex;
	return fillBufferInternal(eventDoubleBuffer[readyBufferIndex], bytesFilled[readyBufferIndex]);
}

char* MidiEventBuffer::getReadyBuffer()
{
	return eventDoubleBuffer[readyBufferIndex];
}

int MidiEventBuffer::getBytesFilled()
{
	return bytesFilled[readyBufferIndex];
}

MidiFileEventBuffer::MidiFileEventBuffer(const MidiFile& midi)
	: m_midi(&midi)
	, m_lastEventTime(0)
	, m_indicies(midi.tracks.size())
	, m_absoluteTimes(midi.tracks.size())
{
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

const Event* getNextEventInMidiFile(const MidiFile* midi, std::vector<unsigned int>& indicies, std::vector<unsigned int>& absoluteTimes, unsigned int& eventAbsoluteTime)
{
	const Event* bestEvent = nullptr;
	unsigned int bestTime = numeric_limits<unsigned int>::max();
	for (unsigned int i = 0; i < midi->tracks.size(); i++)
	{
		const Track& track = midi->tracks[i];
		const Event* event = getNextEventInTrack(track, indicies[i], absoluteTimes[i]);
		if (event && absoluteTimes[i] <= bestTime)
		{
			bestEvent = event;
			bestTime = absoluteTimes[i];
		}
	}
	eventAbsoluteTime = bestTime;
	return bestEvent;
}

bool MidiFileEventBuffer::fillBufferInternal(char* out, unsigned int& bytesFilled)
{
	bool songFinished = true;
	const Event* event;
	unsigned int eventAbsoluteTime;
	while ((bytesFilled < 4000) && (event = getNextEventInMidiFile(this->m_midi, this->m_indicies, this->m_absoluteTimes, eventAbsoluteTime)))
	{
		songFinished = false;
		writeMidiEventToBuffer(out, event, eventAbsoluteTime - m_lastEventTime, bytesFilled);
		m_lastEventTime = eventAbsoluteTime;
	}
	return !songFinished;
}

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

////////////////////////////////////////////////////////////////////////////////

MidiStreamImpl* createStreamImpl(MidiFile& midi)
{
	MidiStreamImpl* stream = new MidiStreamImpl();
	stream->midiEventBuffer = new MidiFileEventBuffer(midi);

	stream->eventHandle = CreateEvent(0, FALSE, FALSE, 0);
	
	unsigned int deviceHandle = 0;
	midiStreamOpen(&stream->midiStreamHandle, (LPUINT)&deviceHandle, 1, (DWORD)midiStreamEventCallback, (DWORD)stream, CALLBACK_FUNCTION);

	MIDIPROPTIMEDIV prop;
	prop.cbStruct = sizeof(MIDIPROPTIMEDIV);
	prop.dwTimeDiv = midi.ticks;
	midiStreamProperty(stream->midiStreamHandle, (LPBYTE)&prop, MIDIPROP_SET|MIDIPROP_TIMEDIV);
	
	return stream;
}

void destroyStreamImpl(MidiStreamImpl* stream)
{
    midiOutUnprepareHeader((HMIDIOUT)stream->midiStreamHandle, &stream->midiHeader, sizeof(MIDIHDR));
    midiStreamClose(stream->midiStreamHandle);
    CloseHandle(stream->eventHandle);
	delete stream;
}

void playStreamImpl(MidiStreamImpl* stream)
{
	DWORD dwThreadId;
	HANDLE thread = CreateThread(NULL, 0, prepareBufferThreadCallback, stream, 0, &dwThreadId);
	midiStreamRestart(stream->midiStreamHandle);
}

} // namespace midi