#include <sys/time.h>

#include "ElapsedTimer.h"

namespace ngrest {

int64_t ElapsedTimer::getTime()
{
    struct timeval now;
    gettimeofday(&now, nullptr);
    return now.tv_sec * 1000000 + now.tv_usec;
}

} // namespace ngrest
