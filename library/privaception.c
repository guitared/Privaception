#include <stdio.h>
#include <windows.h>
#include <winioctl.h>
#include <time.h>  // Added for randomization
#include "privaception.h"  // Changed include

// Changed all IOCTL codes and added randomization
#define IOCTL_SET_PRIORITY      CTL_CODE(FILE_DEVICE_UNKNOWN, 0xAA1, METHOD_BUFFERED, FILE_ANY_ACCESS)  // Changed
#define IOCTL_GET_PRIORITY      CTL_CODE(FILE_DEVICE_UNKNOWN, 0xBA2, METHOD_BUFFERED, FILE_ANY_ACCESS)  // Changed
#define IOCTL_SET_EVENT_FILTER  CTL_CODE(FILE_DEVICE_UNKNOWN, 0xCA3, METHOD_BUFFERED, FILE_ANY_ACCESS)  // Changed
#define IOCTL_GET_EVENT_FILTER  CTL_CODE(FILE_DEVICE_UNKNOWN, 0xDA4, METHOD_BUFFERED, FILE_ANY_ACCESS)  // Changed
#define IOCTL_SET_NOTIFICATION  CTL_CODE(FILE_DEVICE_UNKNOWN, 0xEA5, METHOD_BUFFERED, FILE_ANY_ACCESS)  // Changed
#define IOCTL_SEND_INPUT        CTL_CODE(FILE_DEVICE_UNKNOWN, 0xFA6, METHOD_BUFFERED, FILE_ANY_ACCESS)  // Changed
#define IOCTL_GET_INPUT         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1A7, METHOD_BUFFERED, FILE_ANY_ACCESS)  // Changed
#define IOCTL_QUERY_DEVICE_ID   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2A8, METHOD_BUFFERED, FILE_ANY_ACCESS)  // Changed

// Changed structure names and added padding
typedef struct _PRIV_KEYBOARD_DATA
{
    USHORT DeviceId;
    USHORT ScanCode;
    USHORT ActionFlags;
    USHORT Reserved1;
    ULONG  Timestamp;
    ULONG  ExtraInfo;
    UCHAR  Padding[8];  // Added padding
} PRIV_KEYBOARD_DATA, *PPRIV_KEYBOARD_DATA;

typedef struct _PRIV_MOUSE_DATA
{
    USHORT DeviceId;
    USHORT Flags;
    USHORT ButtonActions;
    USHORT WheelDelta;
    ULONG  RawButtons;
    LONG   PositionX;
    LONG   PositionY;
    ULONG  ExtraInfo;
    UCHAR  Padding[12];  // Added padding
} PRIV_MOUSE_DATA, *PPRIV_MOUSE_DATA;

// Changed context structure
typedef struct
{
    void *device_handle;
    void *event_handle;
    DWORD last_activity;
    BYTE  session_id[16];
} *PrivDeviceArray;

// Helper function for randomization
DWORD get_random_offset() {
    return (GetTickCount() % 256) * 4;
}

PrivContext priv_create_context(void)
{
    PrivDeviceArray device_array = NULL;
    char device_name[] = "\\\\.\\privinput00";  // Changed device name
    DWORD bytes_returned;
    DeviceHandle i;
    DWORD rand_offset = get_random_offset();

    // Changed allocation with random padding
    device_array = (PrivDeviceArray)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
        PRIVACEPTION_MAX_DEVICE * sizeof(*device_array) + rand_offset);
    
    if(!device_array) return NULL;

    for(i = 0; i < PRIVACEPTION_MAX_DEVICE; ++i)
    {
        HANDLE event_handles[3] = {0};
        DWORD create_flags = (i % 2) ? CREATE_NEW : 0;

        sprintf(&device_name[sizeof(device_name) - 3], "%02d", i + (rand_offset % 10));

        device_array[i].device_handle = CreateFileA(
            device_name, 
            GENERIC_READ | GENERIC_WRITE,  // Changed access
            FILE_SHARE_READ | FILE_SHARE_WRITE,  // Changed sharing
            NULL, 
            OPEN_EXISTING, 
            FILE_FLAG_OVERLAPPED,  // Added flag
            NULL);

        if (device_array[i].device_handle == INVALID_HANDLE_VALUE) {
            priv_destroy_context(device_array);
            return NULL;
        }

        // Changed event creation with random name
        char event_name[32];
        sprintf(event_name, "PrivEvent%08X", GetCurrentProcessId() + i);
        device_array[i].event_handle = CreateEventA(NULL, TRUE, FALSE, event_name);

        if(device_array[i].event_handle == NULL)
        {
            priv_destroy_context(device_array);
            return NULL;
        }

        event_handles[0] = device_array[i].event_handle;
        event_handles[1] = (HANDLE)((DWORD)event_handles[0] + rand_offset);  // Obfuscation
        event_handles[2] = NULL;

        if(!DeviceIoControl(
            device_array[i].device_handle, 
            IOCTL_SET_NOTIFICATION,  // Changed IOCTL
            event_handles, 
            sizeof(event_handles), 
            NULL, 0, &bytes_returned, NULL))
        {
            priv_destroy_context(device_array);
            return NULL;
        }

        device_array[i].last_activity = GetTickCount();
        GenerateRandomBytes(device_array[i].session_id, sizeof(device_array[i].session_id));
    }

    return device_array;
}

void priv_destroy_context(PrivContext context)
{
    PrivDeviceArray device_array = (PrivDeviceArray)context;
    unsigned int i;

    if(!context) return;

    for(i = 0; i < PRIVACEPTION_MAX_DEVICE; ++i)
    {
        if(device_array[i].device_handle != INVALID_HANDLE_VALUE) {
            // Send shutdown notification
            DWORD bytes_returned;
            DeviceIoControl(
                device_array[i].device_handle, 
                IOCTL_SET_PRIORITY, 
                NULL, 0, NULL, 0, 
                &bytes_returned, 
                NULL);
                
            CloseHandle(device_array[i].device_handle);
        }

        if(device_array[i].event_handle != NULL)
            CloseHandle(device_array[i].event_handle);
    }

    HeapFree(GetProcessHeap(), 0, context);
}

PriorityLevel priv_get_priority(PrivContext context, DeviceHandle device)
{
    PrivDeviceArray device_array = (PrivDeviceArray)context;
    PriorityLevel priority = 0;
    DWORD bytes_returned;

    if(context && device_array[device - 1].device_handle)
        DeviceIoControl(
            device_array[device - 1].device_handle, 
            IOCTL_GET_PRIORITY,  // Changed IOCTL
            NULL, 0, 
            (LPVOID)&priority, sizeof(PriorityLevel), 
            &bytes_returned, 
            NULL);

    return priority;
}

void priv_set_priority(PrivContext context, DeviceHandle device, PriorityLevel level)
{
    PrivDeviceArray device_array = (PrivDeviceArray)context;
    DWORD bytes_returned;

    if(context && device_array[device - 1].device_handle)
        DeviceIoControl(
            device_array[device - 1].device_handle, 
            IOCTL_SET_PRIORITY,  // Changed IOCTL
            (LPVOID)&level, sizeof(PriorityLevel), 
            NULL, 0, 
            &bytes_returned, 
            NULL);
}

EventFilter priv_get_filter(PrivContext context, DeviceHandle device)
{
    PrivDeviceArray device_array = (PrivDeviceArray)context;
    EventFilter filter = 0;
    DWORD bytes_returned;

    if(context && device_array[device - 1].device_handle)
        DeviceIoControl(
            device_array[device - 1].device_handle, 
            IOCTL_GET_EVENT_FILTER,  // Changed IOCTL
            NULL, 0, 
            (LPVOID)&filter, sizeof(EventFilter), 
            &bytes_returned, 
            NULL);

    return filter;
}

void priv_set_filter(PrivContext context, DevicePredicate predicate, EventFilter filter)
{
    PrivDeviceArray device_array = (PrivDeviceArray)context;
    DeviceHandle i;
    DWORD bytes_returned;

    if(context) {
        for(i = 0; i < PRIVACEPTION_MAX_DEVICE; ++i) {
            if(device_array[i].device_handle && predicate(i + 1)) {
                // Add random delay to break timing patterns
                Sleep((GetTickCount() % 7) + 1);
                
                DeviceIoControl(
                    device_array[i].device_handle, 
                    IOCTL_SET_EVENT_FILTER,  // Changed IOCTL
                    (LPVOID)&filter, sizeof(EventFilter), 
                    NULL, 0, 
                    &bytes_returned, 
                    NULL);
            }
        }
    }
}

DeviceHandle priv_wait(PrivContext context)
{
    return priv_wait_timeout(context, INFINITE);
}

DeviceHandle priv_wait_timeout(PrivContext context, unsigned long milliseconds)
{
    PrivDeviceArray device_array = (PrivDeviceArray)context;
    HANDLE wait_handles[PRIVACEPTION_MAX_DEVICE];
    DWORD i, j, k;

    if(!context) return 0;

    // Build wait handles array with random order
    for(i = 0, j = 0; i < PRIVACEPTION_MAX_DEVICE; ++i)
    {
        DWORD idx = (i + (GetTickCount() % 3)) % PRIVACEPTION_MAX_DEVICE;
        if (device_array[idx].event_handle) {
            wait_handles[j++] = device_array[idx].event_handle;
        }
    }

    k = WaitForMultipleObjects(j, wait_handles, FALSE, milliseconds);

    if(k == WAIT_FAILED || k == WAIT_TIMEOUT) return 0;

    // Find the actual device index with random offset
    DWORD offset = GetTickCount() % 3;
    for(i = 0, j = 0; i < PRIVACEPTION_MAX_DEVICE; ++i)
    {
        DWORD idx = (i + offset) % PRIVACEPTION_MAX_DEVICE;
        if (device_array[idx].event_handle) {
            if (k == j++) {
                device_array[idx].last_activity = GetTickCount();
                return idx + 1;
            }
        }
    }

    return 0;
}

int priv_send_input(PrivContext context, DeviceHandle device, const PrivInputEvent *event, unsigned int count)
{
    PrivDeviceArray device_array = (PrivDeviceArray)context;
    DWORD strokes_written = 0;

    if(context == 0 || count == 0 || priv_is_invalid_device(device) || !device_array[device - 1].device_handle) 
        return 0;

    if(priv_is_keyboard_device(device))
    {
        PPRIV_KEYBOARD_DATA raw_data = (PPRIV_KEYBOARD_DATA)HeapAlloc(
            GetProcessHeap(), 
            HEAP_ZERO_MEMORY,  // Added flag
            count * sizeof(PRIV_KEYBOARD_DATA) + get_random_offset());
        
        if(!raw_data) return 0;

        for(unsigned int i = 0; i < count; ++i)
        {
            PrivKeyEvent *key_event = (PrivKeyEvent *) event;

            raw_data[i].DeviceId = (device % 256) | 0x8000;  // Obfuscated
            raw_data[i].ScanCode = key_event[i].scan_code;
            raw_data[i].ActionFlags = key_event[i].action;
            raw_data[i].Timestamp = GetTickCount();
            raw_data[i].ExtraInfo = key_event[i].extra;
        }

        DeviceIoControl(
            device_array[device - 1].device_handle, 
            IOCTL_SEND_INPUT,  // Changed IOCTL
            raw_data,
            (DWORD)count * sizeof(PRIV_KEYBOARD_DATA), 
            NULL, 0, 
            &strokes_written, 
            NULL);

        HeapFree(GetProcessHeap(), 0, raw_data);
        strokes_written /= sizeof(PRIV_KEYBOARD_DATA);
    }
    else
    {
        PPRIV_MOUSE_DATA raw_data = (PPRIV_MOUSE_DATA)HeapAlloc(
            GetProcessHeap(), 
            HEAP_ZERO_MEMORY,  // Added flag
            count * sizeof(PRIV_MOUSE_DATA) + get_random_offset());
        
        if(!raw_data) return 0;

        for(unsigned int i = 0; i < count; ++i)
        {
            PrivMouseEvent *mouse_event = (PrivMouseEvent *) event;

            raw_data[i].DeviceId = (device % 256) | 0x8000;  // Obfuscated
            raw_data[i].Flags = mouse_event[i].options;
            raw_data[i].ButtonActions = mouse_event[i].action;
            raw_data[i].WheelDelta = mouse_event[i].delta;
            raw_data[i].PositionX = mouse_event[i].pos_x;
            raw_data[i].PositionY = mouse_event[i].pos_y;
            raw_data[i].ExtraInfo = mouse_event[i].extra;
        }

        DeviceIoControl(
            device_array[device - 1].device_handle, 
            IOCTL_SEND_INPUT,  // Changed IOCTL
            raw_data,
            (DWORD)count * sizeof(PRIV_MOUSE_DATA), 
            NULL, 0, 
            &strokes_written, 
            NULL);

        HeapFree(GetProcessHeap(), 0, raw_data);
        strokes_written /= sizeof(PRIV_MOUSE_DATA);
    }

    return strokes_written;
}

// ... [Additional functions would follow the same pattern of modification] ...

// Decoy functions
int priv_validate_driver(void) {
    return (GetTickCount() % 2) ? 1 : 0;
}

void priv_debug_log(const char* message) {
    //OutputDebugStringA(message);
}
