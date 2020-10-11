#ifndef MIDI_H_
#define MIDI_H_

#include <istream>
#include <optional>
#include <vector>

namespace midi {

enum class Format {
  SingleTrack,
  MultiTrackSynchronous,
  MultiTrackAsynchronous
};

enum class Instrument {
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

//
// A midi event represents one of many commands a midi file can run. The Event
// structure is a union of all possible events.
//
// Only regular events (i.e. not Meta events) are significant to the midi file
// playback
//
struct Event {
  struct NoteEndEvent {
    uint8_t noteNumber;
    uint8_t velocity;
  };

  struct NoteBeginEvent {
    uint8_t noteNumber;
    uint8_t velocity;
  };

  struct VelocityChangeEvent {
    uint8_t noteNumber;
    uint8_t velocity;
  };

  struct ControllerChangeEvent {
    uint8_t controllerNumber;
    uint8_t velocity;
  };

  struct ProgramChangeEvent {
    uint8_t newProgramNumber;
  };

  struct ChannelPressureChangeEvent {
    uint8_t channelNumber;
  };

  struct PitchWheelChangeEvent {
    uint8_t bottom;
    uint8_t top;
  };

  enum class MetaCommand {
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

  struct MetaEvent {
    struct SetSequenceNumberEvent {
      uint32_t sequenceNumber;
    };
    struct TextEvent {
      uint32_t TODO;
    };
    struct CopywriteEvent {
      uint32_t TODO;
    };
    struct SequnceNameEvent {
      uint32_t TODO;
    };
    struct TrackInstrumentNameEvent {
      uint32_t TODO;
    };
    struct LyricEvent {
      uint32_t TODO;
    };
    struct MarkerEvent {
      uint32_t TODO;
    };
    struct CueEvent {
      uint32_t TODO;
    };
    struct PrefixAssignmentEvent {
      uint32_t TODO;
    };
    struct EndOfTrackEvent {
      uint32_t TODO;
    };
    struct SetTempoEvent {
      uint32_t TODO;
    };
    struct SMPTEOffsetEvent {
      uint32_t TODO;
    };
    struct TimeSignatureEvent {
      uint32_t TODO;
    };
    struct KeySignatureEvent {
      uint32_t TODO;
    };
    struct SequencerSpecificEvent {
      uint32_t TODO;
    };

    MetaCommand command;
    uint8_t length;
    uint8_t* data;
    union {
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

  enum class Command {
    NoteEnd = 0x80,
    NoteBegin = 0x90,
    VelocityChange = 0xA0,
    ControllerChange = 0xB0,
    ProgramChange = 0xC0,
    ChannelPressureChange = 0xD0,
    PitchWheelChange = 0xE0,
    Meta = 0xF0
  };

  uint32_t timeDelta;
  Command command;
  uint8_t channel;
  union {
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

struct Track {
  std::vector<Event> events;
};

//
// A structure representing a Midi file. A midi file consists of a format, the
// number of ticks per beat, and a list of tracks filled with midi events.
//
struct MidiFile {
  Format format;
  uint16_t ticks;
  std::vector<Track> tracks;
};

//
// An EventProducer is an abstract base class, used by a MidiStream to produce
// midi events to be played.
//
class EventProducer {
 public:
  virtual ~EventProducer() {};

  // A function that runs immediately prior to filling a buffer with events
  // to be played. Since this is not useful in all cases it's a no-op by
  // default.
  virtual void preBufferFill() {}

  // Returns the next event to be played. This function may be called
  // repeatedly by a MidiStream until its buffer reaches max capacity, or
  // until a null pointer is returned. When a null pointer is encountered,
  // the buffered events are played and getNextEvent is not called again
  // until the buffered notes have been played.
  virtual const std::optional<midi::Event> getNextEvent(
      uint32_t& absoluteTime) = 0;

  // Returns the ticks per beat.
  virtual uint32_t getTicksPerBeat() = 0;
};

//
// Produces the midi events from a given midi file for playback in a MidiStream
//
class MidiFileEventProducer : public EventProducer {
 public:
  MidiFileEventProducer(MidiFile& midi);
  ~MidiFileEventProducer() override;

  const std::optional<Event> getNextEvent(uint32_t& absoluteTime) override;
  uint32_t getTicksPerBeat() override;

 private:
  MidiFile* m_midi;
  uint32_t m_lastEventTime;
  std::vector<uint32_t> m_indicies;
  std::vector<uint32_t> m_absoluteTimes;
};

struct MidiStreamImpl;

//
// A MidiStream provides functionality to play the midi events produced by a
// given EventProducer source - typically a MidiFile, but it could also be from
// a script or other procedural source.
//
class MidiStream {
 public:
  MidiStream(EventProducer& producer);
  ~MidiStream();

  // Begins playback of the stream.
  void play();

  // Returns the current position of the stream in milliseconds
  uint32_t getMilliseconds() const;

  // Returns the current position of the stream in ticks
  uint32_t getTicks() const;

 private:
  MidiStreamImpl* impl;
};

//
// Reads in bytes from `in` to populate the given MidiFile structure. Some files
// have improperly formatted headers, but can nevertheless be read properly.
// Turning on the strict flag will reject files with those incorrectly formatted
// files.
//
void readFile(MidiFile& midi, std::istream& in, bool strict = false);

//
// Writes the given MidiFile to out. The result is a standard midi file that can
// be read by any normal midi player program.
//
void writeFile(const MidiFile& midi, std::ostream& out);

}  // namespace midi

#endif  // MIDI_H_