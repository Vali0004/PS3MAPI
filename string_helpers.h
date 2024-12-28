#pragma once
#include <string>
#include <vector>

inline std::string find_replace(const std::string& str, const std::string& find, const std::string& replace)
{
	std::string result{ str };
	size_t pos{};

	while ((pos = result.find(find, pos)) != std::string::npos)
	{
		result.replace(pos, find.length(), replace);
		pos += replace.length();
	}

	return result;
}

inline std::vector<std::string> split_string(const std::string& str, char split)
{
	std::vector<std::string> strings{};
	std::string string{};
	for (size_t i{}; i != str.length(); ++i)
	{
		if (str.at(i) == split)
		{
			strings.push_back(string);
			string.erase();
		}
		else
		{
			string += str.at(i);
			if (i == str.length() - 1)
				strings.push_back(string);
		}
	}
	return strings;
}