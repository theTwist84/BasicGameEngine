#include "engine_globals.h"

namespace engine
{
	s_engine_globals g_engine;

	s_engine_globals* const get_engine_globals() { return &g_engine; }

}