#include "midiutils.hpp"
#include "luamidiutils.hpp"

#include "LuaWrapper.hpp"
#include "LuaWrapperUtil.hpp"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include <string>
#include <iostream>

using namespace midi;
using namespace std;

////////////////////////////////////////////////////////////////////////////////

LuaEventProducer* LuaEventProducer_ctor(lua_State* L)
{
    return new LuaEventProducer(luaL_checkstring(L, 1));
}

int LuaEventProducer_getNextMessage(lua_State* L)
{
    LuaEventProducer* producer = luaW_check<LuaEventProducer>(L, 1);
    string message = producer->getNextMessage();
    if (message.length())
    {
        lua_pushstring(L, message.c_str());
        return 1;
    }
    else
    {
        return 0;
    }
}

static luaL_Reg LuaEventProducer_getNextMessageTable[] =
{
    { "getnextmessage", LuaEventProducer_getNextMessage },
    { NULL, NULL }
};

int luamidiutils_pushLuaEventProducer(lua_State* L, luaL_Reg* metatable)
{
    luaW_register<LuaEventProducer>(L, "LuaEventProducer", NULL, metatable, LuaEventProducer_ctor);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////

unsigned int luamidievent_getvelocity(lua_State* L, int index, const char* field)
{
    double value = luaU_getfield<unsigned int>(L, index, field);
    if (value < 0) 
        return 0;
    else if (value <= 1)
        return static_cast<unsigned int>(0x7F * value);
    else
        return 0x7F;
}

template <>
inline LuaMidiEvent luaU_check<>(lua_State* L, int index)
{
    LuaMidiEvent luaEvent;
    luaEvent.event.timeDelta = luaU_getfield<unsigned int>(L, index, "timedelta");
    luaEvent.absoluteTime = luaU_getfield<double>(L, index, "time");
    luaEvent.event.command = static_cast<Event::Command>(luaU_getfield<int>(L, index, "command"));
    luaEvent.event.channel = luaU_getfield<unsigned int>(L, index, "channel");
    switch (luaEvent.event.command)
    {
    case Event::NoteEnd:
        luaEvent.event.noteEnd.noteNumber = luaU_getfield<unsigned int>(L, index, "notenumber");
        luaEvent.event.noteEnd.velocity = luamidievent_getvelocity(L, index, "velocity");
        break;
    case Event::NoteBegin:
        luaEvent.event.noteBegin.noteNumber = luaU_getfield<unsigned int>(L, index, "notenumber");
        luaEvent.event.noteBegin.velocity = luamidievent_getvelocity(L, index, "velocity");
        break;
    case Event::VelocityChange:
        luaEvent.event.velocityChange.noteNumber = luaU_getfield<unsigned int>(L, index, "notenumber");
       luaEvent. event.velocityChange.velocity = luamidievent_getvelocity(L, index, "velocity");
        break;
    case Event::ControllerChange:
        luaEvent.event.controllerChange.controllerNumber = luaU_getfield<unsigned int>(L, index, "controllernumber");
        luaEvent.event.controllerChange.velocity = luamidievent_getvelocity(L, index, "velocity");
        break;
    case Event::ProgramChange:
        luaEvent.event.programChange.newProgramNumber = luaU_getfield<unsigned int>(L, index, "programnumber");
        break;
    case Event::ChannelPressureChange:
        luaEvent.event.channelPressureChange.channelNumber= luaU_getfield<unsigned int>(L, index, "channelnumber");
        break;
    case Event::PitchWheelChange:
        break;
    case Event::Meta:
        break;
    }
    return luaEvent;
}

LuaEventProducer::LuaEventProducer(const char* filename)
    : m_L(luaL_newstate())
    , m_event()
    , m_ticksPerBeat(0)
    , m_pendingMessages()
{
    luaL_openlibs(m_L);

    luamidiutils_pushLuaEventProducer(m_L, LuaEventProducer_getNextMessageTable);
    lua_setglobal(m_L, "LuaEventProducer");

    luamidiutils_pushCommandEnums(m_L);
    lua_setglobal(m_L, "commands");

    luamidiutils_pushInstrumentEnums(m_L);
    lua_setglobal(m_L, "instruments");

    if (luaL_dofile(m_L, filename))
    {
        printf("LuaEventProducer::LuaEventProducer: %s\n", lua_tostring(m_L, -1));
        lua_pop(m_L, 1);
    }

    lua_getglobal(m_L, "ticksperbeat");
    m_ticksPerBeat = lua_tointeger(m_L, -1);
    lua_pop(m_L, 1);

    if (m_ticksPerBeat == 0) 
        m_ticksPerBeat = 96;
}

LuaEventProducer::~LuaEventProducer()
{
    lua_close(m_L);
}

const Event* LuaEventProducer::getNextEvent(unsigned int& absoluteTime)
{
    lua_getglobal(m_L, "getnextevent");
    luaW_push(m_L, this);
    if (lua_pcall(m_L, 1, 1, 0) == 0)
    {
        if (lua_istable(m_L, -1))
        {
            m_event = luaU_check<LuaMidiEvent>(m_L, -1);
            absoluteTime = static_cast<unsigned int>(m_event.absoluteTime * m_ticksPerBeat);
            lua_pop(m_L, 1);
            return &m_event.event;
        }
        else
        {
            lua_pop(m_L, 1);
        }
    }
    else
    {
        printf("LuaEventProducer::getNextEvent: %s\n", lua_tostring(m_L, -1));
        lua_pop(m_L, 1);
    }
    return nullptr;
}

unsigned int LuaEventProducer::getTicksPerBeat()
{
    return m_ticksPerBeat;
}

void LuaEventProducer::sendMessage(string message)
{
    m_pendingMessages.push_back(message);
}

string LuaEventProducer::getNextMessage()
{
    if (m_pendingMessages.size())
    {
        string message = m_pendingMessages.front();
        m_pendingMessages.pop_front();
        return message;
    }
    else
    {
        return string();
    }
}
