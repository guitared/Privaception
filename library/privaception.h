#ifndef _PRIVACEPTION_H_
#define _PRIVACEPTION_H_

#ifdef PRIVACEPTION_STATIC
    #define PRIVACEPTION_API
#else
    #if defined _WIN32 || defined __CYGWIN__
        #ifdef PRIVACEPTION_EXPORT
            #ifdef __GNUC__
                #define PRIVACEPTION_API __attribute__((dllexport))
            #else
                #define PRIVACEPTION_API __declspec(dllexport)
            #endif
        #else
            #ifdef __GNUC__
                #define PRIVACEPTION_API __attribute__((dllimport))
            #else
                #define PRIVACEPTION_API __declspec(dllimport)
            #endif
        #endif
    #else
        #if __GNUC__ >= 4
            #define PRIVACEPTION_API __attribute__ ((visibility("default")))
        #else
            #define PRIVACEPTION_API
        #endif
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Changed maximum device counts and calculation method
#define PRIVACEPTION_MAX_KEYBOARD 12  // Changed from 10
#define PRIVACEPTION_MAX_MOUSE 8      // Changed from 10
#define PRIVACEPTION_MAX_DEVICE ((PRIVACEPTION_MAX_KEYBOARD * 2) + (PRIVACEPTION_MAX_MOUSE / 2))  // Different calculation

// Changed identifier macros
#define PRIVACEPT_KEYBOARD(index) ((index) * 2 + 5)          // Changed calculation
#define PRIVACEPT_MOUSE(index) ((PRIVACEPTION_MAX_KEYBOARD * 3) + (index) * 4 + 3)  // More complex

typedef void *PrivContext;  // Changed type name
typedef int DeviceHandle;   // Changed type name
typedef int PriorityLevel;  // Changed type name
typedef unsigned short EventFilter;  // Changed type name
typedef int (*DevicePredicate)(DeviceHandle device);  // Changed type name

// Changed enum names and values
enum PrivKeyAction {
    PRIV_KEY_PRESS       = 0x10,  // Changed from 0x00
    PRIV_KEY_RELEASE     = 0x11,  // Changed from 0x01
    PRIV_KEY_EXTENDED0   = 0x12,  // Changed from 0x02
    PRIV_KEY_EXTENDED1   = 0x14,  // Changed from 0x04
    PRIV_KEY_LED_UPDATE  = 0x18,  // Changed from 0x08
    PRIV_KEY_SHADOW_INPUT = 0x20, // Changed from 0x10
    PRIV_KEY_VIRTUAL_PKT = 0x40   // Changed from 0x20
};

// Changed filter enum names and shifted values differently
enum PrivKeyFilter {
    PRIV_FILTER_KEY_NONE        = 0x0000,
    PRIV_FILTER_KEY_ALL         = 0xFFFF,
    PRIV_FILTER_KEY_PRESS       = PRIV_KEY_PRESS << 2,      // Different shift
    PRIV_FILTER_KEY_RELEASE     = PRIV_KEY_RELEASE << 3,    // Different shift
    PRIV_FILTER_KEY_EXTENDED0   = PRIV_KEY_EXTENDED0 << 1,  // Different shift
    PRIV_FILTER_KEY_EXTENDED1   = PRIV_KEY_EXTENDED1 << 2,  // Different shift
    PRIV_FILTER_KEY_LED_UPDATE  = PRIV_KEY_LED_UPDATE >> 1, // Different operation
    PRIV_FILTER_KEY_SHADOW_INPUT= PRIV_KEY_SHADOW_INPUT,
    PRIV_FILTER_KEY_VIRTUAL_PKT = PRIV_KEY_VIRTUAL_PKT << 1
};

// Changed mouse state enum names and values
enum PrivMouseAction {
    PRIV_MOUSE_LEFT_DOWN    = 0x101,  // Changed from 0x001
    PRIV_MOUSE_LEFT_UP      = 0x202,  // Changed from 0x002
    PRIV_MOUSE_RIGHT_DOWN   = 0x404,  // Changed from 0x004
    PRIV_MOUSE_RIGHT_UP     = 0x808,  // Changed from 0x008
    PRIV_MOUSE_MIDDLE_DOWN  = 0x1010, // Changed from 0x010
    PRIV_MOUSE_MIDDLE_UP    = 0x2020, // Changed from 0x020

    PRIV_MOUSE_BTN1_DOWN    = PRIV_MOUSE_LEFT_DOWN,
    PRIV_MOUSE_BTN1_UP      = PRIV_MOUSE_LEFT_UP,
    PRIV_MOUSE_BTN2_DOWN    = PRIV_MOUSE_RIGHT_DOWN,
    PRIV_MOUSE_BTN2_UP      = PRIV_MOUSE_RIGHT_UP,
    PRIV_MOUSE_BTN3_DOWN    = PRIV_MOUSE_MIDDLE_DOWN,
    PRIV_MOUSE_BTN3_UP      = PRIV_MOUSE_MIDDLE_UP,

    PRIV_MOUSE_BTN4_DOWN    = 0x4040,  // Changed from 0x040
    PRIV_MOUSE_BTN4_UP      = 0x8080,  // Changed from 0x080
    PRIV_MOUSE_BTN5_DOWN    = 0x1001,  // Changed from 0x100
    PRIV_MOUSE_BTN5_UP      = 0x2002,  // Changed from 0x200

    PRIV_MOUSE_WHEEL        = 0x4004,  // Changed from 0x400
    PRIV_MOUSE_HWHEEL       = 0x8008   // Changed from 0x800
};

// Changed filter enum with different values
enum PrivMouseFilter {
    PRIV_FILTER_MOUSE_NONE         = 0x0000,
    PRIV_FILTER_MOUSE_ALL          = 0xFFFF,

    PRIV_FILTER_MOUSE_LEFT_DOWN    = PRIV_MOUSE_LEFT_DOWN,
    PRIV_FILTER_MOUSE_LEFT_UP      = PRIV_MOUSE_LEFT_UP,
    PRIV_FILTER_MOUSE_RIGHT_DOWN   = PRIV_MOUSE_RIGHT_DOWN,
    PRIV_FILTER_MOUSE_RIGHT_UP     = PRIV_MOUSE_RIGHT_UP,
    PRIV_FILTER_MOUSE_MIDDLE_DOWN  = PRIV_MOUSE_MIDDLE_DOWN,
    PRIV_FILTER_MOUSE_MIDDLE_UP    = PRIV_MOUSE_MIDDLE_UP,

    PRIV_FILTER_MOUSE_BTN1_DOWN    = PRIV_MOUSE_BTN1_DOWN,
    PRIV_FILTER_MOUSE_BTN1_UP      = PRIV_MOUSE_BTN1_UP,
    PRIV_FILTER_MOUSE_BTN2_DOWN    = PRIV_MOUSE_BTN2_DOWN,
    PRIV_FILTER_MOUSE_BTN2_UP      = PRIV_MOUSE_BTN2_UP,
    PRIV_FILTER_MOUSE_BTN3_DOWN    = PRIV_MOUSE_BTN3_DOWN,
    PRIV_FILTER_MOUSE_BTN3_UP      = PRIV_MOUSE_BTN3_UP,

    PRIV_FILTER_MOUSE_BTN4_DOWN    = PRIV_MOUSE_BTN4_DOWN,
    PRIV_FILTER_MOUSE_BTN4_UP      = PRIV_MOUSE_BTN4_UP,
    PRIV_FILTER_MOUSE_BTN5_DOWN    = PRIV_MOUSE_BTN5_DOWN,
    PRIV_FILTER_MOUSE_BTN5_UP      = PRIV_MOUSE_BTN5_UP,

    PRIV_FILTER_MOUSE_WHEEL        = PRIV_MOUSE_WHEEL,
    PRIV_FILTER_MOUSE_HWHEEL       = PRIV_MOUSE_HWHEEL,

    PRIV_FILTER_MOUSE_MOVE         = 0x1001  // Changed from 0x1000
};

// Changed mouse flag enum names and values
enum PrivMouseFlags {
    PRIV_MOUSE_RELATIVE       = 0x010,  // Changed from 0x000
    PRIV_MOUSE_ABSOLUTE       = 0x021,  // Changed from 0x001
    PRIV_MOUSE_VIRTUAL_SCREEN = 0x042,  // Changed from 0x002
    PRIV_MOUSE_ATTR_CHANGE    = 0x084,  // Changed from 0x004
    PRIV_MOUSE_NO_COALESCE    = 0x108,  // Changed from 0x008
    PRIV_MOUSE_SHADOW_SOURCE  = 0x210   // Changed from 0x100
};

// Changed structure names and added padding
typedef struct {
    unsigned short action;  // Changed from 'state'
    unsigned short options; // Changed from 'flags'
    short delta;           // Changed from 'rolling'
    int pos_x;             // Changed from 'x'
    int pos_y;             // Changed from 'y'
    unsigned int extra;    // Changed from 'information'
    unsigned char pad[8];  // Added padding
} PrivMouseEvent;          // Changed from InterceptionMouseStroke

typedef struct {
    unsigned short scan_code;  // Changed from 'code'
    unsigned short action;     // Changed from 'state'
    unsigned int extra;        // Changed from 'information'
    unsigned int timestamp;    // Added new field
} PrivKeyEvent;               // Changed from InterceptionKeyStroke

typedef char PrivInputEvent[sizeof(PrivMouseEvent) + 16]; // Changed name and size

// Changed all function names and some parameter types
PRIVACEPTION_API PrivContext priv_create_context(void);  // Changed from interception_create_context

PRIVACEPTION_API void priv_destroy_context(PrivContext ctx);  // Changed

PRIVACEPTION_API PriorityLevel priv_get_priority(PrivContext ctx, DeviceHandle dev);  // Changed

PRIVACEPTION_API void priv_set_priority(PrivContext ctx, DeviceHandle dev, PriorityLevel level);  // Changed

PRIVACEPTION_API EventFilter priv_get_filter(PrivContext ctx, DeviceHandle dev);  // Changed

PRIVACEPTION_API void priv_set_filter(PrivContext ctx, DevicePredicate pred, EventFilter filter);  // Changed

PRIVACEPTION_API DeviceHandle priv_wait(PrivContext ctx);  // Changed

PRIVACEPTION_API DeviceHandle priv_wait_timeout(PrivContext ctx, unsigned long ms);  // Changed name

PRIVACEPTION_API int priv_send_input(PrivContext ctx, DeviceHandle dev, const PrivInputEvent *event, unsigned int count);  // Changed

PRIVACEPTION_API int priv_receive_input(PrivContext ctx, DeviceHandle dev, PrivInputEvent *event, unsigned int count);  // Changed

PRIVACEPTION_API unsigned int priv_get_device_id(PrivContext ctx, DeviceHandle dev, void *id_buffer, unsigned int buffer_size);  // Changed

PRIVACEPTION_API int priv_is_invalid_device(DeviceHandle dev);  // Changed

PRIVACEPTION_API int priv_is_keyboard_device(DeviceHandle dev);  // Changed

PRIVACEPTION_API int priv_is_mouse_device(DeviceHandle dev);  // Changed

// Added decoy functions
PRIVACEPTION_API int priv_validate_driver(void);  // Added new
PRIVACEPTION_API void priv_debug_log(const char* message);  // Added new

#ifdef __cplusplus
}
#endif

#endif