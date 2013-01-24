#include "midistreamimpl.hpp"

#include <windows.h>

namespace midi
{

////////////////////////////////////////////////////////////////////////////////
	
static unsigned int swapEndianness(unsigned int val)
{
	return 
		((val & 0xFF000000) >> 24)
		| ((val & 0x00FF0000) >> 8)
		| ((val & 0x0000FF00) << 8)
		| ((val & 0x000000FF) << 24);
}

static unsigned short swapEndianness(unsigned short val)
{
	return (val >> 8) | (val << 8);
}

////////////////////////////////////////////////////////////////////////////////

void CALLBACK example9_callback(HMIDIOUT out, UINT msg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	HANDLE* event = (HANDLE*)dwInstance;
    switch (msg)
    {
    case MOM_DONE:
        SetEvent(*event);
        break;
    case MOM_POSITIONCB:
    case MOM_OPEN:
    case MOM_CLOSE:
        break;
    }
}

struct MidiStreamImpl
{
	HMIDISTRM out;
	HANDLE event;
	MIDIHDR mhdr;
	unsigned int device;
	char* streambuf;
	unsigned int streambufsize;
};

MidiStreamImpl* createStreamImpl(MidiFile& midi)
{
	MidiStreamImpl* stream = new MidiStreamImpl();
	midiStreamOpen(&stream->out, &stream->device, 1, (DWORD)example9_callback, (DWORD_PTR)&stream->event, CALLBACK_FUNCTION);
	
	MIDIPROPTIMEDIV prop;
	prop.cbStruct = sizeof(MIDIPROPTIMEDIV);
	prop.dwTimeDiv = swapEndianness(midi.ticks);
	midiStreamProperty(stream->out, (LPBYTE)&prop, MIDIPROP_SET|MIDIPROP_TIMEDIV);

	stream->mhdr.lpData = stream->streambuf;
	stream->mhdr.dwBufferLength = stream->mhdr.dwBytesRecorded = stream->streambufsize;
	stream->mhdr.dwFlags = 0;
	midiOutPrepareHeader((HMIDIOUT)stream->out, &stream->mhdr, sizeof(MIDIHDR));

	return stream;
}

void destroyStreamImpl(MidiStreamImpl* stream)
{
	midiOutReset((HMIDIOUT)stream->out);
	midiOutUnprepareHeader((HMIDIOUT)stream->out, &stream->mhdr, sizeof(MIDIHDR));
	midiStreamClose(stream->out);
}

void playStreamImpl(MidiStreamImpl* stream)
{
	midiStreamRestart(stream->out);
}

} // namespace midi