#ifndef __ENGINE_ENGINE_DEBUG_H
#define __ENGINE_ENGINE_DEBUG_H

#include <iostream>

#ifdef _DEBUG
#define ENGINE_DEBUG 1
#else
#define ENGINE_DEBUG 0
#endif

#define debug_printf(fmt, ...) do { if (ENGINE_DEBUG) printf(fmt, __VA_ARGS__); } while (0)

#define debug_fprintf(fmt, ...) do { if (ENGINE_DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)


#endif