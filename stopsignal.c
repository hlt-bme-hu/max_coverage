#include "stopsignal.h"

volatile int running = 1;

#ifdef _MSC_VER
BOOL WINAPI consoleHandler(DWORD signal)
{
    switch (signal)
    {
    case CTRL_C_EVENT:
        //case CTRL_BREAK_EVENT:
        //case CTRL_CLOSE_EVENT:
        running = 0;
        break;
    default:
        break;
    }
    return TRUE;
}

int setup_stop_signal()
{
    if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
        return 1;
    else
        return 0;
}

#else
void new_handler(int signum)
{
    running = 0;
}

int setup_stop_signal()
{
    if (signal(SIGINT, new_handler) == SIG_IGN)
    {
        signal(SIGINT, SIG_IGN);
        return 1;
    }
    else
        return 0;
}
#endif
