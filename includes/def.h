#ifndef Y_DEF_H
#define Y_DEF_H

#define Y_SUCCESS (0)
#define Y_FAILED (-1)
#define Y_IS_SUCCESS(expr) (expr == Y_SUCCESS)
#define Y_IS_FAILED(expr) (expr == Y_FAILED)

/* define base type */
namespace y {
using __ushort = unsigned short;
}

/* define return type */
namespace y {
using RET = int;
}

#endif // Y_DEF_H