#include "midiutils.hpp"
#include "luamidiutils.hpp"

#include "LuaWrapper.hpp"
#include "LuaWrapperUtil.hpp"
#include "lua.hpp"

using namespace midi;

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
    luaEvent.absoluteTime = luaU_getfield<unsigned int>(L, index, "time");
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
        luaEvent.event.programChange.newProgramNumber = luaU_getfield<unsigned int>(L, index, "notenumber");
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
{
    luaL_openlibs(m_L);

    luamidiutils_pushcommandenums(m_L);
    lua_setglobal(m_L, "command");

    luamidiutils_pushinstrumentenums(m_L);
    lua_setglobal(m_L, "instrument");

    luaL_dofile(m_L, filename);
}

LuaEventProducer::~LuaEventProducer()
{
    lua_close(m_L);
}

const Event* LuaEventProducer::getNextEvent(unsigned int& absoluteTime)
{
    lua_getglobal(m_L, "getnextevent");
    lua_call(m_L, 0, 1);
    if (lua_istable(m_L, -1))
    {
        m_event = luaU_check<LuaMidiEvent>(m_L, -1);
        absoluteTime = m_event.absoluteTime;
        lua_pop(m_L, 1);
        return &m_event.event;
    }
    else
    {
        lua_pop(m_L, 1);
        return nullptr;
    }
}

unsigned int LuaEventProducer::getInitialTempo()
{
    lua_getglobal(m_L, "tempo");
    unsigned int tempo = lua_tointeger(m_L, -1);
    lua_pop(m_L, 1);
    return tempo ? tempo : 120;
}
