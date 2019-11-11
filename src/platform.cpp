#include "platform/platform.h"

#if defined(__linux__)
#    include <sys/resource.h>
#    include <cerrno>
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
#else
#    error Platform not supported.
#endif
}
