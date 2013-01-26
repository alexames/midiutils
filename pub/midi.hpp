#ifndef MIDI_H_
#define MIDI_H_

#include <istream>
#include <vector>

namespace midi
{

enum Format
{
	SingleTrack,
	MultiTrackSynchronous,
	MultiTrackAsynchronous
};

enum Instrument
{
	// Piano
	Acoustic_Grand,
	Bright_Acoustic,
	Electric_Grand,
	Honky_Tonk,
	Electric_Piano_1,
	Electric_Piano_2,
	Harpsichord,
	Clav,
	
	// Chrome Percussion
	Celesta,
	Glockenspiel,
	Music_Box,
	Vibraphone,
	Marimba,
	Xylophone,
	Tubular_Bells,
	Dulcimer,
	
	// Organ
	Drawbar_Organ,
	Percussive_Organ,
	Rock_Organ,
	Church_Organ,
	Reed_Organ,
	Accoridan,
	Harmonica,
	Tango_Accordian,
	
	// Guitar
	Acoustic_Guitar_Nylon,
	Acoustic_Guitar_Steel,
	Electric_Guitar_Jazz,
	Electric_Guitar_Clean,
	Electric_Guitar_Muted,
	Overdriven_Guitar,
	Distortion_Guitar,
	Guitar_Harmonics,
	
	// Bass
	Acoustic_Bass,
	Electric_Bassfinger,
	Electric_Basspick,
	Fretless_Bass,
	Slap_Bass_1,
	Slap_Bass_2,
	Synth_Bass_1,
	Synth_Bass_2,
	
	// Strings
	Violin,
	Viola,
	Cello,
	Contrabass,
	Tremolo_Strings,
	Pizzicato_Strings,
	Orchestral_Strings,
	Timpani,
	
	// Ensemble
	String_Ensemble_1,
	String_Ensemble_2,
	SynthStrings_1,
	SynthStrings_2,
	Choir_Aahs,
	Voice_Oohs,
	Synth_Voice,
	Orchestra_Hit,
	
	// Brass
	Trumpet,
	Trombone,
	Tuba,
	Muted_Trumpet,
	French_Horn,
	Brass_Section,
	SynthBrass_1,
	SynthBrass_2,
	
	// Reed
	Soprano_Sax,
	Alto_Sax,
	Tenor_Sax,
	Baritone_Sax,
	Oboe,
	English_Horn,
	Bassoon,
	Clarinet,
	
	// Pipe
	Piccolo,
	Flute,
	Recorder,
	Pan_Flute,
	Blown_Bottle,
	Skakuhachi,
	Whistle,
	Ocarina,
	
	// Synth Lead
	Lead_1_Square,
	Lead_2_Sawtooth,
	Lead_3_Calliope,
	Lead_4_Chiff,
	Lead_5_Charang,
	Lead_6_Voice,
	Lead_7_Fifths,
	Lead_8_Bass_Lead,
	
	// Synth Pad
	Pad_1_New_Age,
	Pad_2_Warm,
	Pad_3_Polysynth,
	Pad_4_Choir,
	Pad_5_Bowed,
	Pad_6_Metallic,
	Pad_7_Halo,
	Pad_8_Sweep,
	
	// Synth Effects
	FX_1_Rain,
	FX_2_Soundtrack,
	FX_3_Crystal,
	FX_4_Atmosphere,
	FX_5_Brightness,
	FX_6_Goblins,
	FX_7_Echoes,
	FX_8_Scifi,
	
	// Ethnic
	Sitar,
	Banjo,
	Shamisen,
	Koto,
	Kalimba,
	Bagpipe,
	Fiddle,
	Shanai,
	
	// Percussive
	Tinkle_Bell,
	Agogo,
	Steel_Drums,
	Woodblock,
	Taiko_Drum,
	Melodic_Tom,
	Synth_Drum,
	Reverse_Cymbal,
	
	// Sound Effects
	Guitar_Fret_Noise,
	Breath_Noise,
	Seashore,
	Bird_Tweet,
	Telephone_Ring,
	Helicopter,
	Applause,
	Gunshot
};

struct Event
{
	struct NoteEndEvent
	{
		unsigned char noteNumber;
		unsigned char velocity;
	};

	struct NoteBeginEvent
	{
		unsigned char noteNumber;
		unsigned char velocity;
	};

	struct VelocityChangeEvent
	{
		unsigned char noteNumber;
		unsigned char velocity;
	};

	struct ControllerChangeEvent
	{
		unsigned char controllerNumber;
		unsigned char velocity;
	};

	struct ProgramChangeEvent
	{
		unsigned char newProgramNumber;
	};

	struct ChannelPressureChangeEvent
	{
		unsigned char channelNumber;
	};

	struct PitchWheelChangeEvent
	{
		unsigned char bottom;
		unsigned char top;
	};

	struct MetaEvent
	{
		struct SetSequenceNumberEvent
		{
			unsigned int sequenceNumber;
		};
		struct TextEvent
		{
			unsigned int TODO;
		};
		struct CopywriteEvent
		{
			unsigned int TODO;
		};
		struct SequnceNameEvent
		{
			unsigned int TODO;
		};
		struct TrackInstrumentNameEvent
		{
			unsigned int TODO;
		};
		struct LyricEvent
		{
			unsigned int TODO;
		};
		struct MarkerEvent
		{
			unsigned int TODO;
		};
		struct CueEvent
		{
			unsigned int TODO;
		};
		struct PrefixAssignmentEvent
		{
			unsigned int TODO;
		};
		struct EndOfTrackEvent
		{
			unsigned int TODO;
		};
		struct SetTempoEvent
		{
			unsigned int TODO;
		};
		struct SMPTEOffsetEvent
		{
			unsigned int TODO;
		};
		struct TimeSignatureEvent
		{
			unsigned int TODO;
		};
		struct KeySignatureEvent
		{
			unsigned int TODO;
		};
		struct SequencerSpecificEvent
		{
			unsigned int TODO;
		};

		enum MetaCommand
		{
			SetSequenceNumber = 0x00,
			Text = 0x01,
			Copywrite = 0x02,
			SequnceName = 0x03,
			TrackInstrumentName = 0x04,
			Lyric = 0x05,
			Marker = 06,
			Cue = 0x07,
			PrefixAssignment = 0x20,
			EndOfTrack = 0x2F,
			SetTempo = 0x51,
			SMPTEOffset = 0x54,
			TimeSignature = 0x58,
			KeySignature = 0x59,
			SequencerSpecific = 0x7F
		};

		MetaCommand command;
		unsigned char length;
		char* data;
		union 
		{
			SetSequenceNumberEvent setSequenceNumber;
			TextEvent text;
			CopywriteEvent copywrite;
			SequnceNameEvent sequnceName;
			TrackInstrumentNameEvent trackInstrumentName;
			LyricEvent lyric;
			MarkerEvent marker;
			CueEvent cue;
			PrefixAssignmentEvent prefixAssignment;
			EndOfTrackEvent endOfTrack;
			SetTempoEvent setTempo;
			SMPTEOffsetEvent smtpeOffset;
			TimeSignatureEvent timeSignature;
			KeySignatureEvent keySignature;
			SequencerSpecificEvent sequencerSpecific;
		};
	};

	enum Command
	{
		NoteEnd = 0x80,
		NoteBegin = 0x90,
		VelocityChange = 0xA0,
		ControllerChange = 0xB0,
		ProgramChange = 0xC0,
		ChannelPressureChange = 0xD0,
		PitchWheelChange = 0xE0,
		Meta = 0xF0
	};

	unsigned int timeDelta;
	Command command;
	unsigned char channel;
	union
	{
		NoteEndEvent noteEnd;
		NoteBeginEvent noteBegin;
		VelocityChangeEvent velocityChange;
		ControllerChangeEvent controllerChange;
		ProgramChangeEvent programChange;
		ChannelPressureChangeEvent channelPressureChange;
		PitchWheelChangeEvent pitchWheelChange;
		MetaEvent meta;
	};
};

struct Track 
{
	std::vector<Event> events;
};

struct MidiFile
{	
	Format format;
	unsigned short ticks;
	std::vector<Track> tracks;
};

struct MidiStreamImpl;

class MidiStream
{
public:
	MidiStream(MidiFile& midi);
	~MidiStream();
	
	void play();

private: 
	MidiStreamImpl* impl;
};

void readFile(MidiFile& midi, std::istream& in);

void writeFile(const MidiFile& midi, std::ostream& out);

} // namespace midi

#endif // MIDI_H_