/*
 * Basic debug header.
 *
 * This will provide a convenient D() macro to use for writing debug messages
 * to the Serial stream. The arguments to the D() macro is the same as for the
 * Serial stream when using the Streaming.h macros.
 *
 * The D() macro will only be defined for debug message output if _DEBUG has
 * been defined before including this file. For this reason, this file expects
 * a "config.h" file to exists which it will include. The current project can
 * use this config file to configure itself in addition to using it to set the
 * debug state on or off.
 *
 * Define _DEBUG before including this header to enable debugging.
 */

#include "config.h"
#include <Streaming.h>

#ifdef _DEBUG
	#define D(x) Serial << x
#else
	#define D(x)
#endif
