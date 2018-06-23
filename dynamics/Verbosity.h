#pragma once

#include <cstdio>

enum
{
	quiet,
	terse,
	verbose,
	debug
};

namespace Verbosity
{
	extern int level;

	inline bool is_quiet()
	{
		return level == quiet;
	}

	inline bool is_terse()
	{
		return level == terse;
	}

	inline bool is_verbose()
	{
		return level == verbose;
	}

	inline bool is_debug()
	{
		return level == debug;
	}

	inline void set(int _level)
	{
		std::printf("setting verbosity to ");

		level = _level;

		if (level <= quiet) level = quiet;
		if (level >= debug) level = debug;

		switch (level)
		{
		case quiet:
			std::printf("quiet.  \n");
			break;
		case terse:
			std::printf("terse.  \n");
			break;
		case verbose:
			std::printf("verbose.\n");
			break;
		default:
			std::printf("debug.  \n");
		}

		std::fflush(stdout);
	}
}
