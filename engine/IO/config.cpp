#include "config.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "utils.h"

using json = nlohmann::json;

namespace engine
{
	bool read_config(const std::wstring& path, s_config* const config, const s_config* const default_config)
	{
		if (!config || !default_config)
			return false;

		if (!file_exists(path))
			memcpy(config, default_config, sizeof(s_config));

		json j;
		bool config_valid = true;

		std::ifstream ifs(path);

		ifs >> j;

		if (j.count("window_mode") > 0)
			config->window_mode = j["window_mode"];
		else
		{
			config->window_mode = default_config->window_mode;
			config_valid = false;
		}
			
		if (j.count("resolution_width") > 0)
			config->resolution_width = j["resolution_width"];
		else
		{
			config->resolution_width = default_config->resolution_width;
			config_valid = false;
		}

		if (j.count("resolution_height") > 0)
			config->resolution_height = j["resolution_height"];
		else
		{
			config->resolution_height = default_config->resolution_height;
			config_valid = false;
		}

		return config_valid;
	}

	bool write_config(const std::wstring& path, s_config* const config)
	{
		if (!config)
			return false;
		json j;

		j["window_mode"] = config->window_mode;
		j["resolution_width"] = config->resolution_width;
		j["resolution_height"] = config->resolution_height;

		std::ofstream ofs(path, std::ofstream::trunc);

		ofs << j;

		ofs.close();

		return true;
	}
}