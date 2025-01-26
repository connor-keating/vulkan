#pragma once

#pragma warning(push, 0)
#include <windows.h>
#include <stdio.h>
#pragma warning(pop)
#include <vulkan/vulkan.h >

#include "core.c"

global bool32 RUNNING = 0;

LRESULT CALLBACK ANSIWindowsProcedure(HWND windowHandle, UINT messageID, WPARAM wParam, LPARAM lParam)
{ 
    // Characters are coming in Unicode 
    // lstrcmpW("Q", (LPCWSTR) wParam)
    LRESULT result = 0;
    switch (messageID) 
    { 
        case WM_CLOSE:
        {
            OutputDebugStringA("Closing time!\n");
            RUNNING = false;
            PostQuitMessage(0);
            break;
        }
        case WM_SIZE:
            // Set the size and position of the window. 
            RECT windowDimensions = {0};
            GetClientRect(windowHandle, &windowDimensions); 
            // int left, int top, int right, int bottom
            // glViewport(windowDimensions.left, windowDimensions.top, windowDimensions.right, windowDimensions.bottom);
        default: 
            result = DefWindowProcA(windowHandle, messageID, wParam, lParam); 
    }
    return result;
} 


internal void win32_message_procedure(HWND windowHandle)
{
    MSG osMessage = {0};
    bool32 message_current = true;
    while (message_current)
    {
        message_current = PeekMessageA(&osMessage, windowHandle, 0, 0, PM_REMOVE);
        switch (osMessage.message)
        {
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                u32 VKCode = (u32) osMessage.wParam;
                bool32 altKeyValue =  (1 << 29);
                bool32 wasDownValue = (1 << 30);
                bool32 isDownValue =  (1 << 31);
                // if both of these int32s have a 1 bit in the same position the result will also have that bit set
                bool32 wasDownBitCheck = (osMessage.lParam & wasDownValue);
                bool32 isDownBitCheck  = (osMessage.lParam & isDownValue);
                bool32 altKeyBitCheck  = (osMessage.lParam & altKeyValue);
                bool32 WasDown = (wasDownBitCheck != 0);
                bool32 isDown  = (isDownBitCheck  == 0);
                if(WasDown != isDown)
                {
                    if ( (VKCode == VK_ESCAPE) || ((VKCode == VK_F4) && altKeyBitCheck) )
                    {
                        SendMessageA(windowHandle, WM_CLOSE, 0, 0);
                    }
                }
                break;
            }
        default:
        {
            TranslateMessage(&osMessage); // turn keystrokes into characters
            DispatchMessageA(&osMessage); // tell OS to call window procedure
            break;
        }
        }
    }
}


internal HWND window_init(HINSTANCE currentInstanceHandle, int displayFlag)
{
    i32 window_x = 50;
    i32 window_y = 90;
    i32 window_w = 50;
    i32 window_h = 50;
    char *window_name = "Vulkan Experiment";
    // Create win32 window class.
    WNDCLASSEXA window_class = {0};
    window_class.cbSize = sizeof(window_class);
    window_class.style = CS_HREDRAW|CS_VREDRAW, // |CS_OWNDC;
    window_class.lpfnWndProc = ANSIWindowsProcedure;
    window_class.hInstance = currentInstanceHandle;
    window_class.hCursor = LoadCursor(0, IDC_ARROW);
    // window_class.hbrBackground = CreateSolidBrush(RGB(255, 0, 255)); // Magenta background
    window_class.hbrBackground = (HBRUSH)COLOR_WINDOW;
    window_class.lpszClassName = "WINDOW_CLASS";
    ATOM window_id = RegisterClassExA(&window_class);
    ASSERT(window_id, "ERROR: Failed to register window.");
    
    // Create the window we described.
    HWND window_handle = CreateWindowExA(
        WS_EX_CLIENTEDGE,            // style_extended: has list of possible values.    
        window_class.lpszClassName,  // class_name: null-terminated string.         
        window_name,                  // name: string window name to display in title bar.              
        WS_OVERLAPPEDWINDOW,        // style_basic: has list of possible values.        
        window_x,                          // position_x: Horizontal window position      
        window_y,                          // position_y: Vertical window position         
        window_w,                          // width: Window width in screen coordinates
        window_h,                          // height: Window height in screen coordinates
        0,                          // window_parent: Handle to the parent window.
        0,                          // window_menu: Optional child window ID.
        currentInstanceHandle,      // window_handle: handle to this window's module.
        0                          // window_data_pointer: Pointer to CREATESTRUCT var that sends a message to the window.
    );
    ASSERT(window_handle, "ERROR: Failed to create window.");

    // Get monitor info and resize the window.
    MONITORINFO monitor_info = {0};
    monitor_info.cbSize = sizeof(MONITORINFO);
    HMONITOR monitor_handle = MonitorFromWindow(window_handle, MONITOR_DEFAULTTOPRIMARY);
    BOOL monitor_info_success = GetMonitorInfoA(monitor_handle, &monitor_info);
    ASSERT(monitor_info_success, "ERROR: Failed to get monitor info.");
    i32 monitor_width  = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
    i32 monitor_height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;
    RECT window_border = {0};
    AdjustWindowRect(&window_border, WS_OVERLAPPEDWINDOW, FALSE);
    // Calculate our desired window dimensions.
    window_w = ( monitor_width / 2 )  + ( window_border.right - window_border.left );
    window_h = ( monitor_height / 2 ) + ( window_border.bottom - window_border.top );
    window_x = (monitor_width / 2) -  (window_w / 2);
    window_y = (monitor_height / 2) - (window_h / 2);
    BOOL window_success = SetWindowPos(
        window_handle,
        HWND_TOP,
        window_x,
        window_y,
        window_w,
        window_h,
        0
    );
    ASSERT(window_success, "Failed to resize and open window.");
    ShowWindow(window_handle, displayFlag);

    return window_handle;
}


internal void render_init()
{
    VkInstance instance;

    // Fill in application info.
    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Data Visualization";
    app_info.applicationVersion = VK_MAKE_VERSION(1,0,0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1,0,0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    // Windows interface extensions
    const char* instanceExtensions[] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };

    // Fill in create info
    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    // Specify global validation calls.
    create_info.enabledExtensionCount = sizeof(instanceExtensions) / sizeof(instanceExtensions[0]);
    create_info.ppEnabledExtensionNames = instanceExtensions;
    create_info.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&create_info, 0, &instance);
    ASSERT((result == 0), "ERROR: Failed to create vulkan instance.");
}


int WINAPI WinMain(HINSTANCE currentInstanceHandle, HINSTANCE prevInstanceHandle, PSTR argsCommandLine, int displayFlag)
{
    UNREFERENCED_PARAMETER(currentInstanceHandle);
    UNREFERENCED_PARAMETER(prevInstanceHandle);
    UNREFERENCED_PARAMETER(argsCommandLine);

    // Start up section

    // Allocate program memory
    LPVOID app_memory_base = 0;
    size_t app_memory_size = (size_t) Gigabytes(1);
    void *app_memory = VirtualAlloc(app_memory_base, app_memory_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    ASSERT(app_memory, "ERROR: Unable to allocate application memory.");
    arena root_arena = {0};
    arena_init(&root_arena, app_memory, app_memory_size);

    // Set global == 1 b/c we're good to go.
    RUNNING = true;


    // Initialize a window
    HWND window_handle = window_init(currentInstanceHandle, displayFlag);

    // Initialize renderer
    render_init();

    
    // Main loop
    while (RUNNING)
    {
        // Message loop
        win32_message_procedure(window_handle);
    }

    return 0;
}