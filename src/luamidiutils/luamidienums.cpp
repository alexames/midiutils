#include "luamidiutils.hpp"
#include "midiutils.hpp"

#include "LuaWrapper.hpp"
#include "LuaWrapperUtil.hpp"
#include "lua.h"

using namespace midi;

int luamidiutils_pushCommandEnums(lua_State* L)
{
	lua_newtable(L);
	luaU_setenum(L, -1, "noteend", Event::NoteEnd);
	luaU_setenum(L, -1, "notebegin", Event::NoteBegin);
	luaU_setenum(L, -1, "velocitychange", Event::VelocityChange);
	luaU_setenum(L, -1, "controllerchange", Event::ControllerChange);
	luaU_setenum(L, -1, "programchange", Event::ProgramChange);
	luaU_setenum(L, -1, "channelpressurechange", Event::ChannelPressureChange);
	luaU_setenum(L, -1, "pitchwheelchange", Event::PitchWheelChange);
	luaU_setenum(L, -1, "meta", Event::Meta);
    return 1;
}

int luamidiutils_pushInstrumentEnums(lua_State* L)
{
	lua_newtable(L);
	luaU_setenum(L, -1, "acousticgrand", Acoustic_Grand);
	luaU_setenum(L, -1, "brightacoustic", Bright_Acoustic);
	luaU_setenum(L, -1, "electricgrand", Electric_Grand);
	luaU_setenum(L, -1, "honkytonk", Honky_Tonk);
	luaU_setenum(L, -1, "electricpiano_1", Electric_Piano_1);
	luaU_setenum(L, -1, "electricpiano_2", Electric_Piano_2);
	luaU_setenum(L, -1, "harpsichord", Harpsichord);
	luaU_setenum(L, -1, "clav", Clav);
	luaU_setenum(L, -1, "celesta", Celesta);
	luaU_setenum(L, -1, "glockenspiel", Glockenspiel);
	luaU_setenum(L, -1, "musicbox", Music_Box);
	luaU_setenum(L, -1, "vibraphone", Vibraphone);
	luaU_setenum(L, -1, "marimba", Marimba);
	luaU_setenum(L, -1, "xylophone", Xylophone);
	luaU_setenum(L, -1, "tubularbells", Tubular_Bells);
	luaU_setenum(L, -1, "dulcimer", Dulcimer);
	luaU_setenum(L, -1, "drawbarorgan", Drawbar_Organ);
	luaU_setenum(L, -1, "percussiveorgan", Percussive_Organ);
	luaU_setenum(L, -1, "rockorgan", Rock_Organ);
	luaU_setenum(L, -1, "churchorgan", Church_Organ);
	luaU_setenum(L, -1, "reedorgan", Reed_Organ);
	luaU_setenum(L, -1, "accoridan", Accoridan);
	luaU_setenum(L, -1, "harmonica", Harmonica);
	luaU_setenum(L, -1, "tangoaccordian", Tango_Accordian);
	luaU_setenum(L, -1, "acousticguitarnylon", Acoustic_Guitar_Nylon);
	luaU_setenum(L, -1, "acousticguitarsteel", Acoustic_Guitar_Steel);
	luaU_setenum(L, -1, "electricguitarjazz", Electric_Guitar_Jazz);
	luaU_setenum(L, -1, "electricguitarclean", Electric_Guitar_Clean);
	luaU_setenum(L, -1, "electricguitarmuted", Electric_Guitar_Muted);
	luaU_setenum(L, -1, "overdrivenguitar", Overdriven_Guitar);
	luaU_setenum(L, -1, "distortionguitar", Distortion_Guitar);
	luaU_setenum(L, -1, "guitarharmonics", Guitar_Harmonics);
	luaU_setenum(L, -1, "acousticbass", Acoustic_Bass);
	luaU_setenum(L, -1, "electricbassfinger", Electric_Bassfinger);
	luaU_setenum(L, -1, "electricbasspick", Electric_Basspick);
	luaU_setenum(L, -1, "fretlessbass", Fretless_Bass);
	luaU_setenum(L, -1, "slapbass1", Slap_Bass_1);
	luaU_setenum(L, -1, "slapbass2", Slap_Bass_2);
	luaU_setenum(L, -1, "synthbass1", Synth_Bass_1);
	luaU_setenum(L, -1, "synthbass2", Synth_Bass_2);
	luaU_setenum(L, -1, "violin", Violin);
	luaU_setenum(L, -1, "viola", Viola);
	luaU_setenum(L, -1, "cello", Cello);
	luaU_setenum(L, -1, "contrabass", Contrabass);
	luaU_setenum(L, -1, "tremolostrings", Tremolo_Strings);
	luaU_setenum(L, -1, "pizzicatostrings", Pizzicato_Strings);
	luaU_setenum(L, -1, "orchestralstrings", Orchestral_Strings);
	luaU_setenum(L, -1, "timpani", Timpani);
	luaU_setenum(L, -1, "stringensemble1", String_Ensemble_1);
	luaU_setenum(L, -1, "stringensemble2", String_Ensemble_2);
	luaU_setenum(L, -1, "synthstrings1", SynthStrings_1);
	luaU_setenum(L, -1, "synthstrings2", SynthStrings_2);
	luaU_setenum(L, -1, "choiraahs", Choir_Aahs);
	luaU_setenum(L, -1, "voiceoohs", Voice_Oohs);
	luaU_setenum(L, -1, "synthvoice", Synth_Voice);
	luaU_setenum(L, -1, "orchestrahit", Orchestra_Hit);
	luaU_setenum(L, -1, "trumpet", Trumpet);
	luaU_setenum(L, -1, "trombone", Trombone);
	luaU_setenum(L, -1, "tuba", Tuba);
	luaU_setenum(L, -1, "mutedtrumpet", Muted_Trumpet);
	luaU_setenum(L, -1, "frenchhorn", French_Horn);
	luaU_setenum(L, -1, "brasssection", Brass_Section);
	luaU_setenum(L, -1, "synthbrass1", SynthBrass_1);
	luaU_setenum(L, -1, "synthbrass2", SynthBrass_2);
	luaU_setenum(L, -1, "sopranosax", Soprano_Sax);
	luaU_setenum(L, -1, "altosax", Alto_Sax);
	luaU_setenum(L, -1, "tenorsax", Tenor_Sax);
	luaU_setenum(L, -1, "baritonesax", Baritone_Sax);
	luaU_setenum(L, -1, "oboe", Oboe);
	luaU_setenum(L, -1, "englishhorn", English_Horn);
	luaU_setenum(L, -1, "bassoon", Bassoon);
	luaU_setenum(L, -1, "clarinet", Clarinet);
	luaU_setenum(L, -1, "piccolo", Piccolo);
	luaU_setenum(L, -1, "flute", Flute);
	luaU_setenum(L, -1, "recorder", Recorder);
	luaU_setenum(L, -1, "panflute", Pan_Flute);
	luaU_setenum(L, -1, "blownbottle", Blown_Bottle);
	luaU_setenum(L, -1, "skakuhachi", Skakuhachi);
	luaU_setenum(L, -1, "whistle", Whistle);
	luaU_setenum(L, -1, "ocarina", Ocarina);
	luaU_setenum(L, -1, "lead1square", Lead_1_Square);
	luaU_setenum(L, -1, "lead2sawtooth", Lead_2_Sawtooth);
	luaU_setenum(L, -1, "lead3calliope", Lead_3_Calliope);
	luaU_setenum(L, -1, "lead4chiff", Lead_4_Chiff);
	luaU_setenum(L, -1, "lead5charang", Lead_5_Charang);
	luaU_setenum(L, -1, "lead6voice", Lead_6_Voice);
	luaU_setenum(L, -1, "lead7fifths", Lead_7_Fifths);
	luaU_setenum(L, -1, "lead8basslead", Lead_8_Bass_Lead);
	luaU_setenum(L, -1, "pad1new_age", Pad_1_New_Age);
	luaU_setenum(L, -1, "pad2warm", Pad_2_Warm);
	luaU_setenum(L, -1, "pad3polysynth", Pad_3_Polysynth);
	luaU_setenum(L, -1, "pad4choir", Pad_4_Choir);
	luaU_setenum(L, -1, "pad5bowed", Pad_5_Bowed);
	luaU_setenum(L, -1, "pad6metallic", Pad_6_Metallic);
	luaU_setenum(L, -1, "pad7halo", Pad_7_Halo);
	luaU_setenum(L, -1, "pad8sweep", Pad_8_Sweep);
	luaU_setenum(L, -1, "fx1rain", FX_1_Rain);
	luaU_setenum(L, -1, "fx2soundtrack", FX_2_Soundtrack);
	luaU_setenum(L, -1, "fx3crystal", FX_3_Crystal);
	luaU_setenum(L, -1, "fx4atmosphere", FX_4_Atmosphere);
	luaU_setenum(L, -1, "fx5brightness", FX_5_Brightness);
	luaU_setenum(L, -1, "fx6goblins", FX_6_Goblins);
	luaU_setenum(L, -1, "fx7echoes", FX_7_Echoes);
	luaU_setenum(L, -1, "fx8scifi", FX_8_Scifi);
	luaU_setenum(L, -1, "sitar", Sitar);
	luaU_setenum(L, -1, "banjo", Banjo);
	luaU_setenum(L, -1, "shamisen", Shamisen);
	luaU_setenum(L, -1, "koto", Koto);
	luaU_setenum(L, -1, "kalimba", Kalimba);
	luaU_setenum(L, -1, "bagpipe", Bagpipe);
	luaU_setenum(L, -1, "fiddle", Fiddle);
	luaU_setenum(L, -1, "shanai", Shanai);
	luaU_setenum(L, -1, "tinklebell", Tinkle_Bell);
	luaU_setenum(L, -1, "agogo", Agogo);
	luaU_setenum(L, -1, "steeldrums", Steel_Drums);
	luaU_setenum(L, -1, "woodblock", Woodblock);
	luaU_setenum(L, -1, "taikodrum", Taiko_Drum);
	luaU_setenum(L, -1, "melodictom", Melodic_Tom);
	luaU_setenum(L, -1, "synthdrum", Synth_Drum);
	luaU_setenum(L, -1, "reversecymbal", Reverse_Cymbal);
	luaU_setenum(L, -1, "guitarfret_noise", Guitar_Fret_Noise);
	luaU_setenum(L, -1, "breathnoise", Breath_Noise);
	luaU_setenum(L, -1, "seashore", Seashore);
	luaU_setenum(L, -1, "birdtweet", Bird_Tweet);
	luaU_setenum(L, -1, "telephonering", Telephone_Ring);
	luaU_setenum(L, -1, "helicopter", Helicopter);
	luaU_setenum(L, -1, "applause", Applause);
	luaU_setenum(L, -1, "gunshot", Gunshot);
    return 1;
}