#ifndef NGREST_ELAPSEDTIMER_H
#define NGREST_ELAPSEDTIMER_H

#include <stdint.h>

namespace ngrest {

/**
 * @brief class to calculate elapsed time
 */
class ElapsedTimer
{
public:
    inline ElapsedTimer(bool startNow = false)
    {
        if (startNow)
            start();
    }

    /**
     * @brief start measuring
     */
    inline void start()
    {
        started = getTime();
    }

    /**
     * @brief returns the number of microseconds since this ElapsedTimer was started
     */
    inline int64_t elapsed() const
    {
        return getTime() - started;
    }

    /**
     * @brief get current time in microseconds
     */
    static int64_t getTime();

private:
    int64_t started = 0;
};

} // namespace ngrest

#endif // NGREST_ELAPSEDTIMER_H
