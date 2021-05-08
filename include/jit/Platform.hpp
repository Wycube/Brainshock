#if defined(_WIN32)
    #define PLATFORM_WINDOWS
    #include <windows.h>
    #warning "Windows platform detected"
#elif defined(__linux__)
    #define PLATFORM_LINUX
    #include <sys/mman.h>
    #include <unistd.h>
    #warning "Linux platform detected"
#elif defined(__APPLE__)
    #define PLATFORM_MACOS
    #include <sys/mman.h>
    #include <unistd.h>
    #warning "MacOS platform detected"
#else
    #error "Unsupported platform"
#endif