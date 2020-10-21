#ifndef ENGINE_GLOBALS_ENGINE_GLOBALS_H
#define ENGINE_GLOBALS_ENGINE_GLOBALS_H

namespace engine
{
	class c_engine_time;
	class c_engine_clock;
}

namespace rendering
{
	class c_renderer;
}

namespace engine
{
	struct s_engine_globals
	{
		rendering::c_renderer* renderer;
		c_engine_time* engine_time;
		c_engine_clock* engine_clock;
	};


	s_engine_globals* const get_engine_globals();

}


#endif
