#pragma once

#include <cstring>
#include <thread>
#include <chrono>
#include <cstdio>
#include <exception>

template<typename... Args>
bool matches(const char* str, Args... args)
{
    return true;
}

template<typename... Args>
bool matches(const char* str, const char* pattern1, Args... args)
{
    return ((strcmp(pattern1, str) == 0) || matches(str, args...));
}

template<>
bool matches(const char*)
{
    return false;
}

size_t nchoosek(size_t n, size_t k)
{
    size_t result = 1;
    for (size_t i = 0; i < k; ++i)
        result *= (n - i);
    for (size_t i = 1; i <= k; ++i)
        result /= i;
    return result;
}

template<class Exception = std::exception, typename Func, typename ...Args>
void ProgressIndicator(const char* fmt, bool enable, const Func& f, Args&&... args)
{
    bool run = true;
    std::thread progress;
    if (enable)
    {
        progress = std::thread([&]()
        {
            while (run)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                if (run)
                {
                    fprintf(stderr, fmt, args...);
                    fflush(stderr);
                }
            }
            fprintf(stderr, fmt, args...);
            fprintf(stderr, "\n");
        });
    }

    try
    {
        f();
    }
    catch (const Exception& e)
    {
        run = false;
        if (progress.joinable())
            progress.join();

        throw e;
    }
    run = false;
    if (progress.joinable())
        progress.join();
}
