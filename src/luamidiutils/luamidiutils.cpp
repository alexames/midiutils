#include "midiutils.hpp"

#include "LuaWrapper.hpp"

#include "lauxlib.h"
#include "lua.h"

#define MIDIUTILS_EXPORT __declspec(dllexport)

namespace midi
{

static luaL_Reg Test_table[] =
{
	{ NULL, NULL }
};

} // namespace midi

extern "C"
{

MIDIUTILS_EXPORT int luaopen_luamidiutils(lua_State* L)
{
	luaL_register(L, "luamidiutils", midi::Test_table);
	return 1;
}

}
