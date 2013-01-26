#include "midi.hpp"
#include "midistreamimpl.hpp"

namespace midi
{

MidiStream::MidiStream(MidiFile& midi)
	: impl(createStreamImpl(midi))
{
}

MidiStream::~MidiStream()
{
	destroyStreamImpl(impl);
}

void MidiStream::play()
{
	playStreamImpl(impl);
}

} // namespace midi