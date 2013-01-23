#ifndef MIDI_H_
#define MIDI_H_

#include <istream>
#include <vector>

enum midi_Instrument
{
	// Piano
	midi_Acoustic_Grand,
	midi_Bright_Acoustic,
	midi_Electric_Grand,
	midi_Honky_Tonk,
	midi_Electric_Piano_1,
	midi_Electric_Piano_2,
	midi_Harpsichord,
	midi_Clav,
	
	// Chrome Percussion
	midi_Celesta,
	midi_Glockenspiel,
	midi_Music_Box,
	midi_Vibraphone,
	midi_Marimba,
	midi_Xylophone,
	midi_Tubular_Bells,
	midi_Dulcimer,
	
	// Organ
	midi_Drawbar_Organ,
	midi_Percussive_Organ,
	midi_Rock_Organ,
	midi_Church_Organ,
	midi_Reed_Organ,
	midi_Accoridan,
	midi_Harmonica,
	midi_Tango_Accordian,
	
	// Guitar
	midi_Acoustic_Guitar_Nylon,
	midi_Acoustic_Guitar_Steel,
	midi_Electric_Guitar_Jazz,
	midi_Electric_Guitar_Clean,
	midi_Electric_Guitar_Muted,
	midi_Overdriven_Guitar,
	midi_Distortion_Guitar,
	midi_Guitar_Harmonics,
	
	// Bass
	midi_Acoustic_Bass,
	midi_Electric_Bassfinger,
	midi_Electric_Basspick,
	midi_Fretless_Bass,
	midi_Slap_Bass_1,
	midi_Slap_Bass_2,
	midi_Synth_Bass_1,
	midi_Synth_Bass_2,
	
	// Strings
	midi_Violin,
	midi_Viola,
	midi_Cello,
	midi_Contrabass,
	midi_Tremolo_Strings,
	midi_Pizzicato_Strings,
	midi_Orchestral_Strings,
	midi_Timpani,
	
	// Ensemble
	midi_String_Ensemble_1,
	midi_String_Ensemble_2,
	midi_SynthStrings_1,
	midi_SynthStrings_2,
	midi_Choir_Aahs,
	midi_Voice_Oohs,
	midi_Synth_Voice,
	midi_Orchestra_Hit,
	
	// Brass
	midi_Trumpet,
	midi_Trombone,
	midi_Tuba,
	midi_Muted_Trumpet,
	midi_French_Horn,
	midi_Brass_Section,
	midi_SynthBrass_1,
	midi_SynthBrass_2,
	
	// Reed
	midi_Soprano_Sax,
	midi_Alto_Sax,
	midi_Tenor_Sax,
	midi_Baritone_Sax,
	midi_Oboe,
	midi_English_Horn,
	midi_Bassoon,
	midi_Clarinet,
	
	// Pipe
	midi_Piccolo,
	midi_Flute,
	midi_Recorder,
	midi_Pan_Flute,
	midi_Blown_Bottle,
	midi_Skakuhachi,
	midi_Whistle,
	midi_Ocarina,
	
	// Synth Lead
	midi_Lead_1_Square,
	midi_Lead_2_Sawtooth,
	midi_Lead_3_Calliope,
	midi_Lead_4_Chiff,
	midi_Lead_5_Charang,
	midi_Lead_6_Voice,
	midi_Lead_7_Fifths,
	midi_Lead_8_Bass_Lead,
	
	// Synth Pad
	midi_Pad_1_New_Age,
	midi_Pad_2_Warm,
	midi_Pad_3_Polysynth,
	midi_Pad_4_Choir,
	midi_Pad_5_Bowed,
	midi_Pad_6_Metallic,
	midi_Pad_7_Halo,
	midi_Pad_8_Sweep,
	
	// Synth Effects
	midi_FX_1_Rain,
	midi_FX_2_Soundtrack,
	midi_FX_3_Crystal,
	midi_FX_4_Atmosphere,
	midi_FX_5_Brightness,
	midi_FX_6_Goblins,
	midi_FX_7_Echoes,
	midi_FX_8_Scifi,
	
	// Ethnic
	midi_Sitar,
	midi_Banjo,
	midi_Shamisen,
	midi_Koto,
	midi_Kalimba,
	midi_Bagpipe,
	midi_Fiddle,
	midi_Shanai,
	
	// Percussive
	midi_Tinkle_Bell,
	midi_Agogo,
	midi_Steel_Drums,
	midi_Woodblock,
	midi_Taiko_Drum,
	midi_Melodic_Tom,
	midi_Synth_Drum,
	midi_Reverse_Cymbal,
	
	// Sound Effects
	midi_Guitar_Fret_Noise,
	midi_Breath_Noise,
	midi_Seashore,
	midi_Bird_Tweet,
	midi_Telephone_Ring,
	midi_Helicopter,
	midi_Applause,
	midi_Gunshot
};

enum midi_Error
{
	midi_NoError,
	midi_ReadError,
	midi_WriteError,
	midi_InvalidHeader,
	midi_InvalidTrackHeader,
	midi_InvalidTrackLength,
	midi_InvalidEvent,
	midi_InvalidEventTimeDelta,
};

struct midi_Event
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

struct midi_Track 
{
	std::vector<midi_Event> events;
};

struct midi_Header 
{
	unsigned short format;
	unsigned short ticks;
};

struct midi_File
{	
	midi_Header header;
	std::vector<midi_Track> tracks;
};

midi_Error midi_readFile(midi_File& midi, std::istream& in);

midi_Error midi_writeFile(midi_File& midi, std::ostream& out);

midi_Error midi_play(midi_File& midi);

const char* midi_errorString(midi_Error error);

#endif // MIDI_H_