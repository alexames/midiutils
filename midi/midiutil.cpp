#include "midi.hpp"

const char* midi_errorString(midi_Error error)
{
	switch (error)
	{
	case midi_NoError:
		return "no error";
	case midi_ReadError:
		return "file read error";
	case midi_WriteError:
		return "file write error";
	case midi_InvalidHeader:
		return "invalid header";
	case midi_InvalidTrackHeader:
		return "invalid track header";
	case midi_InvalidTrackLength:
		return "invalid track length";
	case midi_InvalidEvent:
		return "invalid event";
	case midi_InvalidEventTimeDelta:
		return "invalid event time delta";
	default:
		return "unknown error";
	}
}
