#pragma once

#include "DebugUtils.h"

#include <string>

static std::string GetBaseDirectory(const std::string& fname) {
    size_t pos = fname.find_last_of("\\/");
    return (std::string::npos == pos) ? "" : fname.substr(0, pos + 1);
}

static uint8_t* ReadEntireFile(std::string filepath, long* outSize, bool abortOnFailure) {
    FILE* f;
    fopen_s(&f, filepath.c_str(), "rb");

    if (!f) {
        if (abortOnFailure) {
            ABORT_EXIT("Couldn't open file %s\n", filepath.c_str());
        }
        return nullptr;
    }

    fseek(f, 0, SEEK_END);
    long sz = ftell(f) + 1;
    fseek(f, 0, SEEK_SET);

    uint8_t* buffer = new uint8_t[sz];
    fread(buffer, sz - 1, 1, f);
    buffer[sz - 1] = 0;

    if (outSize)
        *outSize = sz;
    fclose(f);

    return buffer;
}