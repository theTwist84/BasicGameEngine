#ifndef ENGINE_ENGINE_SETTINGS_H 
#define ENGINE_ENGINE_SETTINGS_H

#include "engine_definitions.h"

namespace engine
{
	struct s_renderer_settings
	{
		enum e_window_type : byte
		{
			_window_type_fullscreen,
			_window_type_fullscreen_windowed,
			_window_type_windowed
		};

		enum e_anti_aliasing_type : byte
		{
			_anti_aliasing_off,
			_anti_aliasing_msaa
		};

		uint32 width;
		uint32 height;

		e_window_type window_type;
		e_anti_aliasing_type anti_aliasing_type;
		bool use_vsync;
		uint8 : 8;

		uint32 multi_sampling_sample_count;
		uint32 multi_sampling_quality_level;

	};
	static_assert(sizeof(s_renderer_settings) == 0x14);


	struct s_camera_settings
	{
		float32 field_of_view_x;
		float32 field_of_view_y;
	};

}


#endif