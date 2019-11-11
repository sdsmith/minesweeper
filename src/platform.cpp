#include "platform/platform.h"

#if defined(__linux__)
#    include <sys/resource.h>
#    include <cerrno>
#    include <cstring>
#elif defined(_WIN32)
#    include <Windows.h>
#    include <strsafe.h>
#    include <cstring>
#endif

void Platform::set_process_to_high_priority() const
{
#if defined(__linux__)
    rlimit cur_nice_limit = {};
    if (getrlimit(RLIMIT_NICE, &cur_nice_limit) < 0) {
        printf("Unable to get process nice limit: %s\n", strerror(errno));
    }

    const int nice_ceiling = 20 - static_cast<int>(cur_nice_limit.rlim_cur);

    errno = 0; // clear for getpriority
    const int cur_pri = getpriority(PRIO_PROCESS, 0);
    if (errno != 0) {
        printf("Unable to get process priority: %s\n", strerror(errno));
    }

    if (cur_pri <= nice_ceiling) {
        printf("Process is as nice as it can be (%d)\n", cur_pri);
        return;
    }

    if (setpriority(PRIO_PROCESS, 0, nice_ceiling) < 0) {
        printf("Unable to set process to high priority: %s\n", strerror(errno));
        return;
    }

    printf("Set process nice level to %d (was %d)\n", nice_ceiling, cur_pri);

#elif defined(_WIN32)
    // TODO(stewarts): Must have PROCESS_SET_INFORMATION permission, check with
    // GetSecurityInfo

    if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS)) {
        print_windows_error(TEXT("SetPriorityClass"));
        return;
    }
#else
#    error Platform not supported.
#endif
}

#if defined(_WIN32)
void Platform::print_windows_error(LPCTSTR function_name) const
{
    /*
    // Retrieve error message from system
    //
    LPSTR msg_buf;
    LPVOID display_buf;
    DWORD dw = GetLastError();

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS,
                  nullptr, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  msg_buf, 0, nullptr);

    // Print error message
    //
    display_buf = static_cast<LPVOID>(LocalAlloc(
        LMEM_ZEROINIT, (std::strlen(msg_buf) +
                        std::strlen(static_cast<LPCTSTR>(function_name)) + 40) *
                           sizeof(TCHAR)));
    if (!display_buf) {
        printf("Failed to allocate buffer\n");
        return;
    }

    StringCchPrintf(static_cast<LPTSTR>(display_buf), //-V111 -V576
                    LocalSize(display_buf) / sizeof(TCHAR),
                    TEXT("%s failed with error %d: %s"), function_name, dw,
                    msg_buf);

    static_assert(std::is_same<TCHAR, char>::value,
                  "Unicode Windows strings are not compatible with the logger");
    printf("%s\n", static_cast<char*>(display_buf));

    LocalFree(msg_buf);
    LocalFree(display_buf);
    */
}
#endif
