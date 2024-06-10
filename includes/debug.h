#ifndef Y_DEBUG_H
#define Y_DEBUG_H

#include <stdio.h>

#define PRINT_DEBUG(fmt, ARGS...) printf(fmt, ##ARGS)

#endif // Y_DEBUG_H