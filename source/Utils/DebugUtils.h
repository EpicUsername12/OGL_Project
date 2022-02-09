#pragma once

#include <Windows.h>

#define __FILENAME_X__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILENAME_X__)

#define ABORT_EXIT(FMT, ...)                                                  \
    char* formatBuf = new char[2048];                                         \
    snprintf(formatBuf, 2048,                                                 \
             "Fatal error occured\nYou can send it to the developer of this " \
             "application.\n\n" FMT,                                          \
             __VA_ARGS__);                                                    \
    MessageBoxA(NULL, formatBuf, "Fatal error", MB_OK);                       \
    ExitProcess(-1)

#define ABORT_EXIT_EX(TXT)                        \
    MessageBoxA(NULL, TXT, "Fatal error", MB_OK); \
    ExitProcess(-1)

#define WARN(FMT, ...) fprintf(stdout, "Warning: " FMT, __VA_ARGS__)