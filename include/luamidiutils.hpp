#ifndef LUA_MIDI_UTILS_
#define LUA_MIDI_UTILS_

#include "midiutils.hpp"

#include <memory>
#include <deque>
#include <optional>

struct lua_State;
struct luaL_Reg;

int luamidiutils_pushCommandEnums(lua_State* L);
int luamidiutils_pushInstrumentEnums(lua_State* L);
int luamidiutils_pushLuaEventProducer(lua_State* L, luaL_Reg* metatable);

class LuaEventProducer : public midi::EventProducer
{
public:
    LuaEventProducer(const char* filename);
    ~LuaEventProducer() override;

    void preBufferFill() override;
    const std::optional<midi::Event> getNextEvent() override;
    unsigned int getTicksPerBeat() override;
    
    void pushMessage(std::string message);
    
private:
    lua_State* m_L;
    midi::Event m_event;
    int m_eventListRef;
    unsigned int m_ticksPerBeat;
    std::deque<std::string> m_pendingMessages;
};

#endif // LUA_MIDI_UTILS_
