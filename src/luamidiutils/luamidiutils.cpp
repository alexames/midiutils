#include "midiutils.hpp"

#include "LuaWrapper.hpp"
#include "LuaWrapperUtil.hpp"

#include "lauxlib.h"
#include "lua.h"

#include <fstream>
#include <exception>

#define MIDIUTILS_EXPORT __declspec(dllexport)

using namespace std;

namespace midi
{

static int midifileutils_readFile(lua_State* L)
{
	MidiFile* midi = luaW_check<MidiFile>(L, 1);
	const char* filename = luaL_checkstring(L, 2);
	bool strict = lua_toboolean(L, 3);
	ifstream file(filename, ios::binary);
	try
	{
		readFile(*midi, file, strict);
	}
	catch (exception& ex)
	{
		lua_pushstring(L, ex.what());
		return 1;
	}
	return 0;
}

static int midifileutils_writeFile(lua_State* L)
{
	MidiFile* midi = luaW_check<MidiFile>(L, 1);
	const char* filename = luaL_checkstring(L, 2);
	ofstream file(filename, ios::binary);
	try
	{
		writeFile(*midi, file);
	}
	catch (exception& ex)
	{
		lua_pushstring(L, ex.what());
		return 1;
	}
	return 0;
}

static luaL_Reg luamidiutils_table[] =
{
	{ "readfile", midifileutils_readFile },
	{ "writefile", midifileutils_writeFile },
	{ NULL, NULL }
};

} // namespace midi

extern "C"
{

MIDIUTILS_EXPORT int luaopen_luamidiutils(lua_State* L)
{
	luaL_register(L, "luamidiutils", midi::luamidiutils_table);
	luaW_register<midi::MidiFile>(L, "MidiFile", NULL, NULL);
	lua_setfield(L, -2, "MidiFile");

	lua_newtable(L);
	luaU_setenum(L, -1, "acousticgrand", midi::Acoustic_Grand);
	luaU_setenum(L, -1, "brightacoustic", midi::Bright_Acoustic);
	luaU_setenum(L, -1, "electricgrand", midi::Electric_Grand);
	luaU_setenum(L, -1, "honkytonk", midi::Honky_Tonk);
	luaU_setenum(L, -1, "electricpiano_1", midi::Electric_Piano_1);
	luaU_setenum(L, -1, "electricpiano_2", midi::Electric_Piano_2);
	luaU_setenum(L, -1, "harpsichord", midi::Harpsichord);
	luaU_setenum(L, -1, "clav", midi::Clav);
	luaU_setenum(L, -1, "celesta", midi::Celesta);
	luaU_setenum(L, -1, "glockenspiel", midi::Glockenspiel);
	luaU_setenum(L, -1, "musicbox", midi::Music_Box);
	luaU_setenum(L, -1, "vibraphone", midi::Vibraphone);
	luaU_setenum(L, -1, "marimba", midi::Marimba);
	luaU_setenum(L, -1, "xylophone", midi::Xylophone);
	luaU_setenum(L, -1, "tubularbells", midi::Tubular_Bells);
	luaU_setenum(L, -1, "dulcimer", midi::Dulcimer);
	luaU_setenum(L, -1, "drawbarorgan", midi::Drawbar_Organ);
	luaU_setenum(L, -1, "percussiveorgan", midi::Percussive_Organ);
	luaU_setenum(L, -1, "rockorgan", midi::Rock_Organ);
	luaU_setenum(L, -1, "churchorgan", midi::Church_Organ);
	luaU_setenum(L, -1, "reedorgan", midi::Reed_Organ);
	luaU_setenum(L, -1, "accoridan", midi::Accoridan);
	luaU_setenum(L, -1, "harmonica", midi::Harmonica);
	luaU_setenum(L, -1, "tangoaccordian", midi::Tango_Accordian);
	luaU_setenum(L, -1, "acousticguitarnylon", midi::Acoustic_Guitar_Nylon);
	luaU_setenum(L, -1, "acousticguitarsteel", midi::Acoustic_Guitar_Steel);
	luaU_setenum(L, -1, "electricguitarjazz", midi::Electric_Guitar_Jazz);
	luaU_setenum(L, -1, "electricguitarclean", midi::Electric_Guitar_Clean);
	luaU_setenum(L, -1, "electricguitarmuted", midi::Electric_Guitar_Muted);
	luaU_setenum(L, -1, "overdrivenguitar", midi::Overdriven_Guitar);
	luaU_setenum(L, -1, "distortionguitar", midi::Distortion_Guitar);
	luaU_setenum(L, -1, "guitarharmonics", midi::Guitar_Harmonics);
	luaU_setenum(L, -1, "acousticbass", midi::Acoustic_Bass);
	luaU_setenum(L, -1, "electricbassfinger", midi::Electric_Bassfinger);
	luaU_setenum(L, -1, "electricbasspick", midi::Electric_Basspick);
	luaU_setenum(L, -1, "fretlessbass", midi::Fretless_Bass);
	luaU_setenum(L, -1, "slapbass1", midi::Slap_Bass_1);
	luaU_setenum(L, -1, "slapbass2", midi::Slap_Bass_2);
	luaU_setenum(L, -1, "synthbass1", midi::Synth_Bass_1);
	luaU_setenum(L, -1, "synthbass2", midi::Synth_Bass_2);
	luaU_setenum(L, -1, "violin", midi::Violin);
	luaU_setenum(L, -1, "viola", midi::Viola);
	luaU_setenum(L, -1, "cello", midi::Cello);
	luaU_setenum(L, -1, "contrabass", midi::Contrabass);
	luaU_setenum(L, -1, "tremolostrings", midi::Tremolo_Strings);
	luaU_setenum(L, -1, "pizzicatostrings", midi::Pizzicato_Strings);
	luaU_setenum(L, -1, "orchestralstrings", midi::Orchestral_Strings);
	luaU_setenum(L, -1, "timpani", midi::Timpani);
	luaU_setenum(L, -1, "stringensemble1", midi::String_Ensemble_1);
	luaU_setenum(L, -1, "stringensemble2", midi::String_Ensemble_2);
	luaU_setenum(L, -1, "synthstrings1", midi::SynthStrings_1);
	luaU_setenum(L, -1, "synthstrings2", midi::SynthStrings_2);
	luaU_setenum(L, -1, "choiraahs", midi::Choir_Aahs);
	luaU_setenum(L, -1, "voiceoohs", midi::Voice_Oohs);
	luaU_setenum(L, -1, "synthvoice", midi::Synth_Voice);
	luaU_setenum(L, -1, "orchestrahit", midi::Orchestra_Hit);
	luaU_setenum(L, -1, "trumpet", midi::Trumpet);
	luaU_setenum(L, -1, "trombone", midi::Trombone);
	luaU_setenum(L, -1, "tuba", midi::Tuba);
	luaU_setenum(L, -1, "mutedtrumpet", midi::Muted_Trumpet);
	luaU_setenum(L, -1, "frenchhorn", midi::French_Horn);
	luaU_setenum(L, -1, "brasssection", midi::Brass_Section);
	luaU_setenum(L, -1, "synthbrass1", midi::SynthBrass_1);
	luaU_setenum(L, -1, "synthbrass2", midi::SynthBrass_2);
	luaU_setenum(L, -1, "sopranosax", midi::Soprano_Sax);
	luaU_setenum(L, -1, "altosax", midi::Alto_Sax);
	luaU_setenum(L, -1, "tenorsax", midi::Tenor_Sax);
	luaU_setenum(L, -1, "baritonesax", midi::Baritone_Sax);
	luaU_setenum(L, -1, "oboe", midi::Oboe);
	luaU_setenum(L, -1, "englishhorn", midi::English_Horn);
	luaU_setenum(L, -1, "bassoon", midi::Bassoon);
	luaU_setenum(L, -1, "clarinet", midi::Clarinet);
	luaU_setenum(L, -1, "piccolo", midi::Piccolo);
	luaU_setenum(L, -1, "flute", midi::Flute);
	luaU_setenum(L, -1, "recorder", midi::Recorder);
	luaU_setenum(L, -1, "panflute", midi::Pan_Flute);
	luaU_setenum(L, -1, "blownbottle", midi::Blown_Bottle);
	luaU_setenum(L, -1, "skakuhachi", midi::Skakuhachi);
	luaU_setenum(L, -1, "whistle", midi::Whistle);
	luaU_setenum(L, -1, "ocarina", midi::Ocarina);
	luaU_setenum(L, -1, "lead1square", midi::Lead_1_Square);
	luaU_setenum(L, -1, "lead2sawtooth", midi::Lead_2_Sawtooth);
	luaU_setenum(L, -1, "lead3calliope", midi::Lead_3_Calliope);
	luaU_setenum(L, -1, "lead4chiff", midi::Lead_4_Chiff);
	luaU_setenum(L, -1, "lead5charang", midi::Lead_5_Charang);
	luaU_setenum(L, -1, "lead6voice", midi::Lead_6_Voice);
	luaU_setenum(L, -1, "lead7fifths", midi::Lead_7_Fifths);
	luaU_setenum(L, -1, "lead8basslead", midi::Lead_8_Bass_Lead);
	luaU_setenum(L, -1, "pad1new_age", midi::Pad_1_New_Age);
	luaU_setenum(L, -1, "pad2warm", midi::Pad_2_Warm);
	luaU_setenum(L, -1, "pad3polysynth", midi::Pad_3_Polysynth);
	luaU_setenum(L, -1, "pad4choir", midi::Pad_4_Choir);
	luaU_setenum(L, -1, "pad5bowed", midi::Pad_5_Bowed);
	luaU_setenum(L, -1, "pad6metallic", midi::Pad_6_Metallic);
	luaU_setenum(L, -1, "pad7halo", midi::Pad_7_Halo);
	luaU_setenum(L, -1, "pad8sweep", midi::Pad_8_Sweep);
	luaU_setenum(L, -1, "fx1rain", midi::FX_1_Rain);
	luaU_setenum(L, -1, "fx2soundtrack", midi::FX_2_Soundtrack);
	luaU_setenum(L, -1, "fx3crystal", midi::FX_3_Crystal);
	luaU_setenum(L, -1, "fx4atmosphere", midi::FX_4_Atmosphere);
	luaU_setenum(L, -1, "fx5brightness", midi::FX_5_Brightness);
	luaU_setenum(L, -1, "fx6goblins", midi::FX_6_Goblins);
	luaU_setenum(L, -1, "fx7echoes", midi::FX_7_Echoes);
	luaU_setenum(L, -1, "fx8scifi", midi::FX_8_Scifi);
	luaU_setenum(L, -1, "sitar", midi::Sitar);
	luaU_setenum(L, -1, "banjo", midi::Banjo);
	luaU_setenum(L, -1, "shamisen", midi::Shamisen);
	luaU_setenum(L, -1, "koto", midi::Koto);
	luaU_setenum(L, -1, "kalimba", midi::Kalimba);
	luaU_setenum(L, -1, "bagpipe", midi::Bagpipe);
	luaU_setenum(L, -1, "fiddle", midi::Fiddle);
	luaU_setenum(L, -1, "shanai", midi::Shanai);
	luaU_setenum(L, -1, "tinklebell", midi::Tinkle_Bell);
	luaU_setenum(L, -1, "agogo", midi::Agogo);
	luaU_setenum(L, -1, "steeldrums", midi::Steel_Drums);
	luaU_setenum(L, -1, "woodblock", midi::Woodblock);
	luaU_setenum(L, -1, "taikodrum", midi::Taiko_Drum);
	luaU_setenum(L, -1, "melodictom", midi::Melodic_Tom);
	luaU_setenum(L, -1, "synthdrum", midi::Synth_Drum);
	luaU_setenum(L, -1, "reversecymbal", midi::Reverse_Cymbal);
	luaU_setenum(L, -1, "guitarfret_noise", midi::Guitar_Fret_Noise);
	luaU_setenum(L, -1, "breathnoise", midi::Breath_Noise);
	luaU_setenum(L, -1, "seashore", midi::Seashore);
	luaU_setenum(L, -1, "birdtweet", midi::Bird_Tweet);
	luaU_setenum(L, -1, "telephonering", midi::Telephone_Ring);
	luaU_setenum(L, -1, "helicopter", midi::Helicopter);
	luaU_setenum(L, -1, "applause", midi::Applause);
	luaU_setenum(L, -1, "gunshot", midi::Gunshot);
	lua_setfield(L, -2, "instruments");

	return 1;
}

} // extern "C"