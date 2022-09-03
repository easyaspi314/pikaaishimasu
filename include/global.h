#ifndef GLOBAL_H
#define GLOBAL_H

#ifndef __GNUC__
#  define __attribute__(x) // disable GCC attributes in libgba headers
#endif

#ifdef __ARMCC_VERSION
// In the original, the system registers must be treated as normal pointers.
// Since armcc is bad at optimizing global variable usage, it pretty much gets it
// right anyways but it will still emit different code.
// This will include gba_types.h manually to trick the header into not using volatile
// for the vu32/vu8/etc typedefs.
// The places where volatile is required will explicitly use the volatile keyword.
#  ifndef MODERN
#    define volatile // nothing
#    include <gba_types.h>
#    undef volatile
#  endif
#  define inline __inline
#endif

// We only need these headers.
// Some libgba headers actually error (primarily ones with inline asm) 
#include <gba_base.h>
#include <gba_input.h>
#include <gba_sprites.h>
#include <gba_timers.h>
#include <gba_video.h>

// Slight bug: The original code treats this as a u32 instead of a u16.
// It has no effect in practice.
#ifndef MODERN
typedef vu32 dispcnt_p_t;
#  define REG_DISPCNT32 *((vu32 *)REG_BASE)
#else
typedef vu16 dispcnt_p_t;
#  define REG_DISCPNT32 REG_DISPCNT
#endif

// optimization: Some globals are constants
#ifdef MODERN
#  define CONSTANT const
#else
#  define CONSTANT // nothing
#endif

#endif // GLOBAL_H
