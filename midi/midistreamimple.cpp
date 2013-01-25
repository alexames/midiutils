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

struct MidiStreamImpl
{
	HMIDISTRM midiStreamHandle;
	HANDLE eventHandle;
	MIDIHDR midiHeader;
	unsigned int deviceHandle;
	char* streambuf;
	unsigned int streambufsize;
};

void CALLBACK midiCallback(HMIDIOUT handle, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	HANDLE eventHandle = (HANDLE)dwParam1;
    LPMIDIHDR   lpMIDIHeader;
    MIDIEVENT * lpMIDIEvent;

    /* Determine why Windows called me */
    switch (uMsg)
    {
        /* Got some event with its MEVT_F_CALLBACK flag set */
        case MOM_POSITIONCB:

            /* Assign address of MIDIHDR to a LPMIDIHDR variable. Makes it easier to access the
               field that contains the pointer to our block of MIDI events */
            lpMIDIHeader = (LPMIDIHDR)dwParam1;

            /* Get address of the MIDI event that caused this call */
            lpMIDIEvent = (MIDIEVENT *)&(lpMIDIHeader->lpData[lpMIDIHeader->dwOffset]);

            /* Normally, if you had several different types of events with the
               MEVT_F_CALLBACK flag set, you'd likely now do a switch on the highest
               byte of the dwEvent field, assuming that you need to do different
               things for different types of events.
            */

            break;

        /* The last event in the MIDIHDR has played */
        case MOM_DONE:

            /* Wake up main() */
            SetEvent(eventHandle);

            break;


        /* Process these messages if you desire */
        case MOM_OPEN:
        case MOM_CLOSE:

            break;
    }
}

MidiStreamImpl* createStreamImpl(MidiFile& midi)
{
	static unsigned long myNotes[] = {0, 0, 0x007F3C90, 192, 0, 0x00003C90};

	MidiStreamImpl* stream = new MidiStreamImpl();
	
	stream->eventHandle = CreateEvent(0, FALSE, FALSE, 0);

	midiStreamOpen(&stream->midiStreamHandle, (LPUINT)&stream->deviceHandle, 1, (DWORD)midiCallback, (DWORD)stream->eventHandle, CALLBACK_FUNCTION);

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

void playStreamImpl(MidiStreamImpl* stream)
{
	midiStreamRestart(stream->midiStreamHandle);
	WaitForSingleObject(stream->eventHandle, INFINITE);
}

} // namespace midi