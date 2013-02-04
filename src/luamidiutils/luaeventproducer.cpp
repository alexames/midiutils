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

lua_State* LuaEventProducer_prepareLuaState(const char* filename)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    luamidiutils_pushCommandEnums(L);
    lua_setglobal(L, "commands");

    luamidiutils_pushInstrumentEnums(L);
    lua_setglobal(L, "instruments");

    if (luaL_dofile(L, filename))
    {
        exception ex(lua_tostring(L, -1));
        lua_close(L);
        throw ex;
    }

    return L;
}

unsigned int LuaEventProducer_getTicksPerBeat(lua_State* L)
{
    lua_getglobal(L, "ticksperbeat");
    unsigned int ticksPerBeat = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return ticksPerBeat ? ticksPerBeat : 96;
}

LuaEventProducer::LuaEventProducer(const char* filename)
    : m_L(LuaEventProducer_prepareLuaState(filename))
    , m_event()
    , m_ticksPerBeat(LuaEventProducer_getTicksPerBeat(m_L))
    , m_pendingMessages()
{
}

LuaEventProducer::~LuaEventProducer()
{
    lua_close(m_L);
}

void LuaEventProducer_handleMessages(lua_State* L, deque<string>& pendingMessages)
{
    lua_getglobal(L, "handlemessage");
    lua_getglobal(L, "context");
    for (auto iter = begin(pendingMessages); iter != end(pendingMessages); iter++)
    {
        lua_pushvalue(L, -2);
        lua_pushvalue(L, -2);
        lua_pushstring(L, iter->c_str());
        if (lua_pcall(L, 2, 0, 0))
        {
            printf("LuaEventProducer_handleMessages: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
    pendingMessages.clear();
}

void LuaEventProducer_prepareEvents(lua_State* L)
{
    lua_getglobal(L, "prepareevents");
    lua_getglobal(L, "context");
    if (lua_pcall(L, 1, 0, 0))
    {
        printf("LuaEventProducer_prepareEvents: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

void LuaEventProducer::preBufferFill()
{
    LuaEventProducer_handleMessages(m_L, m_pendingMessages);
    LuaEventProducer_prepareEvents(m_L);
}

const Event* LuaEventProducer::getNextEvent(unsigned int& absoluteTime)
{
    lua_getglobal(m_L, "getnextevent");
    lua_getglobal(m_L, "context");
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

void LuaEventProducer::pushMessage(string message)
{
    m_pendingMessages.push_back(message);
}

