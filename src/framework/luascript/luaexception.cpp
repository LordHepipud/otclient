#include "luaexception.h"
#include "luainterface.h"

LuaException::LuaException(const std::string& error, int traceLevel)
{
    g_lua.clearStack(); // on every exception, clear lua stack
    generateLuaErrorMessage(error, traceLevel);
}

void LuaException::generateLuaErrorMessage(const std::string& error, int traceLevel)
{
    // append trace level to error message
    if(traceLevel >= 0)
        m_what = aux::make_string("LUA ERROR: ", g_lua.traceback(error, traceLevel));
    else
        m_what = aux::make_string("LUA ERROR: ", error);
}

LuaBadNumberOfArgumentsException::LuaBadNumberOfArgumentsException(int expected, int got)
{
    std::string error = "attempt to call a function with wrong number of arguments";
    if(expected >= 0 && got >= 0)
        error = aux::make_string(error, " (expected ", expected, ", but got ", got, ")");
    generateLuaErrorMessage(error, 1);
}

LuaBadValueCastException::LuaBadValueCastException(const std::string& luaTypeName, const std::string& cppTypeName)
{
    std::string error = aux::make_string("attempt to cast a '", luaTypeName, "' lua value to '", cppTypeName, "'");
    generateLuaErrorMessage(error, 0);
}
