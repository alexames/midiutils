
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
  return std::clamp(static_cast<uint8_t>(0x7F * luaU_getfield<double>(L, index, field)),
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
      m_pendingMessages{} {}

LuaEventProducer::~LuaEventProducer() { lua_close(m_L); }

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

void LuaEventProducer_prepareEvents(lua_State* L) {
  lua_getglobal(L, "prepareevents");
  if (lua_pcall(L, 0, 0, 0)) {
    spdlog::error("LuaEventProducer_prepareEvents: {}\n", lua_tostring(L, -1));
    lua_pop(L, 1);
  }
}

void LuaEventProducer::preBufferFill() {
  LuaEventProducer_handleMessages(m_L, m_pendingMessages);
  LuaEventProducer_prepareEvents(m_L);
}

const std::optional<Event> LuaEventProducer::getNextEvent() {
  lua_getglobal(m_L, "getnextevent");
  if (lua_pcall(m_L, 0, 1, 0) == 0) {
    if (lua_istable(m_L, -1)) {
      auto event = luaU_check<midi::Event>(m_L, -1);
      lua_pop(m_L, 1);
      return std::optional<midi::Event>{event};
    } else {
      lua_pop(m_L, 1);
    }
  } else {
    spdlog::error("LuaEventProducer::getNextEvent: {}\n",
                  lua_tostring(m_L, -1));
    lua_pop(m_L, 1);
  }
  return std::nullopt;
}

uint32_t LuaEventProducer::getTicksPerBeat() { return m_ticksPerBeat; }

void LuaEventProducer::pushMessage(string message) {
  m_pendingMessages.push_back(message);
}
