#ifndef LUA_MIDI_UTILS_
#define LUA_MIDI_UTILS_

#include "midiutils.hpp"

#include <memory>
#include <deque>

struct lua_State;
struct luaL_Reg;

int luamidiutils_pushCommandEnums(lua_State* L);
int luamidiutils_pushInstrumentEnums(lua_State* L);
int luamidiutils_pushLuaEventProducer(lua_State* L, luaL_Reg* metatable);

struct LuaMidiEvent
{
    midi::Event event;
    double absoluteTime;
};

class LuaEventProducer : public midi::EventProducer
{
public:
    LuaEventProducer(const char* filename);
    ~LuaEventProducer();

    virtual const midi::Event* getNextEvent(unsigned int& absoluteTime);
    virtual unsigned int getTicksPerBeat();
    
    void sendMessage(std::string message);
    std::string getNextMessage();
    
private:
    lua_State* m_L;
    LuaMidiEvent m_event;
    unsigned int m_ticksPerBeat;
    std::deque<std::string> m_pendingMessages;
};

#endif // LUA_MIDI_UTILS_
