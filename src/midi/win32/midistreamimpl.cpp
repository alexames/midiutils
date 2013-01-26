#include "midistreamimpl.hpp"

#include <windows.h>

namespace midi
{

struct MidiEventBuffer
{
	char eventDoubleBuffer[2][4096];
	int bytesFilled[2];

	int bufferIndex;

	bool fillBuffer();
	char* getBuffer();
	int getBytesFilled();
};

static unsigned char testNotes0[] = 
{ 
	0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x3C, 0x7F, 0x00,

	0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x40, 0x7F, 0x00,

	0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x43, 0x7F, 0x00,

	0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x40, 0x7F, 0x00
};

static unsigned char testNotes1[] = 
{ 
	0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x48, 0x7F, 0x00,

	0x60, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x4C, 0x7F, 0x00,

	0x60, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x4F, 0x7F, 0x00,

	0x60, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x4C, 0x7F, 0x00,

	0x60, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x48, 0x7F, 0x00,

	0x60, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x4C, 0x7F, 0x00,

	0x60, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x4F, 0x7F, 0x00,

	0x60, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x4C, 0x7F, 0x00
};

static unsigned char testNotes2[] = 
{ 
	0x60, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x3C, 0x4F, 0x00,

	0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x3E, 0x4F, 0x00,

	0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x40, 0x4F, 0x00,

	0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x90, 0x42, 0x4F, 0x00
};


bool MidiEventBuffer::fillBuffer()
{
	static int sourceIndex = 0;
	static unsigned char* testNotes[] = { testNotes0, testNotes1, testNotes2 };
	static int sizes[] = { 48, 96, 48 };

	for (int i = 0; i < sizes[sourceIndex]; i++)
	{
		eventDoubleBuffer[bufferIndex][i] = testNotes[sourceIndex][i];
	}
	bytesFilled[bufferIndex] = sizes[sourceIndex];
	sourceIndex = (sourceIndex+1) % 3;

	bufferIndex = 1 - bufferIndex;
	return true;
}

char* MidiEventBuffer::getBuffer()
{
	return eventDoubleBuffer[1 - bufferIndex];
}

int MidiEventBuffer::getBytesFilled()
{
	return bytesFilled[1 - bufferIndex];
}

struct MidiStreamImpl
{
	HMIDISTRM midiStreamHandle;
	HANDLE eventHandle;
	MIDIHDR midiHeader;
	MidiEventBuffer midiEventBuffer;
};

void CALLBACK midiStreamEventCallback(HMIDIOUT handle, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
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

MidiStreamImpl* createStreamImpl(MidiFile& midi)
{
	MidiStreamImpl* stream = new MidiStreamImpl();

	stream->eventHandle = CreateEvent(0, FALSE, FALSE, 0);
	
	unsigned int deviceHandle = 0;
	midiStreamOpen(&stream->midiStreamHandle, (LPUINT)&deviceHandle, 1, (DWORD)midiStreamEventCallback, (DWORD)stream, CALLBACK_FUNCTION);

	MIDIPROPTIMEDIV prop;
	prop.cbStruct = sizeof(MIDIPROPTIMEDIV);
	prop.dwTimeDiv = 96;
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

DWORD WINAPI prepareBufferThreadCallback(LPVOID lpParam) 
{
	MidiStreamImpl* stream = (MidiStreamImpl*)lpParam;

	if (!stream->midiEventBuffer.fillBuffer())
		return 0;

	while (true)
	{
		stream->midiHeader.lpData = (LPSTR)stream->midiEventBuffer.getBuffer();
		stream->midiHeader.dwBufferLength = stream->midiHeader.dwBytesRecorded = stream->midiEventBuffer.getBytesFilled();
		stream->midiHeader.dwFlags = 0;
		midiOutPrepareHeader((HMIDIOUT)stream->midiStreamHandle, &stream->midiHeader, sizeof(MIDIHDR));
		midiStreamOut(stream->midiStreamHandle, &stream->midiHeader, sizeof(MIDIHDR));
		
		if (!stream->midiEventBuffer.fillBuffer())
			break;

		WaitForSingleObject(stream->eventHandle, INFINITE);
	}

	return 0;
}

void playStreamImpl(MidiStreamImpl* stream)
{
	DWORD dwThreadId;
	HANDLE thread = CreateThread(NULL, 0, prepareBufferThreadCallback, stream, 0, &dwThreadId);
	midiStreamRestart(stream->midiStreamHandle);
}

} // namespace midi