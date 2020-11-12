#ifndef ENGINE_IO_UTILS_H
#define ENGINE_IO_UTILS_H

#include <string>
#include <windows.h>
#include <vector>

namespace engine
{
	/// <summary>
	/// Get the path of the current directory
	/// </summary>
	/// <returns></returns>
	std::string current_directory();

	/// <summary>
	/// Get the path to the executable's directory
	/// </summary>
	/// <returns></returns>
	std::wstring executable_directory();

	/// <summary>
	/// Get the filename from the input path.
	/// </summary>
	/// <param name="input_path"></param>
	/// <param name="filename"></param>
	void get_filename(const std::string& input_path, std::string& filename);

	/// <summary>
	/// Get directory from input path, excluding the filename.
	/// </summary>
	/// <param name="input_path"></param>
	/// <param name="directory"></param>
	void get_directory(const std::string& input_path, std::string& directory);

	/// <summary>
	/// Get filename and directory from path
	/// </summary>
	/// <param name="input_path"></param>
	/// <param name="directory"></param>
	/// <param name="filename"></param>
	void get_filename_directory(const std::string& input_path, std::string& directory, std::string& filename);

	/// <summary>
	/// Load a binary file and return its content in a char* with
	/// </summary>
	/// <param name="filename"></param>
	/// <param name="data"></param>
	void load_binary_file(const std::wstring& filename, std::vector<char>* data);

	/// <summary>
	/// Convert std::string to std::wstring
	/// </summary>
	/// <param name="source"></param>
	/// <param name="dest"></param>
	void to_wide_string(const std::string& source, std::wstring& dest);

	/// <summary>
	/// Convert std::string to std::wstring
	/// </summary>
	/// <param name="source"></param>
	/// <returns></returns>
	std::wstring to_wide_string(const std::string& source);

	/// <summary>
	/// Join 2 paths (dir and file)
	/// </summary>
	/// <param name="dest"></param>
	/// <param name="source_directory"></param>
	/// <param name="source_file"></param>
	void path_join(std::wstring& dest, const std::wstring& source_directory, const std::wstring& source_file);

	/// <summary>
	/// Get extension of file from path
	/// </summary>
	/// <param name="source"></param>
	/// <param name="dest"></param>
	void get_path_extension(const std::wstring& source, std::wstring& dest);

	bool file_exists(const std::wstring& file_path);
}

#endif

