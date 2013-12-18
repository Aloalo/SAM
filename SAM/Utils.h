#pragma once

#include <string>

//host utils
namespace utils
{
	enum
	{
		MIRROR = 0,
		GLASS = 1,
		WALL = 2,
		FLOOR = 3,
		EMPTY = 4
	};

	const std::string pathToPTX(const std::string &filename)
	{
		return "../SAM/ptxfiles/" + filename + ".ptx";
	}
};