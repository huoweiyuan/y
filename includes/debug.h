#ifndef Y_DEBUG_H
#define Y_DEBUG_H

#ifdef NDEBUG

#define COST_TIME(PROCESSOR, STR)                                              \
  do {                                                                         \
  } while (0) #else

#else

#include <stdio.h>

#define COST_TIME(PROCESSOR, STR)                                              \
  do {                                                                         \
    using Clock = std::chrono::high_resolution_clock;                          \
    std::chrono::time_point<Clock> t1, t2;                                     \
    t1 = Clock::now();                                                         \
    PROCESSOR                                                                  \
    t2 = Clock::now();                                                         \
    printf(                                                                    \
        "Cost: %lld.\t%s\n",                                                   \
        std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count(), \
        STR);                                                                  \
  } while (0)

#endif

#endif // Y_DEBUG_H