#pragma once

#include <string>

//host utils
namespace utils
{
	const std::string pathToPTX(const std::string &filename)
	{
		return "SAM/ptxfiles/" + filename + ".ptx";
	}
};