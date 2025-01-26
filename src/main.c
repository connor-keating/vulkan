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

    // Start up section

    // Allocate program memory
    // Allocate application memory
    LPVOID app_memory_base = 0;
    size_t app_memory_size = (size_t) Gigabytes(1);
    void *app_memory = VirtualAlloc(app_memory_base, app_memory_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    ASSERT(app_memory, "ERROR: Unable to allocate application memory.");
    arena root_arena = {0};
    arena_init(&root_arena, app_memory, app_memory_size);
     
    printf("Hello, world\n");
    #if defined(_WIN64)
        printf("Compiled for x64.\n");
    #else
        printf("Not compiled for x64.\n");
    #endif
    return 0;
}