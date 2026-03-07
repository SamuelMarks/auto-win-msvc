#ifdef _WIN32
#if defined(_MSC_VER) && _MSC_VER < 1700
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <minwindef.h>
#include <sysinfoapi.h>
#include <timezoneapi.h>
#include <fileapi.h>
#include <handleapi.h>
#endif
#endif
int main() { return 0; }
