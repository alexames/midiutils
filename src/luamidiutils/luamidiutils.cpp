#include "midiutils.hpp"
#include "luamidiutils.hpp"

#include "LuaWrapper.hpp"
#include "LuaWrapperUtil.hpp"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include <fstream>
#include <exception>
#include <memory>

#define LUAMIDIUTILS_EXPORT __declspec(dllexport)

using namespace std;
using namespace midi;

////////////////////////////////////////////////////////////////////////
//
// midiutils
//

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

////////////////////////////////////////////////////////////////////////
//
// MidiFileEventProducer
//

MidiFileEventProducer* MidiFileEventProducer_ctor(lua_State* L)
{
	MidiFile* midi = luaW_check<MidiFile>(L, 1);
	if (midi)
	{
		return new MidiFileEventProducer(*midi);
	}
	else 
	{
		return nullptr;
	}
}

////////////////////////////////////////////////////////////////////////
//
// MidiStream
//

MidiStream* MidiStream_ctor(lua_State* L)
{
	EventProducer* producer = luaW_check<EventProducer>(L, 1);
	if (producer)
	{
		return new MidiStream(*producer);
	}
	else 
	{
		return nullptr;
	}
}

int MidiStream_play(lua_State* L)
{
	MidiStream* stream = luaW_check<MidiStream>(L, 1);
	if (stream)
	{
		stream->play();
	}
	return 0;
}

static luaL_Reg MidiStream_metatable[] =
{
	{ "play", MidiStream_play },
	{ NULL, NULL }
};

////////////////////////////////////////////////////////////////////////////////

LuaEventProducer* LuaEventProducer_ctor(lua_State* L)
{
    return new LuaEventProducer(luaL_checkstring(L, 1));
}

int LuaEventProducer_pushMessage(lua_State* L)
{
    LuaEventProducer* producer = luaW_check<LuaEventProducer>(L, 1);
    const char* messagestring = luaL_checkstring(L, 2);
    if (messagestring && messagestring[0])
    {
        producer->pushMessage(string(messagestring));
    }
    return 0;
}

static luaL_Reg LuaEventProducer_metatablesender[] =
{
    { "pushmessage", LuaEventProducer_pushMessage },
    { NULL, NULL }
};

////////////////////////////////////////////////////////////////////////////////

extern "C"
{

LUAMIDIUTILS_EXPORT int luaopen_luamidiutils(lua_State* L)
{
	luaL_register(L, "lmu", luamidiutils_table);
	
	luaW_register<MidiFile>(L, "MidiFile", NULL, NULL);
	lua_setfield(L, -2, "MidiFile");

	luaW_register<EventProducer>(L, "EventProducer", NULL, NULL, NULL, NULL);
	lua_setfield(L, -2, "EventProducer");

	luaW_register<MidiFileEventProducer>(L, "MidiFileEventProducer", NULL, NULL, MidiFileEventProducer_ctor);
	luaW_extend<MidiFileEventProducer, EventProducer>(L);
	lua_setfield(L, -2, "MidiFileEventProducer");
    
	luaW_register<MidiStream>(L, "MidiStream", NULL, MidiStream_metatable, MidiStream_ctor);
	lua_setfield(L, -2, "MidiStream");

	luaW_register<LuaEventProducer>(L, "MidiStream", NULL, LuaEventProducer_metatablesender, LuaEventProducer_ctor);
	luaW_extend<LuaEventProducer, EventProducer>(L);
	lua_setfield(L, -2, "LuaEventProducer");

    luamidiutils_pushInstrumentEnums(L);
	lua_setfield(L, -2, "instruments");

    luamidiutils_pushCommandEnums(L);
	lua_setfield(L, -2, "commands");

	return 1;
}

} // extern "C"