#ifndef MIDI_STREAM_IMPL_H_
#define MIDI_STREAM_IMPL_H_

#include "midi.hpp"

namespace midi
{

MidiStreamImpl* createStreamImpl(MidiFile& midi);

void destroyStreamImpl(MidiStreamImpl* impl);

void playStreamImpl(MidiStreamImpl* impl);

} // namespace midi

#endif // MIDI_STREAM_IMPL_H_