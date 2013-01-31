#ifndef LUA_MIDI_UTILS_
#define LUA_MIDI_UTILS_

#include "midiutils.hpp"

struct lua_State;

int luamidiutils_pushcommandenums(lua_State* L);
int luamidiutils_pushinstrumentenums(lua_State* L);

struct LuaMidiEvent
{
    midi::Event event;
    unsigned int absoluteTime;
};

class LuaEventProducer : public midi::EventProducer
{
public:
    LuaEventProducer(const char* filename);
    ~LuaEventProducer();

    virtual const midi::Event* getNextEvent(unsigned int& absoluteTime);
    virtual unsigned int getInitialTempo();

private:
    lua_State* m_L;
    LuaMidiEvent m_event;
};

#endif // LUA_MIDI_UTILS_
