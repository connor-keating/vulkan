#pragma once

#pragma warning(push, 0)
#include <windows.h>
#include <stdio.h>
#pragma warning(pop)
#include <vulkan/vulkan.h >

#include "core.c"

int WINAPI WinMain(HINSTANCE currentInstanceHandle, HINSTANCE prevInstanceHandle, PSTR argsCommandLine, int displayFlag)
{
    UNREFERENCED_PARAMETER(currentInstanceHandle);
    UNREFERENCED_PARAMETER(prevInstanceHandle);
    UNREFERENCED_PARAMETER(argsCommandLine);
    UNREFERENCED_PARAMETER(displayFlag);

    printf("Hello, world\n");
    #if defined(_WIN64)
        printf("Compiled for x64.\n");
    #else
        printf("Not compiled for x64.\n");
    #endif
    return 0;
}