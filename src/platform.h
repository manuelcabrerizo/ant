#pragma once

#include "common.h"

struct File
{
    void* data;
    u64 size;
};

void* PlatformGetOsWindow();
void PlaformGetWindowPos(i32* x, i32* y);
void PlaformSetCursorPos(i32 x, i32 y);
void PlatformShowMouse(bool show);
void PlatformClientDimensions(i32* width, i32* height);
File PlatformReadFile(const char* filepath, i32 stackNum);
void PlatformQuitApplication();
int PlatformGetWindowDPI();