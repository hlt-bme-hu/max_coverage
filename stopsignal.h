#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#include <windows.h>
BOOL WINAPI consoleHandler(DWORD signal);
#else
#include <signal.h>
void new_handler(int signum);
#endif

int setup_stop_signal();

#ifdef __cplusplus
}
#endif
