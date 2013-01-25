#include "midistreamimpl.hpp"

#include <windows.h>

namespace midi
{

struct MidiStreamImpl
{
	HMIDISTRM midiStreamHandle;
	HANDLE eventHandle;
	MIDIHDR midiHeader;
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
	static unsigned long myNotes[] = {0, 0, 0x007F3C90, 192, 0, 0x00003C80};

	MidiStreamImpl* stream = new MidiStreamImpl();
	
	stream->eventHandle = CreateEvent(0, FALSE, FALSE, 0);
	
	unsigned int deviceHandle = 0;
	midiStreamOpen(&stream->midiStreamHandle, (LPUINT)&deviceHandle, 1, (DWORD)midiStreamEventCallback, (DWORD)stream, CALLBACK_FUNCTION);

	MIDIPROPTIMEDIV prop;
	prop.cbStruct = sizeof(MIDIPROPTIMEDIV);
	prop.dwTimeDiv = 96;
	midiStreamProperty(stream->midiStreamHandle, (LPBYTE)&prop, MIDIPROP_SET|MIDIPROP_TIMEDIV);

	stream->midiHeader.lpData = (LPSTR)&myNotes[0];
	stream->midiHeader.dwBufferLength = stream->midiHeader.dwBytesRecorded = sizeof(myNotes);
	stream->midiHeader.dwFlags = 0;
	midiOutPrepareHeader((HMIDIOUT)stream->midiStreamHandle, &stream->midiHeader, sizeof(MIDIHDR));

	midiStreamOut(stream->midiStreamHandle, &stream->midiHeader, sizeof(MIDIHDR));
	
	return stream;
}

void destroyStreamImpl(MidiStreamImpl* stream)
{
    midiOutUnprepareHeader((HMIDIOUT)stream->midiStreamHandle, &stream->midiHeader, sizeof(MIDIHDR));
    midiStreamClose(stream->midiStreamHandle);
    CloseHandle(stream->eventHandle);
	delete stream;
}

DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
{
	MidiStreamImpl* stream = (MidiStreamImpl*)lpParam;
	while (true)
	{
		WaitForSingleObject(stream->eventHandle, INFINITE);
	}
	return 0;
}

void playStreamImpl(MidiStreamImpl* stream)
{
	DWORD dwThreadId;
	HANDLE thread = CreateThread(NULL, 0, MyThreadFunction, stream, 0, &dwThreadId);
	midiStreamRestart(stream->midiStreamHandle);
}

} // namespace midi