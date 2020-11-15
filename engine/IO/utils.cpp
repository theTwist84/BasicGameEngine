#include "utils.h"
#include <algorithm>
#include <exception>
#include <Shlwapi.h>
#include <fstream>
#include "../engine_definitions.h"

namespace engine
{
	void replace_backward_slashes(std::string& input)
	{
		std::replace(input.begin(), input.end(), '\\', '/');
	}

	std::string current_directory()
	{
		WCHAR buffer[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, buffer);
		std::wstring current_directoryW(buffer);
		return std::string(current_directoryW.begin(), current_directoryW.end());
	}

	std::wstring executable_directory()
	{
		WCHAR buffer[MAX_PATH];
		GetModuleFileName(nullptr, buffer, MAX_PATH);
		PathRemoveFileSpec(buffer);
		return std::wstring(buffer);
	}

	void get_filename(const std::string& input_path, std::string& filename)
	{
		std::string full_path(input_path);
		replace_backward_slashes(full_path);

		std::string::size_type last_slash_index = full_path.find_last_of('/');

		if (last_slash_index == std::string::npos)
			filename = full_path;
		else
			filename = full_path.substr(last_slash_index + 1, full_path.size() - last_slash_index - 1);
	}

	void get_directory(const std::string& input_path, std::string& directory)
	{
		std::string full_path(input_path);
		replace_backward_slashes(full_path);

		std::string::size_type last_slash_index = full_path.find_last_of('/');

		if (last_slash_index == std::string::npos)
			directory = "";
		else
			directory = full_path.substr(0, last_slash_index);
	}

	void get_filename_directory(const std::string& input_path, std::string& directory, std::string& filename)
	{
		std::string full_path(input_path);
		replace_backward_slashes(full_path);

		std::string::size_type last_slash_index = full_path.find_last_of('/');

		if (last_slash_index == std::string::npos)
		{
			directory = "";
			filename = full_path;
		}
		else
		{
			directory = full_path.substr(0, last_slash_index);
			filename = full_path.substr(last_slash_index + 1, full_path.size() - last_slash_index - 1);
		}
	}

	bool file_exists(const std::wstring& file_path)
	{
		return PathFileExists(file_path.c_str());
	}

	void load_binary_file(const std::wstring& filename, std::vector<char>* data)
	{
		if(!file_exists(filename))
			throw std::exception("Failed to find file.");

		std::ifstream file(filename.c_str(), std::ios::binary);
		if (file.bad() || !((bool)file))
		{
			throw std::exception("Failed to open file.");
		}

		file.seekg(0, std::ios::end);
		uint32 size = (uint32)file.tellg();

		if (size > 0)
		{
			data->resize(size);
			file.seekg(0, std::ios::beg);
			file.read(&(data->front()), size);
		}
		file.close();
	}

	void to_wide_string(const std::string& source, std::wstring& dest)
	{
		dest.assign(source.begin(), source.end());
	}

	std::wstring to_wide_string(const std::string& source)
	{
		std::wstring dest;
		dest.assign(source.begin(), source.end());
		return dest;
	}

	void path_join(std::wstring& dest, const std::wstring& source_directory, const std::wstring& source_file)
	{
		WCHAR buffer[MAX_PATH];
		PathCombine(buffer, source_directory.c_str(), source_file.c_str());
		dest = buffer;
	}

	void get_path_extension(const std::wstring& source, std::wstring& dest)
	{
		dest = PathFindExtension(source.c_str());
	}
}