#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "aic_tactics/diagnostics.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace AicTactics {
namespace SHC141 {

int diagnosticsEnabled = 0;

namespace {
// The framework's native log (ucp3.h ucp_log), the log Lua's log() writes to.
typedef void (__cdecl *UcpLog)(int, const char*);
const int InfoLevel = 0;

UcpLog resolveLog()
{
    const HMODULE ucp = GetModuleHandleA("ucp.dll");
    return ucp ? reinterpret_cast<UcpLog>(GetProcAddress(ucp, "ucp_log")) : 0;
}
} // namespace

void diagnostic(const char* format, ...)
{
    if (!diagnosticsEnabled) return;
    static const UcpLog ucpLog = resolveLog();
    if (!ucpLog) return;
    char message[256] = "[aic-tactics] ";
    const size_t prefix = std::strlen(message);
    va_list arguments;
    va_start(arguments, format);
    _vsnprintf_s(message + prefix, sizeof(message) - prefix, _TRUNCATE, format, arguments);
    va_end(arguments);
    ucpLog(InfoLevel, message);
}

} // namespace SHC141
} // namespace AicTactics
