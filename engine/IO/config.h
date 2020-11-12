#ifndef ENGINE_IO_CONFIG_H
#define ENGINE_IO_CONFIG_H

#include "../engine_definitions.h"
#include <string>


namespace engine
{
	struct s_config
	{
		byte window_mode;
		int32 resolution_width;
		int32 resolution_height;
	};

	bool read_config(const std::wstring& path, s_config* const config, const s_config* const default_config);
	bool write_config(const std::wstring& path, s_config* const config);
}

#endif