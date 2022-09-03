// libgba expects stdbool.h and stdint.h which armcc lacks.
#if !defined(__ARMCC_VERSION)
#  include_next <stdbool.h>
#else
#ifndef STDBOOL_H_POLYFILL
#define STDBOOL_H_POLYFILL
// close enough
#define _Bool unsigned char
#define bool _Bool

#define true 1
#define false 0

#define __bool_true_false_are_defined 1

#endif // STDBOOL_H_POLYFILL
#endif // __ARMCC_VERSION
