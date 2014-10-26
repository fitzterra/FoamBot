/*
 * Basic debug header.
 *
 * Define DEBUG before including this header to enable debugging.
 */

#include "config.h"
#include "Streaming.h"

#ifdef DEBUG
	#define D(x) Serial << x
#else
	#define D(x)
#endif
