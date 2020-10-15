
#include <optional>
#include <string>

#include "LuaWrapper.hpp"
#include "LuaWrapperUtil.hpp"
#include "luamidiutils.hpp"
#include "midiutils.hpp"
#include "spdlog/spdlog.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

using namespace midi;
using namespace std;

////////////////////////////////////////////////////////////////////////////////

uint8_t luamidievent_getvelocity(lua_State* L, int index, const char* field) {
  return std::clamp(
      static_cast<uint8_t>(0x7F * luaU_getfield<double>(L, index, field)),
      uint8_t{0}, uint8_t{0x7F});
}

template <>
inline midi::Event luaU_check<>(lua_State* L, int index) {
  midi::Event event;
  event.timeDelta = luaU_getfield<uint32_t>(L, index, "timedelta");
  event.command =
      static_cast<Event::Command>(luaU_getfield<int>(L, index, "command"));
  event.channel = luaU_getfield<uint32_t>(L, index, "channel");
  switch (event.command) {
    case Event::Command::NoteEnd:
      event.noteEnd.noteNumber = luaU_getfield<uint8_t>(L, index, "notenumber");
      event.noteEnd.velocity = luamidievent_getvelocity(L, index, "velocity");
      break;
    case Event::Command::NoteBegin:
      event.noteBegin.noteNumber =
          luaU_getfield<uint8_t>(L, index, "notenumber");
      event.noteBegin.velocity = luamidievent_getvelocity(L, index, "velocity");
      break;
    case Event::Command::VelocityChange:
      event.velocityChange.noteNumber =
          luaU_getfield<uint8_t>(L, index, "notenumber");
      event.velocityChange.velocity =
          luamidievent_getvelocity(L, index, "velocity");
      break;
    case Event::Command::ControllerChange:
      event.controllerChange.controllerNumber =
          luaU_getfield<uint8_t>(L, index, "controllernumber");
      event.controllerChange.velocity =
          luamidievent_getvelocity(L, index, "velocity");
      break;
    case Event::Command::ProgramChange:
      event.programChange.newProgramNumber =
          luaU_getfield<uint8_t>(L, index, "programnumber");
      break;
    case Event::Command::ChannelPressureChange:
      event.channelPressureChange.channelNumber =
          luaU_getfield<uint8_t>(L, index, "channelnumber");
      break;
    case Event::Command::PitchWheelChange:
      break;
    case Event::Command::Meta:
      break;
  }
  return event;
}

lua_State* LuaEventProducer_prepareLuaState(const char* filename) {
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);

  lua_newtable(L);  // {}

  luamidiutils_pushCommandEnums(L);  // {} commands
  lua_setfield(L, -2, "commands");   // {}

  luamidiutils_pushInstrumentEnums(L);  // {} instruments
  lua_setfield(L, -2, "instruments");   // {}

  lua_setglobal(L, "midi");

  if (luaL_dofile(L, filename)) {
    exception ex(lua_tostring(L, -1));
    lua_close(L);
    throw ex;
  }

  return L;
}

uint32_t LuaEventProducer_getTicksPerBeat(lua_State* L) {
  lua_getglobal(L, "ticksperbeat");
  uint32_t ticksPerBeat = lua_tointeger(L, -1);
  lua_pop(L, 1);
  return ticksPerBeat ? ticksPerBeat : 96;
}

LuaEventProducer::LuaEventProducer(const char* filename)
    : m_L{LuaEventProducer_prepareLuaState(filename)},
      m_ticksPerBeat{LuaEventProducer_getTicksPerBeat(m_L)},
      m_eventListRef{luaL_ref(m_L, LUA_REGISTRYINDEX)},
      m_pendingMessages{} {}

LuaEventProducer::~LuaEventProducer() {
  luaL_unref(m_L, LUA_REGISTRYINDEX, m_eventListRef);
  lua_close(m_L);
}

void LuaEventProducer_handleMessages(lua_State* L,
                                     deque<string>& pendingMessages) {
  lua_getglobal(L, "handlemessage");
  for (auto str : pendingMessages) {
    lua_pushstring(L, str.c_str());
    if (lua_pcall(L, 1, 0, 0)) {
      spdlog::error("LuaEventProducer_handleMessages: {}\n",
                    lua_tostring(L, -1));
      lua_pop(L, 1);
    }
  }
  pendingMessages.clear();
}

void LuaEventProducer_prepareEvents(lua_State* L, int eventListRef) {
  lua_getglobal(L, "prepareevents");  // prepareevents
  if (lua_pcall(L, 0, 1, 0)) {
    // errorstr
    spdlog::error("LuaEventProducer_prepareEvents: {}\n", lua_tostring(L, -1));
    lua_pop(L, 2);  //
    return;
  }
  // The return value should be a list of channels. Channels are a list of
  // events that have absolute times and notes have lengths instead of end
  // events. We now go through each event to convert it to delta time and add in
  // the note end events, and then sort them.
  if (!lua_istable(L, -1)) {
    spdlog::error(
        "LuaEventProducer_prepareEvents: return value should have been a list");
    return;
  }

  // Create note end events.
  for (int channelIndex = 0; channelIndex < lua_rawlen(L, -1); channelIndex++) {
    lua_rawgeti(L, -1, channelIndex);  // channelList channel
    // Add note end events
    for (int eventIndex = 0; eventIndex < lua_rawlen(L, -1); eventIndex++) {
      lua_rawgeti(L, -1, eventIndex);  // channelList channel event
      lua_pushliteral(L, "command");   // channelList channel event "command"
      lua_rawget(L, -2);               // channelList channel event command
      auto command = luaL_checkinteger(L, -1);
      if (command == static_cast<lua_Integer>(Event::Command::NoteBegin)) {
        lua_newtable(L);                      // channelList channel event command {}
        lua_pushinteger(L, static_cast<lua_Integer>(Event::Command::NoteEnd));
                                              // channelList channel event command {} NoteEnd
        lua_setfield(L, -2, "command");       // channelList channel event command {}

        lua_getfield(L, -3, "notenumber");    // channelList channel event command {} noteNumber
        lua_setfield(L, -2, "notenumber");    // channelList channel event command {}
        
        lua_getfield(L, -3, "absolutetime");  // channelList channel event command {} absolutetime
        lua_getfield(L, -4, "length");        // channelList channel event command {} absolutetime length
        auto aboluteTime = luaL_checkinteger(L, -2) + luaL_checkinteger(L, -2);
        lua_pop(L, 2);                        // channelList channel event command {}
        lua_pushinteger(L, aboluteTime);      // channelList channel event command {} absolutetime
        lua_setfield(L, -2, "absolutetime");  // channelList channel event command {}

        auto length = lua_rawlen(L, -4);
        lua_rawseti(L, -4, length + 1);       // channelList channel event command 
      }
      lua_pop(L, 2);  // channelList channel
    }
    lua_pop(L, 1);  // channelList
  }

  // Sort the events

  // Add delta times
  lua_rawseti(L, LUA_REGISTRYINDEX, eventListRef);  //
}

void LuaEventProducer::preBufferFill() {
  LuaEventProducer_handleMessages(m_L, m_pendingMessages);
  LuaEventProducer_prepareEvents(m_L, m_eventListRef);
}

const std::optional<Event> LuaEventProducer::getNextEvent() {
  lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_eventListRef);  // list
  if (!lua_istable(m_L, -1)) {
    spdlog::error("LuaEventProducer::getNextEvent: Invalid event list\n");
    lua_pop(m_L, 1);
  }

  auto event = luaU_check<midi::Event>(m_L, -1);
  lua_pop(m_L, 1);
  return std::optional<midi::Event>{event};
}

uint32_t LuaEventProducer::getTicksPerBeat() { return m_ticksPerBeat; }

void LuaEventProducer::pushMessage(string message) {
  m_pendingMessages.push_back(message);
}
