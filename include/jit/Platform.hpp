#if defined(_WIN32)
    #ifndef PLATFORM_WINDOWS
        #define PLATFORM_WINDOWS
        #include <windows.h>
        #warning "Windows platform detected"
    #endif
#elif defined(__linux__)
    #ifndef PLATFORM_LINUX
        #define PLATFORM_LINUX
        #include <sys/mman.h>
        #include <unistd.h>
        #warning "Linux platform detected"
    #endif
#elif defined(__APPLE__)
    #ifndef PLATFORM_MACOS
        #define PLATFORM_MACOS
        #include <sys/mman.h>
        #include <unistd.h>
        #warning "MacOS platform detected"
    #endif
#else
    #error "Unsupported platform"
#endif