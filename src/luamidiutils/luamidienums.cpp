#include "LuaWrapper.hpp"
#include "LuaWrapperUtil.hpp"
#include "lua.h"
#include "luamidiutils.hpp"
#include "midiutils.hpp"

using namespace midi;

int luamidiutils_pushCommandEnums(lua_State* L) {
  lua_newtable(L);
  luaU_setfield(L, -1, "noteend", static_cast<int>(Event::Command::NoteEnd));
  luaU_setfield(L, -1, "notebegin", static_cast<int>(Event::Command::NoteBegin));
  luaU_setfield(L, -1, "velocitychange",
                static_cast<int>(Event::Command::VelocityChange));
  luaU_setfield(L, -1, "controllerchange",
                static_cast<int>(Event::Command::ControllerChange));
  luaU_setfield(L, -1, "programchange", static_cast<int>(Event::Command::ProgramChange));
  luaU_setfield(L, -1, "channelpressurechange",
                static_cast<int>(Event::Command::ChannelPressureChange));
  luaU_setfield(L, -1, "pitchwheelchange",
                static_cast<int>(Event::Command::PitchWheelChange));
  luaU_setfield(L, -1, "meta", static_cast<int>(Event::Command::Meta));
  return 1;
}

int luamidiutils_pushInstrumentEnums(lua_State* L) {
  lua_newtable(L);
  luaU_setfield(L, -1, "acousticgrand",
                static_cast<int>(Instrument::Acoustic_Grand));
  luaU_setfield(L, -1, "brightacoustic",
                static_cast<int>(Instrument::Bright_Acoustic));
  luaU_setfield(L, -1, "electricgrand",
                static_cast<int>(Instrument::Electric_Grand));
  luaU_setfield(L, -1, "honkytonk", static_cast<int>(Instrument::Honky_Tonk));
  luaU_setfield(L, -1, "electricpiano_1",
                static_cast<int>(Instrument::Electric_Piano_1));
  luaU_setfield(L, -1, "electricpiano_2",
                static_cast<int>(Instrument::Electric_Piano_2));
  luaU_setfield(L, -1, "harpsichord",
                static_cast<int>(Instrument::Harpsichord));
  luaU_setfield(L, -1, "clav", static_cast<int>(Instrument::Clav));
  luaU_setfield(L, -1, "celesta", static_cast<int>(Instrument::Celesta));
  luaU_setfield(L, -1, "glockenspiel",
                static_cast<int>(Instrument::Glockenspiel));
  luaU_setfield(L, -1, "musicbox", static_cast<int>(Instrument::Music_Box));
  luaU_setfield(L, -1, "vibraphone", static_cast<int>(Instrument::Vibraphone));
  luaU_setfield(L, -1, "marimba", static_cast<int>(Instrument::Marimba));
  luaU_setfield(L, -1, "xylophone", static_cast<int>(Instrument::Xylophone));
  luaU_setfield(L, -1, "tubularbells",
                static_cast<int>(Instrument::Tubular_Bells));
  luaU_setfield(L, -1, "dulcimer", static_cast<int>(Instrument::Dulcimer));
  luaU_setfield(L, -1, "drawbarorgan",
                static_cast<int>(Instrument::Drawbar_Organ));
  luaU_setfield(L, -1, "percussiveorgan",
                static_cast<int>(Instrument::Percussive_Organ));
  luaU_setfield(L, -1, "rockorgan", static_cast<int>(Instrument::Rock_Organ));
  luaU_setfield(L, -1, "churchorgan",
                static_cast<int>(Instrument::Church_Organ));
  luaU_setfield(L, -1, "reedorgan", static_cast<int>(Instrument::Reed_Organ));
  luaU_setfield(L, -1, "accoridan", static_cast<int>(Instrument::Accoridan));
  luaU_setfield(L, -1, "harmonica", static_cast<int>(Instrument::Harmonica));
  luaU_setfield(L, -1, "tangoaccordian",
                static_cast<int>(Instrument::Tango_Accordian));
  luaU_setfield(L, -1, "acousticguitarnylon",
                static_cast<int>(Instrument::Acoustic_Guitar_Nylon));
  luaU_setfield(L, -1, "acousticguitarsteel",
                static_cast<int>(Instrument::Acoustic_Guitar_Steel));
  luaU_setfield(L, -1, "electricguitarjazz",
                static_cast<int>(Instrument::Electric_Guitar_Jazz));
  luaU_setfield(L, -1, "electricguitarclean",
                static_cast<int>(Instrument::Electric_Guitar_Clean));
  luaU_setfield(L, -1, "electricguitarmuted",
                static_cast<int>(Instrument::Electric_Guitar_Muted));
  luaU_setfield(L, -1, "overdrivenguitar",
                static_cast<int>(Instrument::Overdriven_Guitar));
  luaU_setfield(L, -1, "distortionguitar",
                static_cast<int>(Instrument::Distortion_Guitar));
  luaU_setfield(L, -1, "guitarharmonics",
                static_cast<int>(Instrument::Guitar_Harmonics));
  luaU_setfield(L, -1, "acousticbass",
                static_cast<int>(Instrument::Acoustic_Bass));
  luaU_setfield(L, -1, "electricbassfinger",
                static_cast<int>(Instrument::Electric_Bassfinger));
  luaU_setfield(L, -1, "electricbasspick",
                static_cast<int>(Instrument::Electric_Basspick));
  luaU_setfield(L, -1, "fretlessbass",
                static_cast<int>(Instrument::Fretless_Bass));
  luaU_setfield(L, -1, "slapbass1", static_cast<int>(Instrument::Slap_Bass_1));
  luaU_setfield(L, -1, "slapbass2", static_cast<int>(Instrument::Slap_Bass_2));
  luaU_setfield(L, -1, "synthbass1",
                static_cast<int>(Instrument::Synth_Bass_1));
  luaU_setfield(L, -1, "synthbass2",
                static_cast<int>(Instrument::Synth_Bass_2));
  luaU_setfield(L, -1, "violin", static_cast<int>(Instrument::Violin));
  luaU_setfield(L, -1, "viola", static_cast<int>(Instrument::Viola));
  luaU_setfield(L, -1, "cello", static_cast<int>(Instrument::Cello));
  luaU_setfield(L, -1, "contrabass", static_cast<int>(Instrument::Contrabass));
  luaU_setfield(L, -1, "tremolostrings",
                static_cast<int>(Instrument::Tremolo_Strings));
  luaU_setfield(L, -1, "pizzicatostrings",
                static_cast<int>(Instrument::Pizzicato_Strings));
  luaU_setfield(L, -1, "orchestralstrings",
                static_cast<int>(Instrument::Orchestral_Strings));
  luaU_setfield(L, -1, "timpani", static_cast<int>(Instrument::Timpani));
  luaU_setfield(L, -1, "stringensemble1",
                static_cast<int>(Instrument::String_Ensemble_1));
  luaU_setfield(L, -1, "stringensemble2",
                static_cast<int>(Instrument::String_Ensemble_2));
  luaU_setfield(L, -1, "synthstrings1",
                static_cast<int>(Instrument::SynthStrings_1));
  luaU_setfield(L, -1, "synthstrings2",
                static_cast<int>(Instrument::SynthStrings_2));
  luaU_setfield(L, -1, "choiraahs", static_cast<int>(Instrument::Choir_Aahs));
  luaU_setfield(L, -1, "voiceoohs", static_cast<int>(Instrument::Voice_Oohs));
  luaU_setfield(L, -1, "synthvoice", static_cast<int>(Instrument::Synth_Voice));
  luaU_setfield(L, -1, "orchestrahit",
                static_cast<int>(Instrument::Orchestra_Hit));
  luaU_setfield(L, -1, "trumpet", static_cast<int>(Instrument::Trumpet));
  luaU_setfield(L, -1, "trombone", static_cast<int>(Instrument::Trombone));
  luaU_setfield(L, -1, "tuba", static_cast<int>(Instrument::Tuba));
  luaU_setfield(L, -1, "mutedtrumpet",
                static_cast<int>(Instrument::Muted_Trumpet));
  luaU_setfield(L, -1, "frenchhorn", static_cast<int>(Instrument::French_Horn));
  luaU_setfield(L, -1, "brasssection",
                static_cast<int>(Instrument::Brass_Section));
  luaU_setfield(L, -1, "synthbrass1",
                static_cast<int>(Instrument::SynthBrass_1));
  luaU_setfield(L, -1, "synthbrass2",
                static_cast<int>(Instrument::SynthBrass_2));
  luaU_setfield(L, -1, "sopranosax", static_cast<int>(Instrument::Soprano_Sax));
  luaU_setfield(L, -1, "altosax", static_cast<int>(Instrument::Alto_Sax));
  luaU_setfield(L, -1, "tenorsax", static_cast<int>(Instrument::Tenor_Sax));
  luaU_setfield(L, -1, "baritonesax",
                static_cast<int>(Instrument::Baritone_Sax));
  luaU_setfield(L, -1, "oboe", static_cast<int>(Instrument::Oboe));
  luaU_setfield(L, -1, "englishhorn",
                static_cast<int>(Instrument::English_Horn));
  luaU_setfield(L, -1, "bassoon", static_cast<int>(Instrument::Bassoon));
  luaU_setfield(L, -1, "clarinet", static_cast<int>(Instrument::Clarinet));
  luaU_setfield(L, -1, "piccolo", static_cast<int>(Instrument::Piccolo));
  luaU_setfield(L, -1, "flute", static_cast<int>(Instrument::Flute));
  luaU_setfield(L, -1, "recorder", static_cast<int>(Instrument::Recorder));
  luaU_setfield(L, -1, "panflute", static_cast<int>(Instrument::Pan_Flute));
  luaU_setfield(L, -1, "blownbottle",
                static_cast<int>(Instrument::Blown_Bottle));
  luaU_setfield(L, -1, "skakuhachi", static_cast<int>(Instrument::Skakuhachi));
  luaU_setfield(L, -1, "whistle", static_cast<int>(Instrument::Whistle));
  luaU_setfield(L, -1, "ocarina", static_cast<int>(Instrument::Ocarina));
  luaU_setfield(L, -1, "lead1square",
                static_cast<int>(Instrument::Lead_1_Square));
  luaU_setfield(L, -1, "lead2sawtooth",
                static_cast<int>(Instrument::Lead_2_Sawtooth));
  luaU_setfield(L, -1, "lead3calliope",
                static_cast<int>(Instrument::Lead_3_Calliope));
  luaU_setfield(L, -1, "lead4chiff",
                static_cast<int>(Instrument::Lead_4_Chiff));
  luaU_setfield(L, -1, "lead5charang",
                static_cast<int>(Instrument::Lead_5_Charang));
  luaU_setfield(L, -1, "lead6voice",
                static_cast<int>(Instrument::Lead_6_Voice));
  luaU_setfield(L, -1, "lead7fifths",
                static_cast<int>(Instrument::Lead_7_Fifths));
  luaU_setfield(L, -1, "lead8basslead",
                static_cast<int>(Instrument::Lead_8_Bass_Lead));
  luaU_setfield(L, -1, "pad1new_age",
                static_cast<int>(Instrument::Pad_1_New_Age));
  luaU_setfield(L, -1, "pad2warm", static_cast<int>(Instrument::Pad_2_Warm));
  luaU_setfield(L, -1, "pad3polysynth",
                static_cast<int>(Instrument::Pad_3_Polysynth));
  luaU_setfield(L, -1, "pad4choir", static_cast<int>(Instrument::Pad_4_Choir));
  luaU_setfield(L, -1, "pad5bowed", static_cast<int>(Instrument::Pad_5_Bowed));
  luaU_setfield(L, -1, "pad6metallic",
                static_cast<int>(Instrument::Pad_6_Metallic));
  luaU_setfield(L, -1, "pad7halo", static_cast<int>(Instrument::Pad_7_Halo));
  luaU_setfield(L, -1, "pad8sweep", static_cast<int>(Instrument::Pad_8_Sweep));
  luaU_setfield(L, -1, "fx1rain", static_cast<int>(Instrument::FX_1_Rain));
  luaU_setfield(L, -1, "fx2soundtrack",
                static_cast<int>(Instrument::FX_2_Soundtrack));
  luaU_setfield(L, -1, "fx3crystal",
                static_cast<int>(Instrument::FX_3_Crystal));
  luaU_setfield(L, -1, "fx4atmosphere",
                static_cast<int>(Instrument::FX_4_Atmosphere));
  luaU_setfield(L, -1, "fx5brightness",
                static_cast<int>(Instrument::FX_5_Brightness));
  luaU_setfield(L, -1, "fx6goblins",
                static_cast<int>(Instrument::FX_6_Goblins));
  luaU_setfield(L, -1, "fx7echoes", static_cast<int>(Instrument::FX_7_Echoes));
  luaU_setfield(L, -1, "fx8scifi", static_cast<int>(Instrument::FX_8_Scifi));
  luaU_setfield(L, -1, "sitar", static_cast<int>(Instrument::Sitar));
  luaU_setfield(L, -1, "banjo", static_cast<int>(Instrument::Banjo));
  luaU_setfield(L, -1, "shamisen", static_cast<int>(Instrument::Shamisen));
  luaU_setfield(L, -1, "koto", static_cast<int>(Instrument::Koto));
  luaU_setfield(L, -1, "kalimba", static_cast<int>(Instrument::Kalimba));
  luaU_setfield(L, -1, "bagpipe", static_cast<int>(Instrument::Bagpipe));
  luaU_setfield(L, -1, "fiddle", static_cast<int>(Instrument::Fiddle));
  luaU_setfield(L, -1, "shanai", static_cast<int>(Instrument::Shanai));
  luaU_setfield(L, -1, "tinklebell", static_cast<int>(Instrument::Tinkle_Bell));
  luaU_setfield(L, -1, "agogo", static_cast<int>(Instrument::Agogo));
  luaU_setfield(L, -1, "steeldrums", static_cast<int>(Instrument::Steel_Drums));
  luaU_setfield(L, -1, "woodblock", static_cast<int>(Instrument::Woodblock));
  luaU_setfield(L, -1, "taikodrum", static_cast<int>(Instrument::Taiko_Drum));
  luaU_setfield(L, -1, "melodictom", static_cast<int>(Instrument::Melodic_Tom));
  luaU_setfield(L, -1, "synthdrum", static_cast<int>(Instrument::Synth_Drum));
  luaU_setfield(L, -1, "reversecymbal",
                static_cast<int>(Instrument::Reverse_Cymbal));
  luaU_setfield(L, -1, "guitarfret_noise",
                static_cast<int>(Instrument::Guitar_Fret_Noise));
  luaU_setfield(L, -1, "breathnoise",
                static_cast<int>(Instrument::Breath_Noise));
  luaU_setfield(L, -1, "seashore", static_cast<int>(Instrument::Seashore));
  luaU_setfield(L, -1, "birdtweet", static_cast<int>(Instrument::Bird_Tweet));
  luaU_setfield(L, -1, "telephonering",
                static_cast<int>(Instrument::Telephone_Ring));
  luaU_setfield(L, -1, "helicopter", static_cast<int>(Instrument::Helicopter));
  luaU_setfield(L, -1, "applause", static_cast<int>(Instrument::Applause));
  luaU_setfield(L, -1, "gunshot", static_cast<int>(Instrument::Gunshot));
  return 1;
}